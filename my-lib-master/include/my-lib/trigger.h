#ifndef __MY_LIB_TRIGGER_HEADER_H__
#define __MY_LIB_TRIGGER_HEADER_H__

#include <iostream>
#include <list>
#include <functional>
#include <type_traits>

#include <cstdint>

#include <my-lib/macros.h>
#include <my-lib/std.h>
#include <my-lib/memory.h>

namespace Mylib
{
namespace Trigger
{

#ifdef MYLIB_TRIGGER_BASE_OPERATIONS
#error nooooooooooo
#endif

// ---------------------------------------------------

template <typename Tevent>
class Callback
{
public:
	virtual void operator() (Tevent& event) = 0;
	virtual ~Callback () = default;
	virtual std::size_t get_size () const noexcept = 0; // returns object size
	virtual uint32_t get_alignment () const noexcept = 0; // returns object alignment
	virtual Callback<Tevent>* make_copy (Memory::Manager& memory_manager) const = 0;
	virtual void deconstruct_free_memory (Memory::Manager& memory_manager) = 0;
};

#define MYLIB_TRIGGER_BASE_OPERATIONS \
	public: \
		std::size_t get_size () const noexcept override final \
		{ \
			return sizeof(*this); \
		} \
		uint32_t get_alignment () const noexcept override final \
		{ \
			return alignof(decltype(*this)); \
		} \
		Callback<Tevent>* make_copy (Memory::Manager& memory_manager) const override final \
		{ \
			return new (memory_manager.allocate(this->get_size(), 1, this->get_alignment())) DerivedCallback(*this); \
		} \
		void deconstruct_free_memory (Memory::Manager& memory_manager) override final \
		{ \
			this->~DerivedCallback(); \
			memory_manager.deallocate(this, this->get_size(), 1, this->get_alignment()); \
		}

// ---------------------------------------------------

/*
	Template parameter Tevent must be explicitly set.
	Function should be:
	void Tfunc (Tevent&);
*/

template <typename Tevent, typename Tfunc>
auto make_callback_function (Tfunc callback)
{
	class DerivedCallback : public Callback<Tevent>
	{
	private:
		Tfunc callback_function;

	public:
		DerivedCallback (Tfunc callback_function_)
			: callback_function(callback_function_)
		{
		}

		void operator() (Tevent& event) override
		{
			/*auto built_params = std::tuple_cat(
				std::forward_as_tuple(this->obj),
				std::forward_as_tuple(event)
			);
			std::apply(this->callback_function, built_params);*/
			std::invoke(this->callback_function, event);
		}

		MYLIB_TRIGGER_BASE_OPERATIONS
	};

	return DerivedCallback(callback);
}

// ---------------------------------------------------

/*
	Template parameter Tevent must be explicitly set.
	Object function should be:
	void Tobj::Tfunc (const Tevent&);
*/

template <typename Tevent, typename Tobj, typename Tfunc>
auto make_callback_object (Tobj& obj, Tfunc callback)
{
	class DerivedCallback : public Callback<Tevent>
	{
	private:
		Tobj& obj;
		Tfunc callback_function;

	public:
		DerivedCallback (Tobj& obj_, Tfunc callback_function_)
			: obj(obj_), callback_function(callback_function_)
		{
		}

		void operator() (Tevent& event) override
		{
			/*auto built_params = std::tuple_cat(
				std::forward_as_tuple(this->obj),
				std::forward_as_tuple(event)
			);
			std::apply(this->callback_function, built_params);*/
			std::invoke(this->callback_function, this->obj, event);
		}

		MYLIB_TRIGGER_BASE_OPERATIONS
	};

	return DerivedCallback(obj, callback);
}

// ---------------------------------------------------

/*
	Template parameter Tevent must be explicitly set.
	Object function should be:
	void Tobj::Tfunc (const Tevent&, const Tfilter&);
*/

template <typename Tevent, typename Tfilter_, typename Tobj, typename Tfunc>
auto make_filter_callback_object (Tfilter_&& filter, Tobj& obj, Tfunc callback)
{
	using Tfilter = typename remove_type_qualifiers<Tfilter_>::type;

	class DerivedCallback : public Callback<Tevent>
	{
	private:
		Tfilter filter;
		Tobj& obj;
		Tfunc callback_function;

	public:
		DerivedCallback (Tfilter& filter_, Tobj& obj_, Tfunc callback_function_)
			: filter(filter_), obj(obj_), callback_function(callback_function_)
		{
		}

		void operator() (Tevent& event) override
		{
			if (this->filter(event)) {
				/*auto built_params = std::tuple_cat(
					std::forward_as_tuple(this->obj),
					std::forward_as_tuple(event),
					std::forward_as_tuple(this->filter)
				);
				std::apply(this->callback_function, built_params);*/
				std::invoke(this->callback_function, this->obj, event, this->filter);
			}
		}

		MYLIB_TRIGGER_BASE_OPERATIONS
	};

	return DerivedCallback(filter, obj, callback);
}

// ---------------------------------------------------

/*
	Template parameter Tevent must be explicitly set.
	Object function should be:
	void Tobj::Tfunc (const Tevent&, parameters...);

	Important, the first parameter is the Event Data (Tevent).
*/

template <typename Tevent, typename Tobj, typename Tfunc, typename Tfirst_param, typename... Args>
auto make_callback_object_with_params (Tobj& obj, Tfunc callback, const Tfirst_param& first_param, Args&&... args)
{
	//auto params = std::make_tuple(first_param, std::forward<Args>(args)...);
	auto params = std::make_tuple(first_param, args...);

	using Tparams = decltype(params);

	class DerivedCallback : public Callback<Tevent>
	{
	private:
		Tobj& obj;
		Tfunc callback_function;
		Tparams callback_params;
	
	public:
		DerivedCallback (Tobj& obj_, Tfunc callback_function_, Tparams callback_params_)
			: obj(obj_), callback_function(callback_function_), callback_params(callback_params_)
		{
		}

		void operator() (Tevent& event) override
		{
			auto built_params = std::tuple_cat(
				std::forward_as_tuple(this->obj), // can use std::tie as well
				std::forward_as_tuple(event),
				this->callback_params
			);
			
			std::apply(this->callback_function, built_params);
			//std::invoke(this->callback_function, *(this->obj));
		}

		MYLIB_TRIGGER_BASE_OPERATIONS
	};

	return DerivedCallback(obj, callback, params);
}

// ---------------------------------------------------

/*
	Template parameter Tevent must be explicitly set.
	Object function should be:
	void Tobj::Tfunc (const Tevent&, const Tfilter&, parameters...);

	Important, the first parameter is the Event Data (Tevent).
	Important, the second parameter is the Filter Data (Tfilter).
*/

template <typename Tevent, typename Tfilter_, typename Tobj, typename Tfunc, typename Tfirst_param, typename... Args>
auto make_filter_callback_object_with_params (Tfilter_&& filter, Tobj& obj, Tfunc callback, const Tfirst_param& first_param, Args&&... args)
{
	using Tfilter = typename remove_type_qualifiers<Tfilter_>::type;

	//auto params = std::make_tuple(first_param, std::forward<Args>(args)...);
	auto params = std::make_tuple(first_param, args...);

	using Tparams = decltype(params);

	class DerivedCallback : public Callback<Tevent>
	{
	private:
		Tfilter filter;
		Tobj& obj;
		Tfunc callback_function;
		Tparams callback_params;
	
	public:
		DerivedCallback (Tfilter& filter_, Tobj& obj_, Tfunc callback_function_, Tparams callback_params_)
			: filter(filter_), obj(obj_), callback_function(callback_function_), callback_params(callback_params_)
		{
		}

		void operator() (Tevent& event) override
		{
			if (this->filter(event)) {
				auto built_params = std::tuple_cat(
					std::forward_as_tuple(this->obj), // can use std::tie as well
					std::forward_as_tuple(event),
					std::forward_as_tuple(this->filter),
					this->callback_params
				);
				
				std::apply(this->callback_function, built_params);
				//std::invoke(this->callback_function, *(this->obj));
			}
		}

		MYLIB_TRIGGER_BASE_OPERATIONS
	};

	return DerivedCallback(filter, obj, callback, params);
}

// ---------------------------------------------------

template <typename Tevent>
class EventHandler
{
public:
	using Type = Tevent;
	using EventCallback = Callback<Tevent>;

