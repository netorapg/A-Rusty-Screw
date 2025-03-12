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
#include <my-lib/event.h>
#include <my-lib/memory.h>
#include <my-lib/coroutine.h>


namespace Mylib
{
namespace Event
{

// ---------------------------------------------------

template <typename Tget_current_time>
class Timer
{
public:
	using Ttime = typename remove_type_qualifiers< decltype(std::declval<Tget_current_time>()()) >::type;

	static consteval bool debug () noexcept
	{
		return false;
	}

	struct Event {
		Ttime time;
		bool re_schedule;
	};

	struct Descriptor__ {
		Event *ptr = nullptr;
	};

	struct Descriptor {
		std::shared_ptr<Descriptor__> shared_ptr;

		bool is_valid () const noexcept
		{
			return (this->shared_ptr && this->shared_ptr->ptr != nullptr);
		}
	};

	struct CoroutineAwaiter {
		Timer& timer;
		const Ttime time;
		CoroutineHandle handler;

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

		void await_suspend (CoroutineHandle handler)
		{
			this->handler = handler;
			PromiseType& promise = handler.promise();
			//Coroutine coro = promise.get_return_object();

			EventFull *event = new (this->timer.memory_manager.template allocate_type<EventFull>(1)) EventFull;
			event->time = this->time;
			event->var_callback = EventCoroutine {
				.coroutine_handler = handler
			},
			event->enabled = true;

			// Store the event in the coroutine promise.
			// We do this to be able to destroy the event when the coroutine is unregistered.
			promise.awaiter_owner = &this->timer;
			promise.awaiter_data = event;

			if constexpr (debug()) std::cout << "await_suspend current.time " << this->timer.get_current_time() << " event.time " << event->time << std::endl;

			this->timer.push(event);
		}

		// await_resume is called when the coroutine is resumed.
		// Return void since we don't have a value to return to the caller.

		constexpr void await_resume () const noexcept
		{
			PromiseType& promise = this->handler.promise();
			promise.awaiter_owner = nullptr;
			promise.awaiter_data = nullptr;
			//std::cout << "\t\tawait_resume" << std::endl;
		}
	};

	friend struct CoroutineAwaiter;

private:
	using TimerCallback = Callback<Event>;

	struct EventCallback {
		Descriptor descriptor;
		TimerCallback *callback;
	};

	struct EventCoroutine {
		CoroutineHandle coroutine_handler;
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
	std::vector<Internal> events; // we let the vector use its standard allocators

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
					
					if (event->enabled) {
						if constexpr (debug()) std::cout << "\tresume coroutine time=" << event->time << std::endl;
					
						event_coro.coroutine_handler.resume(); // resume automatically sets promise owner to nullptr
					}
				}
				else
					mylib_throw_exception_msg("invalid event callback type");

				if (event->re_schedule)
					this->push(event);
				else
					this->destroy_event(event); // coroutine always fall here
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
		using TallocatorDescriptor = Memory::AllocatorSTL<Descriptor__>;
		TallocatorDescriptor descriptor_allocator(this->memory_manager);

		auto unique_ptr = callback.make_copy(this->memory_manager);

		EventFull *event = new (this->memory_manager.template allocate_type<EventFull>(1)) EventFull;
		event->time = time;

		std::shared_ptr<Descriptor__> shared_ptr_ = std::allocate_shared<Descriptor__>(descriptor_allocator, Descriptor__ {
			.ptr = event
		});

		event->var_callback = EventCallback {
			.descriptor = Descriptor { .shared_ptr = shared_ptr_ },
			.callback = unique_ptr.release(),
		},
		event->enabled = true;

		// We store a raw pointer and release the unique_ptr.
		// We do this because the pool memory manager doesn't support
		// polymorphic types.
		// I intend to change this in the future.

		this->push(event);
		
		return Descriptor { .shared_ptr = std::move(shared_ptr_) };
	}

	inline void unschedule_event (Descriptor& descriptor)
	{
		EventFull *event = static_cast<EventFull*>(descriptor.shared_ptr->ptr);
		event->enabled = false; // better than rebuild the heap
		descriptor.shared_ptr->ptr = nullptr;
		descriptor.shared_ptr.reset();
	}

	inline void force_resume_coroutine (Coroutine coro)
	{
		PromiseType& promise = coro.handler.promise();

		if (promise.awaiter_owner == static_cast<void*>(this)) {
			EventFull *event = static_cast<EventFull*>(promise.awaiter_data);
			event->enabled = false; // better than rebuild the heap
			coro.handler.resume(); // resume automatically sets promise owner to nullptr
		}
	}

	inline void unregister_coroutine (Coroutine coro)
	{
		PromiseType& promise = coro.handler.promise();

		if (promise.awaiter_owner == static_cast<void*>(this)) {
			EventFull *event = static_cast<EventFull*>(promise.awaiter_data);
			event->enabled = false; // better than rebuild the heap
			promise.awaiter_owner = nullptr;
			promise.awaiter_data = nullptr;
		}
	}

	CoroutineAwaiter coroutine_wait_until (const Ttime& time)
	{
		return CoroutineAwaiter {
			.timer = *this,
			.time = time
		};
	}

	template <typename Tduration>
	CoroutineAwaiter coroutine_wait (const Tduration& time)
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
			callback.callback->destruct_deallocate_memory(this->memory_manager);
			callback.descriptor.shared_ptr->ptr = nullptr;
		}
		this->memory_manager.template destruct_deallocate_type<EventFull>(event);
	}
};

// ---------------------------------------------------

} // end namespace Event
} // end namespace Mylib

#endif