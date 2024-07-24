#ifndef __MY_LIB_STD_HEADER_H__
#define __MY_LIB_STD_HEADER_H__

#include <concepts>
#include <type_traits>
#include <iostream>
#include <sstream>
#include <ostream>
#include <string>
#include <string_view>
#include <stdexcept>
#include <utility>

#include <cstdint>

#if !(defined(__clang__) || defined(__GNUC__) || defined(_MSC_VER))
	#warning Untested compiler
#endif

namespace Mylib
{

// ---------------------------------------------------

template<typename T>
using remove_type_qualifiers = typename std::remove_cvref<T>;
//using remove_type_qualifiers = typename std::remove_cv<typename std::remove_reference<T>::type>;

// no idea why the following causes issues
//template<typename T>
//using remove_type_qualifierss = typename remove_type_qualifiers<T>::type;

// ---------------------------------------------------

using size_t = std::size_t;

// ---------------------------------------------------

constexpr void build_str_from_stream__ (std::ostringstream& str_stream) noexcept
{
}

template <typename T, typename... Types>
void build_str_from_stream__ (std::ostringstream& str_stream, T&& var1, Types&&... vars)
{
//	using namespace std;
	str_stream << var1;
	build_str_from_stream__(str_stream, vars...);
}

template <typename... Types>
std::string build_str_from_stream (Types&&... vars)
{
	std::ostringstream str_stream;
	build_str_from_stream__(str_stream, vars...);
	return str_stream.str();
}

// ---------------------------------------------------

constexpr void print_stream__ (std::ostream& out) noexcept
{
}

template <typename T, typename... Types>
void print_stream__ (std::ostream& out, T&& var1, Types&&... vars)
{
//	using namespace std;
	out << var1;
	print_stream__(out, vars...);
}

template <typename... Types>
void print_stream (std::ostream& out, Types&&... vars)
{
	print_stream__(out, vars...);
}

// ---------------------------------------------------

class Exception : public std::exception
{
private:
	std::string msg;

public:
	Exception (std::string&& msg)
	{
		this->msg = std::move(msg);
	}

	Exception (const std::string& msg)
	{
		this->msg = msg;
	}

	Exception (const char *msg)
	{
		this->msg = msg;
	}

	const char* what() const noexcept override
	{
		return this->msg.data();
	}
};

#define mylib_throw_exception_msg(...) \
	throw Mylib::Exception( Mylib::build_str_from_stream("exception throw at file ", __FILE__, " line ", __LINE__, '\n', __VA_ARGS__, '\n') )

#define mylib_assert_exception_diecode_msg(bool_expr, die_code, ...) \
	if (!(bool_expr)) [[unlikely]] { \
		die_code \
		mylib_throw_exception_msg("assert failed", '\n', #bool_expr, '\n', __VA_ARGS__); \
	}

#define mylib_assert_exception_msg(bool_expr, ...) mylib_assert_exception_diecode_msg(bool_expr, , __VA_ARGS__)

#define mylib_assert_exception(bool_expr) mylib_assert_exception_msg(bool_expr, "")

// ---------------------------------------------------

} // end namespace Mylib

#endif