	struct Subscriber {
		EventCallback *callback;
		bool enabled;
	};

	struct Descriptor {
		Subscriber *subscriber;
	};

private:
	Memory::Manager& memory_manager;
	//using TallocSubscriber = typename std::allocator_traits<Talloc>::template rebind_alloc<Subscriber>;
	using TallocSubscriber = Memory::AllocatorSTL<Subscriber>;
	TallocSubscriber subscriber_allocator;
	std::list<Subscriber, TallocSubscriber> subscribers;

public:
	EventHandler ()
		: memory_manager(Memory::default_manager),
		  subscriber_allocator(memory_manager),
		  subscribers(subscriber_allocator)
	{
	}

	EventHandler (Memory::Manager& memory_manager_)
		: memory_manager(memory_manager_),
		  subscriber_allocator(memory_manager),
		  subscribers(subscriber_allocator)
	{
	}

	~EventHandler ()
	{
		for (auto& subscriber : this->subscribers)
			subscriber.callback->deconstruct_free_memory(this->memory_manager);
	}

	// We don't use const Tevent& because we allow the user to manipulate event data.
	// This is useful for the timer, allowing us to re-schedule events.
	void publish (Tevent& event)
	{
		for (auto& subscriber : this->subscribers) {
			if (subscriber.enabled) {
				auto& c = *(subscriber.callback);
				c(event);
			}
		}
	}

	inline void publish (Tevent&& event)
	{
		this->publish(event);
	}

	/* When creating the event listener by r-value ref,
	   we allocate internal storage and copy the value to it.
	*/
	Descriptor subscribe (const EventCallback& callback)
		//requires std::is_rvalue_reference<decltype(callback)>::value
	{
		this->subscribers.push_back( Subscriber {
			.callback = callback.make_copy(this->memory_manager),
			.enabled = true
			} );

		return Descriptor { .subscriber = &this->subscribers.back() };
	}

	void unsubscribe (const Descriptor& descriptor)
	{
		this->subscribers.remove_if(
			[&descriptor, this] (Subscriber& subscriber) -> bool {
				bool found = (descriptor.subscriber == &subscriber);

				if (found)
					subscriber.callback->deconstruct_free_memory(this->memory_manager);
				
				return found;
			}
		);
	}
};

// ---------------------------------------------------

#undef MYLIB_TRIGGER_BASE_OPERATIONS

} // end namespace Trigger
} // end namespace Mylib

#endif