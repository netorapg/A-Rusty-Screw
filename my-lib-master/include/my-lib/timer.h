#ifndef __MY_LIB_TIMER_HEADER_H__
#define __MY_LIB_TIMER_HEADER_H__

#include <vector>
#include <algorithm>
#include <memory>
#include <utility>
#include <variant>
#include <coroutine>

#include <cstdint>
#include <cstdlib>

#include <my-lib/macros.h>
#include <my-lib/std.h>
#include <my-lib/trigger.h>
#include <my-lib/memory.h>

namespace Mylib
{
namespace Trigger
{

// ---------------------------------------------------

template <typename Tget_current_time>
class Timer
{
public:
	using Ttime = typename remove_type_qualifiers< decltype(std::declval<Tget_current_time>()()) >::type;

	static consteval bool debug ()
	{
		return false;
	}

	struct Event {
		Ttime time;
		bool re_schedule;
	};

	struct Descriptor {
		Event *ptr;
	};

	struct Coroutine {
		struct promise_type {
			Coroutine get_return_object ()
			{
				return Coroutine {
					.handler = std::coroutine_handle<promise_type>::from_promise(*this)
				};
			}

			std::suspend_always initial_suspend () const noexcept { return {}; }
			std::suspend_always final_suspend () const noexcept { return {}; }
			void return_void() const noexcept {}
			void unhandled_exception () { std::terminate(); }
		};

		struct Awaiter {
			Timer& timer;
			const Ttime time;

			// await_ready is called before the coroutine is suspended.
			// We return false to tell the caller to suspend.

			constexpr bool await_ready () const noexcept
			{
				return false;
			}

			// await_suspend is called when the coroutine is suspended.
			// Return void to tell the caller to suspend.
			// Only at this point we have access to the coroutine handler so we can
			// resume the coroutine later, when the timer expires.

			void await_suspend (std::coroutine_handle<promise_type> handler)
			{
				auto& promise = handler.promise();
				Coroutine coro = promise.get_return_object();

				EventFull *event = new (this->timer.memory_manager.template allocate_type<EventFull>(1)) EventFull;
				event->time = this->time;
				event->var_callback = EventCoroutine {
					.coro = coro,
				},
				event->enabled = true;

				if constexpr (debug()) std::cout << "await_suspend current.time " << this->timer.get_current_time() << " event.time " << event->time << std::endl;

				this->timer.push(event);
			}

			// await_resume is called when the coroutine is resumed.
			// Return void since we don't have a value to return to the caller.

			constexpr void await_resume () const noexcept { }
		};

		std::coroutine_handle<promise_type> handler;
	};

private:
	friend class Coroutine::Awaiter;

	using TimerCallback = Callback<Event>;

	struct EventCallback {
		TimerCallback *callback;
	};

	struct EventCoroutine {
		Coroutine coro;
	};

	struct EventFull : public Event {
		std::variant<EventCallback, EventCoroutine> var_callback;
		bool enabled;
	};

	/*
		Since we store pointers to the events, not the events themselves,
		we need a way to compare the event.time values to keep the heap property.
		If nothing is done, STL will use the pointer addresses to compare the events.
		The easist way to solve this is to encampulate the event in a struct,
		and define the operator< for the struct.
	*/

	struct Internal {
		EventFull *event_full;

		inline bool operator< (const Internal& rhs) const noexcept
		{
			return (this->event_full->time > rhs.event_full->time);
		}
	};

	Tget_current_time get_current_time_;
	Memory::Manager& memory_manager;
	std::vector<Internal> events; // we let the vector use its standard allocator

public:
	Timer (Tget_current_time get_current_time__)
		: get_current_time_(get_current_time__),
		  memory_manager(Memory::default_manager)
	{
	}

	Timer (Tget_current_time get_current_time__, Memory::Manager& memory_manager_)
		: get_current_time_(get_current_time__),
		  memory_manager(memory_manager_)
	{
	}

