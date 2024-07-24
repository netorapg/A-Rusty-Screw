#ifndef __MY_LIB_MATH_VECTOR_HEADER_H__
#define __MY_LIB_MATH_VECTOR_HEADER_H__

#include <iostream>
#include <concepts>
#include <type_traits>
#include <ostream>

#include <cmath>

#include "/home/netorapg/projects/platfom2d/my-lib-master/include/my-lib/std.h"

namespace Mylib
{
namespace Math
{

#ifdef MYLIB_MATH_BUILD_OPERATION
#error nooooooooooo
#endif

// ---------------------------------------------------

template <typename T, uint32_t dim>
class VectorStorage__;

// ---------------------------------------------------

template <typename T>
class VectorStorage__<T, 2>
{
public:
	using Type = T;

	union {
		Type data[2];

		struct {
			Type x;
			Type y;
		};

		//uint64_t ivalue;
	};

	constexpr VectorStorage__ () noexcept = default;

	constexpr VectorStorage__ (const Type x_, const Type y_) noexcept
		: x(x_), y(y_)
	{
	}

	constexpr void set (const Type x, const Type y) noexcept
	{
		this->x = x;
		this->y = y;
	}
};

// ---------------------------------------------------

template <typename T>
class VectorStorage__<T, 3>
{
public:
	using Type = T;

	union {
		Type data[3];

		struct {
			Type x;
			Type y;
			Type z;
		};

		//uint64_t ivalue;
	};

	constexpr VectorStorage__ () noexcept = default;

	constexpr VectorStorage__ (const Type x_, const Type y_, const Type z_) noexcept
		: x(x_), y(y_), z(z_)
	{
	}

	constexpr void set (const Type x, const Type y, const Type z) noexcept
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

// ---------------------------------------------------

template <typename T>
class VectorStorage__<T, 4>
{
public:
	using Type = T;

	union {
		Type data[4];

		struct {
			Type x;
			Type y;
			Type z;
			Type w;
		};

		//uint64_t ivalue;
	};

	constexpr VectorStorage__ () noexcept = default;

	constexpr VectorStorage__ (const Type x_, const Type y_, const Type z_, const Type w_) noexcept
		: x(x_), y(y_), z(z_), w(w_)
	{
	}

	constexpr void set (const Type x, const Type y, const Type z, const Type w) noexcept
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
};

// ---------------------------------------------------

template <typename T, uint32_t dim,
          typename TParent = VectorStorage__<T, dim>>
class Vector : public TParent
{
public:
	using Type = T;

	static_assert(sizeof(TParent) == (dim * sizeof(Type)));

	inline Type* get_raw () noexcept
	{
		return this->data;
	}

	inline const Type* get_raw () const noexcept
	{
		return this->data;
	}

	consteval static uint32_t get_dim ()
	{
		return dim;
	}

	// ------------------------ Constructors

	Vector () noexcept = default;

	using TParent::TParent;

	template <uint32_t dim_other>
	constexpr Vector (const Vector<T, dim_other>& other) noexcept
	{
		static_assert(dim_other <= dim);
		for (uint32_t i = 0; i < dim_other; i++)
			this->data[i] = other.data[i];
		for (uint32_t i = dim_other; i < dim; i++)
			this->data[i] = 0;
	}

	// ------------------------ operator=

	// use default ones, so no need to define them

	// ------------------------ Other stuff

	#undef MYLIB_MATH_BUILD_OPERATION
	#define MYLIB_MATH_BUILD_OPERATION(OP) \
		constexpr Vector& operator OP (const Vector& other) noexcept \
		{ \
			for (uint32_t i = 0; i < dim; i++) \
				this->data[i] OP other.data[i]; \
			return *this; \
		} \
		constexpr Vector& operator OP (const Type s) noexcept \
		{ \
			for (uint32_t i = 0; i < dim; i++) \
				this->data[i] OP s; \
			return *this; \
		}
	
	MYLIB_MATH_BUILD_OPERATION( += )
	MYLIB_MATH_BUILD_OPERATION( -= )

