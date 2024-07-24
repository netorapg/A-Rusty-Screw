#ifndef __MY_LIB_ANY_HEADER_H__
#define __MY_LIB_ANY_HEADER_H__

#include <type_traits>

#include <cstdint>
#include <cstring>

#include <my-lib/macros.h>
#include <my-lib/std.h>

namespace Mylib
{

// ---------------------------------------------------

/*
	I don't like std::any because the C++ standard doesn't require
	it support static memory allocation.
	So I wrote this simpler version of it with static memory.
*/

template <uint32_t minimum_storage_size, uint32_t alignment=__STDCPP_DEFAULT_NEW_ALIGNMENT__>
class MYLIB_ALIGN_STRUCT(alignment) Any
{
public:
	consteval static uint32_t size ()
	{
		return ((minimum_storage_size % alignment) == 0)
		? minimum_storage_size
		: static_cast<uint32_t>((minimum_storage_size + alignment) / alignment) * alignment;
	}

private:
	uint8_t data[ size() ];

public:
	Any () noexcept = default;

	template <typename T>
	Any (const T& value) noexcept
	{
		static_assert(sizeof(T) <= size());
		memcpy(this, &value, sizeof(T));
	}

	// -----------------------

	template <typename T>
	Any& operator= (const T& value) noexcept
	{
		static_assert(sizeof(T) <= size());
		memcpy(this, &value, sizeof(T));
		return *this;
	}

	// -----------------------

	// T must be explicitly set when calling
	template <typename T>
	T& get_ref () noexcept
	{
		static_assert(sizeof(T) <= size());
		return *(reinterpret_cast<T*>(this));
	}

	// T must be explicitly set when calling
	template <typename T>
	const T& get_ref () const noexcept
	{
		static_assert(sizeof(T) <= size());
		return *(reinterpret_cast<const T*>(this));
	}

	// T must be explicitly set when calling
	template <typename T>
	T get_value () const noexcept
	{
		static_assert(sizeof(T) <= size());
		return *(reinterpret_cast<const T*>(this));
	}
};

// ---------------------------------------------------

} // end namespace Mylib

#endif