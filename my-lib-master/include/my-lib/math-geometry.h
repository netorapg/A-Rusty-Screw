#ifndef __MY_LIB_MATH_GEOMETRY_HEADER_H__
#define __MY_LIB_MATH_GEOMETRY_HEADER_H__

#include <iostream>
#include <concepts>
#include <type_traits>
#include <ostream>

#include <cmath>

#include <my-lib/std.h>
#include <my-lib/math-vector.h>

namespace Mylib
{
namespace Math
{

// ---------------------------------------------------

template <typename T, uint32_t dim>
class Line
{
public:
	Point<T, dim> base_point;
	Vector<T, dim> direction;
};

// ---------------------------------------------------

using Line2f = Line<float, 2>;
using Line3f = Line<float, 3>;

// ---------------------------------------------------

template <typename T, uint32_t dim>
std::ostream& operator << (std::ostream& out, const Line<T, dim>& line)
{
	out << "base=" << line.base << " direction " << line.direction;

	return out;
}

// ---------------------------------------------------

} // end namespace Math
} // end namespace Mylib

#endif