	#undef MYLIB_MATH_BUILD_OPERATION
	#define MYLIB_MATH_BUILD_OPERATION(OP) \
		inline Vector& operator OP (const Type s) noexcept \
		{ \
			for (uint32_t i = 0; i < dim; i++) \
				this->data[i] OP s; \
			return *this; \
		}
	
	MYLIB_MATH_BUILD_OPERATION( *= )
	MYLIB_MATH_BUILD_OPERATION( /= )

	constexpr Type& operator[] (const uint32_t i) noexcept
	{
		return this->data[i];
	}

	constexpr Type operator[] (const uint32_t i) const noexcept
	{
		return this->data[i];
	}

	constexpr Type length () const noexcept
	{
		Type value = 0;
		for (uint32_t i = 0; i < dim; i++)
			value += this->data[i] * this->data[i];
		return std::sqrt(value);
	}

	constexpr void set_length (const Type len) noexcept
	{
		const Type ratio = len / this->length();
		for (uint32_t i = 0; i < dim; i++)
			this->data[i] *= ratio;
	}

	constexpr void normalize () noexcept
	{
		const Type len = this->length();
		for (uint32_t i = 0; i < dim; i++)
			this->data[i] /= len;
	}

	constexpr void cross_product (const Vector& a, const Vector& b) noexcept
	{
		static_assert(dim == 3);
		this->x = a.y * b.z - a.z * b.y;
		this->y = a.z * b.x - a.x * b.z;
		this->z = a.x * b.y - a.y * b.x;
	}

	// requires math-matrix.h
	constexpr void rotate_around_axis (const Vector<T, dim>& axis, const T angle) noexcept
		requires (dim == 3)
	{
		*this = gen_rotation_matrix<T, dim>(axis, angle) * (*this);
	}

	constexpr void set_zero () noexcept
	{
		for (uint32_t i = 0; i < dim; i++)
			this->data[i] = 0;
	}

	static consteval Vector zero () noexcept
	{
		Vector v;
		v.set_zero();
		return v;
	}

	static constexpr Vector up () noexcept
		requires (dim == 3)
	{
		return Vector(0, 1, 0);
	}

	static constexpr Vector down () noexcept
		requires (dim == 3)
	{
		return Vector(0, -1, 0);
	}

	static constexpr Vector left () noexcept
		requires (dim == 3)
	{
		return Vector(-1, 0, 0);
	}

