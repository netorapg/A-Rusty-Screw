#ifndef __MY_LIB_MATH_HEADER_H__
#define __MY_LIB_MATH_HEADER_H__

#include <numeric>
#include <numbers>

#include <my-lib/std.h>

namespace Mylib
{
namespace Math
{

// ---------------------------------------------------

template <typename T>
constexpr T base2_log_of_integer (const T value)
{
	constexpr uint32_t nbits = sizeof(T) * 8;

	for (uint32_t pos = 0; pos < nbits; pos++) {
		if ((value >> pos) & 0x01)
			return pos;
	}

	mylib_throw_exception_msg("Mylib::base2_log_of_integer\nNumber ", value, " cant be zero");
}

constexpr auto radians_to_degrees (const auto radians) noexcept -> decltype(radians)
{
	using Type = decltype(radians);
	return (radians * static_cast<Type>(180)) / std::numbers::pi_v<Type>;
}

constexpr auto degrees_to_radians (const auto degrees) noexcept -> decltype(degrees)
{
	using Type = decltype(degrees);
	return (degrees / static_cast<Type>(180)) * std::numbers::pi_v<Type>;
}

template <typename Tout>
constexpr Tout round_to_nearest (const auto v) noexcept
{
	return static_cast<Tout>(v + static_cast<decltype(v)>(0.5));
}

// ---------------------------------------------------

} // end namespace Math
} // end namespace Mylib

#endif