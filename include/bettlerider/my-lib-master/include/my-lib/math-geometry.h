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
	Point<T, dim> point;
	Vector<T, dim> vector;
};

// ---------------------------------------------------

template <typename T>
class Plane
{
public:
	Point<T, 3> point;
	Vector<T, 3> normal;
};

// ---------------------------------------------------

template <typename T>
Point<T, 3> intersection (const Plane<T>& plane, const Line<T, 3>& line)
{
	const Vector<T, 3> v = plane.point - line.point;
	const T t = dot_product(v, plane.normal) / dot_product(line.vector, plane.normal);
	return line.point + (line.vector * t);
}

// ---------------------------------------------------

template <typename T>
Point<T, 3> intersection (const Line<T, 3>& line, const Plane<T>& plane)
{
	return intersection(plane, line);
}


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