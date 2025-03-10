#ifndef __MY_LIB_EVENT_HEADER_H__
#define __MY_LIB_EVENT_HEADER_H__

#include <iostream>
#include <list>
#include <functional>
#include <type_traits>
#include <memory>

#include <cstdint>

#include <my-lib/macros.h>
#include <my-lib/std.h>
#include <my-lib/memory.h>

namespace Mylib
{
namespace Event
{

#ifdef MYLIB_EVENT_BASE_OPERATIONS
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
	virtual Mylib::Memory::unique_ptr<Callback<Tevent>> make_copy (Memory::Manager& memory_manager) const = 0;
	virtual void destruct_deallocate_memory (Memory::Manager& memory_manager) = 0;
};

#define MYLIB_EVENT_BASE_OPERATIONS \
	public: \
		constexpr std::size_t get_size () const noexcept override final \
		{ \
			return sizeof(*this); \
		} \
		constexpr uint32_t get_alignment () const noexcept override final \
		{ \
			return alignof(decltype(*this)); \
		} \
		Mylib::Memory::unique_ptr<Callback<Tevent>> make_copy (Memory::Manager& memory_manager) const override final \
		{ \
			return Mylib::Memory::make_unique<DerivedCallback>(memory_manager, *this); \
		} \
		void destruct_deallocate_memory (Memory::Manager& memory_manager) override final \
		{ \
			memory_manager.template destruct_deallocate_type<DerivedCallback>(this); \
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

		MYLIB_EVENT_BASE_OPERATIONS
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

		MYLIB_EVENT_BASE_OPERATIONS
	};

	return DerivedCallback(obj, callback);
}

// ---------------------------------------------------

template <typename Tevent, typename Tlambda_>
auto make_callback_lambda (Tlambda_&& callback)
{
	using Tlambda = typename remove_type_qualifiers<Tlambda_>::type;

	class DerivedCallback : public Callback<Tevent>
	{
	private:
		Tlambda callback_lambda;

	public:
		DerivedCallback (Tlambda_ callback_lambda_)
			: callback_lambda(callback_lambda_)
		{
		}

		void operator() (Tevent& event) override
		{
			/*auto built_params = std::tuple_cat(
				std::forward_as_tuple(this->obj),
				std::forward_as_tuple(event)
			);
			std::apply(this->callback_function, built_params);*/
			std::invoke(this->callback_lambda, event);
		}

		MYLIB_EVENT_BASE_OPERATIONS
	};

	return DerivedCallback(callback);
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

		MYLIB_EVENT_BASE_OPERATIONS
	};

	return DerivedCallback(obj, callback, params);
}

// ---------------------------------------------------

template <typename Tevent>
class Handler
{
public:
	using Type = Tevent;
	using EventCallback = Callback<Tevent>;

	struct Subscriber;

	struct Descriptor__ {
		Subscriber *subscriber = nullptr;
	};

	struct Descriptor {
		std::shared_ptr<Descriptor__> shared_ptr;

		bool is_valid () const noexcept
		{
			return (this->shared_ptr && this->shared_ptr->subscriber != nullptr);
		}
	};

	struct Subscriber {
		Descriptor descriptor;
		EventCallback *callback;
	};

private:
	Memory::Manager *memory_manager;
	//using TallocSubscriber = typename std::allocator_traits<Talloc>::template rebind_alloc<Subscriber>;
	using TallocSubscriber = Memory::AllocatorSTL<Subscriber>;
	TallocSubscriber subscriber_allocator;
	std::list<Subscriber, TallocSubscriber> subscribers;

public:
	Handler ()
		: memory_manager(&Memory::default_manager),
		  subscriber_allocator(*memory_manager),
		  subscribers(subscriber_allocator)
	{
	}

	Handler (Memory::Manager& memory_manager_)
		: memory_manager(&memory_manager_),
		  subscriber_allocator(*memory_manager),
		  subscribers(subscriber_allocator)
	{
	}

	~Handler ()
	{
		for (auto& subscriber : this->subscribers) {
			subscriber.descriptor.shared_ptr->subscriber = nullptr;
			subscriber.callback->destruct_deallocate_memory(*this->memory_manager);
		}
	}

	// We don't use const Tevent& because we allow the user to manipulate event data.
	// This is useful for the timer, allowing us to re-schedule events.
	void publish (Tevent& event)
	{
		for (auto& subscriber : this->subscribers) {
			auto& c = *(subscriber.callback);
			c(event);
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
		auto unique_ptr = callback.make_copy(*this->memory_manager);

		this->subscribers.push_back( Subscriber {
			.callback = unique_ptr.release(),
			} );
		
		// We store a raw pointer and release the unique_ptr.
		// We do this because the pool memory manager doesn't support
		// polymorphic types.
		// I intend to change this in the future.
		
		Subscriber& subscriber = this->subscribers.back();

		subscriber.descriptor = Descriptor {
			.shared_ptr = std::allocate_shared<Descriptor__>(this->subscriber_allocator, Descriptor__ {
				.subscriber = &subscriber
			})
		};

		return subscriber.descriptor;
	}

	void unsubscribe (Descriptor& descriptor)
	{
		bool found = false;

		this->subscribers.remove_if(
			[&descriptor, &found, this] (Subscriber& subscriber) -> bool {
				const bool local_found = (descriptor.shared_ptr->subscriber == &subscriber);

				if (local_found) {
					subscriber.callback->destruct_deallocate_memory(*this->memory_manager);
					found = true;
				}
				
				return local_found;
			}
		);

		mylib_assert_exception_msg(found, "subscriber not found");

		descriptor.shared_ptr->subscriber = nullptr;
		descriptor.shared_ptr.reset();
	}
};

// ---------------------------------------------------

#undef MYLIB_EVENT_BASE_OPERATIONS

} // end namespace Event
} // end namespace Mylib

#endif