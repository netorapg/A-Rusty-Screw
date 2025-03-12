#ifndef __MY_LIB_MEMORY_HEADER_H__
#define __MY_LIB_MEMORY_HEADER_H__

#include <iostream>
#include <initializer_list>
#include <vector>
#include <memory>

#include <cstdint>
#include <cstdlib>
#include <cassert>

#include <my-lib/macros.h>
#include <my-lib/std.h>

namespace Mylib
{
namespace Memory
{

// ---------------------------------------------------

[[nodiscard]] inline void* m_allocate (const size_t size, const uint32_t align)
{
	void *p;
#ifdef __cpp_aligned_new
	if (align > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
		p = ::operator new(size, std::align_val_t(align));
	else
		p = ::operator new(size);
#else
	#error "aligned new not supported"
#endif
//	std::cout << "\tstd align " << __STDCPP_DEFAULT_NEW_ALIGNMENT__ << std::endl;
//	std::cout << "m_allocate size " << size << " align " << align << " address " << p << std::endl;
	return p;
}

// ---------------------------------------------------

inline void m_deallocate (void *p, const size_t size, const uint32_t align)
{
//std::cout << "m_deallocate size " << size << " align " << align << " address " << p << std::endl;
#if __cpp_aligned_new
	if (align > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
		::operator delete(p,
	#if __cpp_sized_deallocation
		size,
	#endif
		std::align_val_t(align));
		return;
	}
#endif
	::operator delete(p
#if __cpp_sized_deallocation
	, size
#endif
	);
}

// ---------------------------------------------------

template <typename T>
constexpr uint32_t calculate_alignment () noexcept
{
	if constexpr (alignof(T) <= __STDCPP_DEFAULT_NEW_ALIGNMENT__)
		return __STDCPP_DEFAULT_NEW_ALIGNMENT__;
	else
		return alignof(T);
}

// ---------------------------------------------------

class Manager
{
public:
	[[nodiscard]] virtual void* allocate (const size_t type_size, const size_t count, const uint32_t align) = 0;
	virtual void deallocate (void *p, const size_t type_size, const size_t count, const uint32_t align) = 0;

	[[nodiscard]] inline void* allocate (const size_t type_size, const size_t count)
	{
		return this->allocate(type_size, count, __STDCPP_DEFAULT_NEW_ALIGNMENT__);
	}

	void deallocate (void *p, const size_t type_size, const size_t count)
	{
		this->deallocate(p, type_size, count, __STDCPP_DEFAULT_NEW_ALIGNMENT__);
	}

	template <typename T>
	[[nodiscard]] T* allocate_type (const size_t count)
	{
		return static_cast<T*>( this->allocate(sizeof(T), count, calculate_alignment<T>()) );
	}

	template <typename T>
	void deallocate_type (T *p, const size_t count)
	{
		this->deallocate(p, sizeof(T), count, calculate_alignment<T>());
	}

	template <typename T>
	void destruct_deallocate_type (T *p)
	{
		p->~T();
		this->deallocate_type<T>(p, 1);
	}
};

// ---------------------------------------------------

/*
	Custom STL allocator interface for Memory::Manager.
	Based on GCC Standard C++ Library.
	<include>/c++/11/ext/new_allocator.h

	Requires that the number of elements to be allocated at a time is 1.
*/

template <typename T>
class AllocatorSTL
{
public:
	using value_type = T;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using propagate_on_container_move_assignment = std::true_type;

	Manager *manager;

	AllocatorSTL () = delete;

	AllocatorSTL (Manager& manager_)
		: manager(&manager_)
	{
	}

	AllocatorSTL (const AllocatorSTL& other)
		: manager(other.manager)
	{
	}

	template <typename Tother>
	AllocatorSTL (const AllocatorSTL<Tother>& other)
		: manager(other.manager)
	{
	}

	[[nodiscard]] T* allocate (const size_type n, const void* = static_cast<const void*>(0))
	{
	#if __cplusplus >= 201103L
		static_assert(sizeof(T) != 0, "cannot allocate incomplete types");
	#endif

		return this->manager->template allocate_type<T>(n);
	}

	void deallocate (T *p, const size_type n)
	{
		this->manager->template deallocate_type<T>(p, n);
	}

	template<typename Tother>
	friend constexpr bool operator== (const AllocatorSTL&, const AllocatorSTL<Tother>&) noexcept
	{
		return true;
	}

	#if __cpp_impl_three_way_comparison < 201907L
	template <typename Tother>
	friend constexpr bool operator!= (const AllocatorSTL&, const AllocatorSTL<_Up>&) noexcept
	{
		return false;
	}
	#endif
};

// ---------------------------------------------------

// To be used with unique_ptr

template <typename T>
class DeAllocatorSTL
{
public:
	Manager *manager;

	DeAllocatorSTL () = delete;

	DeAllocatorSTL (Manager& manager_)
		: manager(&manager_)
	{
	}

	template <typename Tother>
	DeAllocatorSTL (const DeAllocatorSTL<Tother>& other)
		: manager(other.manager)
	{
	}

	DeAllocatorSTL (const AllocatorSTL<T>& allocator)
		: manager(allocator.manager)
	{
	}

	template <typename Tother>
	DeAllocatorSTL (const AllocatorSTL<Tother>& other)
		: manager(other.manager)
	{
	}

	void operator() (T *p)
	{
		this->manager->template deallocate_type<T>(p, 1);
	}
};

// ---------------------------------------------------

template <typename T>
using unique_ptr = std::unique_ptr<T, DeAllocatorSTL<T>>;

template <typename T, typename... Types>
[[nodiscard]] unique_ptr<T> make_unique (Manager& manager, Types&&... vars)
{
	T *ptr = new (manager.template allocate_type<T>(1)) T(std::forward<Types>(vars)...);
	return unique_ptr<T>(ptr, DeAllocatorSTL<T>(manager));
}

// ---------------------------------------------------

class DefaultManager : public Manager
{
public:
	[[nodiscard]] void* allocate (const size_t type_size, const size_t count, const uint32_t align) override final
	{
		return m_allocate(type_size * count, align);
	}

	void deallocate (void *p, const size_t type_size, const size_t count, const uint32_t align) override final
	{
		m_deallocate(p, type_size * count, align);
	}
};

// ---------------------------------------------------

inline DefaultManager default_manager;
inline AllocatorSTL<int> default_allocator_stl(default_manager);

// ---------------------------------------------------


} // end namespace Memory
} // end namespace Mylib

#endif