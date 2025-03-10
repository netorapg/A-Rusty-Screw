#ifndef __MY_LIB_UTILS_HEADER_H__
#define __MY_LIB_UTILS_HEADER_H__

#include <unordered_map>
#include <string>
#include <string_view>

#include <my-lib/std.h>


namespace Mylib
{

// ---------------------------------------------------

/* Allow to use std::string_view as key in std::unordered_map
   using heterogeneous lookup.
*/

struct string_hash__ {
	using is_transparent = void;

	[[nodiscard]] std::size_t operator() (const char *txt) const {
		return std::hash<std::string_view>{}(txt);
	}
	
	[[nodiscard]] std::size_t operator() (const std::string_view txt) const {
		return std::hash<std::string_view>{}(txt);
	}
	
	[[nodiscard]] std::size_t operator() (const std::string& txt) const {
		return std::hash<std::string>{}(txt);
	}
};

template <typename T>
using unordered_map_string_key = std::unordered_map<std::string, T, string_hash__, std::equal_to<>>;

// ---------------------------------------------------

} // end namespace Mylib

#endif