	~Timer ()
	{
		for (auto& internal : this->events)
			this->destroy_event(internal.event_full);
	}

	inline Ttime get_current_time () const
	{
		return this->get_current_time_();
	}

	inline uint32_t get_n_scheduled_events () const
	{
		return this->events.size();
	}

	void trigger_events ()
	{
		const Ttime time = this->get_current_time();

		if constexpr (debug()) {
			std::cout << "trigger_events time=" << time << " n_events " << this->get_n_scheduled_events() << std::endl;

			for (auto& internal : this->events) {
				EventFull *event = internal.event_full;
				std::cout << "\tevent.time=" << event->time << std::endl;
			}
		}

		while (!this->events.empty()) {
			EventFull *event = this->events.front().event_full;

			if (event->time <= time) {
				this->pop();
				event->re_schedule = false;
				
				if (std::holds_alternative<EventCallback>(event->var_callback)) {
					EventCallback& callback = std::get<EventCallback>(event->var_callback);
					auto& c = *(callback.callback);

					if constexpr (debug()) std::cout << "\tcallback time=" << event->time << std::endl;

					if (event->enabled)
						c(*event);
				}
				else if (std::holds_alternative<EventCoroutine>(event->var_callback)) {
					EventCoroutine& event_coro = std::get<EventCoroutine>(event->var_callback);
					
					if constexpr (debug()) std::cout << "\tresume coroutine time=" << event->time << std::endl;
					
					event_coro.coro.handler.resume();
				}
				else
					mylib_throw_exception_msg("invalid event callback type");

				if (event->re_schedule)
					this->push(event);
				else
					this->destroy_event(event);
			}
			else
				break;
		}
	}

	/*
		When creating the event listener by r-value ref,
		we allocate internal storage and copy the value to it.
	*/
	Descriptor schedule_event (const Ttime& time, const TimerCallback& callback)
		//requires std::is_rvalue_reference<decltype(callback)>::value
	{
		EventFull *event = new (this->memory_manager.template allocate_type<EventFull>(1)) EventFull;
		event->time = time;
		event->var_callback = EventCallback {
			.callback = callback.make_copy(this->memory_manager),
		},
		event->enabled = true;

		this->push(event);
		
		return Descriptor { .ptr = event };
	}

	inline void unschedule_event (Descriptor& descriptor)
	{
		EventFull *event = static_cast<EventFull*>(descriptor.ptr);
		event->enabled = false; // better than rebuild the heap
	}

	void register_coroutine (Coroutine coro)
	{
		// We created the coroutine in a suspended state.
		// We need to resume it to start the execution.
		coro.handler.resume();
	}

	Coroutine::Awaiter coroutine_wait_until (const Ttime& time)
	{
		return typename Coroutine::Awaiter {
			.timer = *this,
			.time = time
		};
	}

	template <typename Tduration>
	Coroutine::Awaiter coroutine_wait (const Tduration& time)
	{
		return this->coroutine_wait_until(this->get_current_time() + time);
	}

private:
	inline void push (EventFull *event)
	{
		this->events.push_back( Internal { event } );
		std::push_heap(this->events.begin(), this->events.end());

		if constexpr (debug()) std::cout << "push event.time=" << event->time << " n_events " << this->get_n_scheduled_events() << std::endl;
	}

	inline void pop ()
	{
		std::pop_heap(this->events.begin(), this->events.end());
		this->events.pop_back();
	}

	inline void destroy_event (EventFull *event)
	{
		if (std::holds_alternative<EventCallback>(event->var_callback)) {
			EventCallback& callback = std::get<EventCallback>(event->var_callback);
			callback.callback->deconstruct_free_memory(this->memory_manager);
		}
		this->memory_manager.template deallocate_type<EventFull>(event, 1);
	}
};

// ---------------------------------------------------

} // end namespace Trigger
} // end namespace Mylib

#endif