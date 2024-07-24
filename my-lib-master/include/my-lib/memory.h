#ifndef __MY_LIB_MEMORY_HEADER_H__
#define __MY_LIB_MEMORY_HEADER_H__

#include <iostream>
#include <initializer_list>
#include <vector>

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
//	std::cout << "m_allocate size " << size << " align " << align << std::endl;
#if __cpp_aligned_new
	if (align > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
		return ::operator new(size, std::align_val_t(align));
#endif
//	std::cout << "\tstd align " << __STDCPP_DEFAULT_NEW_ALIGNMENT__ << std::endl;
	return ::operator new(size);
}

// ---------------------------------------------------

inline void m_deallocate (void *p, const size_t size, const uint32_t align)
{
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
consteval uint32_t calculate_alignment ()
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

	Manager& manager;

	AllocatorSTL (Manager& manager_)
		: manager(manager_)
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

		return this->manager.template allocate_type<T>(n);
	}

	void deallocate (T *p, const size_type n)
	{
		this->manager.template deallocate_type<T>(p, n);
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
//inline AllocatorSTL<int> default_allocator_stl(default_manager);

// ---------------------------------------------------


} // end namespace Memory
} // end namespace Mylib

#endif