	static constexpr Vector right () noexcept
		requires (dim == 3)
	{
		return Vector(1, 0, 0);
	}
};

// ---------------------------------------------------

template <typename T, uint32_t dim>
using Point = Vector<T, dim>;

// ---------------------------------------------------

#undef MYLIB_MATH_BUILD_OPERATION
#define MYLIB_MATH_BUILD_OPERATION(OP) \
	template <typename T, uint32_t dim> \
	constexpr Vector<T, dim> operator OP (const Vector<T, dim>& a, const Vector<T, dim>& b) noexcept \
	{ \
		Vector<T, dim> r; \
		for (uint32_t i = 0; i < dim; i++) \
			r.data[i] = a.data[i] OP b.data[i]; \
		return r; \
	} \
	template <typename T, uint32_t dim> \
	constexpr Vector<T, dim> operator OP (const Vector<T, dim>& a, const T s) noexcept \
	{ \
		Vector<T, dim> r; \
		for (uint32_t i = 0; i < dim; i++) \
			r.data[i] = a.data[i] OP s; \
		return r; \
	}

MYLIB_MATH_BUILD_OPERATION( + )
MYLIB_MATH_BUILD_OPERATION( - )

#undef MYLIB_MATH_BUILD_OPERATION
#define MYLIB_MATH_BUILD_OPERATION(OP) \
	template <typename T, uint32_t dim> \
	constexpr Vector<T, dim> operator OP (const Vector<T, dim>& a, const T s) noexcept \
	{ \
		Vector<T, dim> r; \
		for (uint32_t i = 0; i < dim; i++) \
			r.data[i] = a.data[i] OP s; \
		return r; \
	}

MYLIB_MATH_BUILD_OPERATION( * )
MYLIB_MATH_BUILD_OPERATION( / )

template <typename T, uint32_t dim> \
constexpr Vector<T, dim> operator- (const Vector<T, dim>& v) noexcept
{
	Vector<T, dim> r;
	for (uint32_t i = 0; i < dim; i++) \
		r.data[i] = -v.data[i];
	return r;
}

// ---------------------------------------------------

template <typename T, uint32_t dim>
constexpr T dot_product (const Vector<T, dim>& a, const Vector<T, dim>& b) noexcept
{
	T value = 0;
	for (uint32_t i = 0; i < dim; i++)
		value += a[i] * b[i];
	return value;
}

// ---------------------------------------------------

template <typename T>
constexpr Vector<T, 3> cross_product (const Vector<T, 3>& a, const Vector<T, 3>& b) noexcept
{
	Vector<T, 3> v;
	v.cross_product(a, b);
	return v;
}

// ---------------------------------------------------

template <typename T, uint32_t dim>
constexpr Vector<T, dim> normalize (const Vector<T, dim>& v) noexcept
{
	return v / v.length();
}

// ---------------------------------------------------

template <typename T, uint32_t dim>
constexpr Vector<T, dim> with_length (const Vector<T, dim>& v, const T len) noexcept
{
	return v * (len / v.length());
}

// ---------------------------------------------------

template <typename T, uint32_t dim>
constexpr T distance (const Point<T, dim>& a, const Point<T, dim>& b) noexcept
{
	//static_assert(remove_type_qualifiers<Ta>::type::get_dim() == remove_type_qualifiers<Tb>::type::get_dim());
	return (a - b).length();
}

// ---------------------------------------------------

using Vector2f = Vector<float, 2>;
using Point2f = Vector2f;

static_assert(sizeof(Vector2f) == (2 * sizeof(float)));

template <typename T>
concept is_Vector2f = std::same_as< typename remove_type_qualifiers<T>::type, Vector2f >;
//concept is_Vector2d = std::same_as<T, Vector2d> || std::same_as<T, Vector2d&> || std::same_as<T, Vector2d&&>;
//concept is_Vector2d = std::same_as< Vector2d, typename std::remove_cv< typename std::remove_reference<T>::type >::type >;
//concept is_Vector2d = std::same_as< typename std::remove_reference<T>::type, Vector2d >;

// ---------------------------------------------------

using Vector3f = Vector<float, 3>;
using Point3f = Vector3f;

static_assert(sizeof(Vector3f) == (3 * sizeof(float)));

template <typename T>
concept is_Vector3f = std::same_as< typename remove_type_qualifiers<T>::type, Vector3f >;
//concept is_Vector2d = std::same_as<T, Vector2d> || std::same_as<T, Vector2d&> || std::same_as<T, Vector2d&&>;
//concept is_Vector2d = std::same_as< Vector2d, typename std::remove_cv< typename std::remove_reference<T>::type >::type >;
//concept is_Vector2d = std::same_as< typename std::remove_reference<T>::type, Vector2d >;

// ---------------------------------------------------

using Vector4f = Vector<float, 4>;
using Point4f = Vector4f;

static_assert(sizeof(Vector4f) == (4 * sizeof(float)));

template <typename T>
concept is_Vector4f = std::same_as< typename remove_type_qualifiers<T>::type, Vector4f >;

// ---------------------------------------------------

template <typename T>
concept is_Vector = is_Vector2f<T> || is_Vector3f<T> || is_Vector4f<T>;

template <typename T>
concept is_Point = is_Vector<T>;

// ---------------------------------------------------

template <typename T, uint32_t dim>
std::ostream& operator << (std::ostream& out, const Vector<T, dim>& v)
{
	out << "[";
	
	for (uint32_t i = 0; i < dim; i++) {
		out << v[i];
		
		if (i < (dim-1))
			out << ", ";
	}

	out << "]";

	return out;
}

// ---------------------------------------------------

#undef MYLIB_MATH_BUILD_OPERATION

} // end namespace Math
} // end namespace Mylib

#endif