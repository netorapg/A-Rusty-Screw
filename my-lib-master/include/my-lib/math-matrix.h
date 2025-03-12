#ifndef __MY_LIB_MATH_MATRIX_HEADER_H__
#define __MY_LIB_MATH_MATRIX_HEADER_H__

#include <iostream>
#include <concepts>
#include <type_traits>
#include <ostream>
#include <utility>
#include <initializer_list>

#include <cmath>

#include <my-lib/std.h>
#include <my-lib/math-vector.h>

namespace Mylib
{
namespace Math
{

#ifdef MYLIB_MATH_BUILD_OPERATION
#error nooooooooooo
#endif

// ---------------------------------------------------

template <typename T, uint32_t nrows, uint32_t ncols>
class Matrix
{
private:
	T data[nrows*ncols];
public:
	using Type = T;

	constexpr static uint32_t get_nrows () noexcept
	{
		return nrows;
	}

	constexpr static uint32_t get_ncols () noexcept
	{
		return ncols;
	}

	constexpr static uint32_t get_length () noexcept
	{
		return nrows * ncols;
	}

	constexpr static T fp (const auto v) noexcept
	{
		return static_cast<T>(v);
	}

	inline T* get_raw () noexcept
	{
		return this->data;
	}

	inline const T* get_raw () const noexcept
	{
		return this->data;
	}

	// ------------------------ Constructors

	constexpr Matrix () noexcept = default;

	constexpr Matrix (const std::initializer_list<T> values) noexcept
	{
		for (uint32_t i = 0; const auto v : values)
			this->data[i++] = v;
	}

	// -------------------------------------

	constexpr T& operator[] (const uint32_t i) noexcept
	{
		return this->data[i];
	}

	constexpr const T operator[] (const uint32_t i) const noexcept
	{
		return this->data[i];
	}

	constexpr T& operator[] (const uint32_t row, const uint32_t col) noexcept
	{
		return this->data[row*ncols + col];
	}

	constexpr T operator[] (const uint32_t row, const uint32_t col) const noexcept
	{
		return this->data[row*ncols + col];
	}

	#undef MYLIB_MATH_BUILD_OPERATION
	#define MYLIB_MATH_BUILD_OPERATION(OP) \
		constexpr Matrix& operator OP (const Matrix& other) noexcept \
		{ \
			for (uint32_t i = 0; i < get_length(); i++) \
				this->data[i] OP other.data[i]; \
			return *this; \
		} \
		constexpr Matrix& operator OP (const Type s) noexcept \
		{ \
			for (uint32_t i = 0; i < get_length(); i++) \
				this->data[i] OP s; \
			return *this; \
		}
	
	MYLIB_MATH_BUILD_OPERATION( += )
	MYLIB_MATH_BUILD_OPERATION( -= )

	#undef MYLIB_MATH_BUILD_OPERATION
	#define MYLIB_MATH_BUILD_OPERATION(OP) \
		constexpr Matrix& operator OP (const Type s) noexcept \
		{ \
			for (uint32_t i = 0; i < get_length(); i++) \
				this->data[i] OP s; \
			return *this; \
		}
	
	MYLIB_MATH_BUILD_OPERATION( *= )
	MYLIB_MATH_BUILD_OPERATION( /= )

	constexpr Matrix& operator *= (const Matrix& b) noexcept
	{
		static_assert(nrows == ncols);
		*this = *this * b;
		return *this;
	}

	constexpr void set_zero () noexcept
	{
		for (uint32_t i = 0; i < get_length(); i++)
			this->data[i] = 0;
	}

	constexpr void set_identity () noexcept
	{
		static_assert(nrows == ncols);

		this->set_zero();
		
		auto& m = *this;

		for (uint32_t i = 0; i < nrows; i++)
			m[i, i] = 1;
	}

	template <uint32_t vector_dim>
	constexpr void set_scale (const Vector<T, vector_dim>& v) noexcept
	{
		static_assert(nrows == ncols);
		static_assert(vector_dim <= nrows);

		this->set_zero();

		auto& m = *this;
		
		for (uint32_t i = 0; i < vector_dim; i++)
			m[i, i] = v[i];
		
		for (uint32_t i = vector_dim; i < nrows; i++)
			m[i, i] = 1;
	}

	template <uint32_t vector_dim>
	constexpr void set_translate (const Vector<T, vector_dim>& v) noexcept
	{
		static_assert(nrows == ncols);
		static_assert(vector_dim < nrows);

		this->set_identity();
		
		constexpr uint32_t last = ncols - 1;

		auto& m = *this;

		for (uint32_t i = 0; i < vector_dim; i++)
			m[i, last] = v[i];
	}

	constexpr void set_rotation_matrix (const Vector<T, 3>& axis_, const T angle) noexcept
		requires (nrows == ncols && ncols == 3)
	{
		const T c = std::cos(angle);
		const T s = std::sin(angle);
		
		const T t = fp(1) - c;
		//const T sh = std::sin(angle / fp(2));
		//const T t = fp(2) * sh * sh;

		const Vector<T, 3> axis = normalize(axis_);

		// Rodrigues' rotation

	#if 1
		Matrix w;

		w[0, 0] = 0;
		w[0, 1] = -axis.z;
		w[0, 2] = axis.y;

		w[1, 0] = axis.z;
		w[1, 1] = 0;
		w[1, 2] = -axis.x;

		w[2, 0] = -axis.y;
		w[2, 1] = axis.x;
		w[2, 2] = 0;

		const Matrix w2 = w * w;

		this->set_identity();
		*this += w * s + w2 * t;
	#else
		const T x = axis.x;
		const T y = axis.y;
		const T z = axis.z;

		auto& m = *this;

		m(0, 0) = t*x*x + c;
		m(0, 1) = t*x*y - s*z;
		m(0, 2) = t*x*z + s*y;

		m(1, 0) = t*x*y + s*z;
		m(1, 1) = t*y*y + c;
		m(1, 2) = t*y*z - s*x;

		m(2, 0) = t*x*z - s*y;
		m(2, 1) = t*y*z + s*x;
		m(2, 2) = t*z*z + c;
	#endif
	}

	constexpr void set_rotation_matrix (const Vector<T, 3>& axis_, const T angle) noexcept
		requires (nrows == ncols && ncols == 4)
	{
		const Matrix<T, 3, 3> m = Matrix<T, 3, 3>::rotation(axis_, angle);
		auto& self = *this;

		for (uint32_t i = 0; i < 3; i++) {
			for (uint32_t j = 0; j < 3; j++)
				self[i, j] = m[i, j];
		}

		self[0, 3] = 0;
		self[1, 3] = 0;
		self[2, 3] = 0;

		self[3, 0] = 0;
		self[3, 1] = 0;
		self[3, 2] = 0;

		self[3, 3] = 1;
	}

	/*
		Perspective projection matrix

		About perspective projection matrix:
		https://gamedev.stackexchange.com/questions/120338/what-does-a-perspective-projection-matrix-look-like-in-opengl
		https://stackoverflow.com/questions/76304134/understanding-opengl-perspective-projection-matrix-setting-the-near-plane-below
		https://stackoverflow.com/questions/4124041/is-opengl-coordinate-system-left-handed-or-right-handed
		
		https://github.com/google/mathfu
			File include/mathfu/matrix.h
			Function PerspectiveHelper
		
		https://github.com/g-truc/glm
			File glm/ext/matrix_clip_space.inl
			Function perspectiveRH_NO
		
		My function is working fine for Opengl.
		Considers a right-handed world coordinate system.
	*/

	constexpr void set_perspective (const T fovy,
	                                const T screen_width,
									const T screen_height,
									const T znear,
									const T zfar
									) noexcept
	{
		static_assert(nrows == ncols && nrows == 4);

		const T aspect = screen_width / screen_height;
		const T zdist = znear - zfar;
		const T y = fp(1) / std::tan(fovy * fp(0.5));

		auto& m = *this;

		m[0, 0] = y / aspect;
		m[0, 1] = 0;
		m[0, 2] = 0;
		m[0, 3] = 0;

		m[1, 0] = 0;
		m[1, 1] = y;
		m[1, 2] = 0;
		m[1, 3] = 0;

		m[2, 0] = 0;
		m[2, 1] = 0;
		m[2, 2] = (zfar + znear) / zdist;
		m[2, 3] = (fp(2) * znear * zfar) / zdist;
		
		m[3, 0] = 0;
		m[3, 1] = 0;
		m[3, 2] = fp(-1);
		m[3, 3] = 0;
	}

	/*
		Orthogonal projection matrix

		https://github.com/google/mathfu
			File include/mathfu/matrix.h
			Function OrthoHelper
		
		This orthographic projection matrix is for a right-handed
		world coordinate system and opengl.
	*/

	constexpr void set_orthogonal  (const T view_width, // height will be calculated using the aspect ratio
	                                const T screen_width,
									const T screen_height,
									const T znear,
									const T zfar
									) noexcept
	{
		static_assert(nrows == ncols && nrows == 4);

		const T aspect = screen_width / screen_height;
		const T view_height = view_width / aspect;
		const T left = view_width / fp(-2);
		const T right = -left;
		const T bottom = view_height / fp(-2);
		const T top = -bottom;

		auto& self = *this;

		self[0, 0] = fp(2) / (right - left);
		self[0, 1] = 0;
		self[0, 2] = 0;
		self[0, 3] = 0; // -(right + left) / (right - left);

		self[1, 0] = 0;
		self[1, 1] = fp(2) / (top - bottom);
		self[1, 2] = 0;
		self[1, 3] = 0; // -(top + bottom) / (top - bottom);

		self[2, 0] = 0;
		self[2, 1] = 0;
		self[2, 2] = fp(-2) / (zfar - znear);
		self[2, 3] = (znear + zfar) / (znear - zfar);

		self[3, 0] = 0;
		self[3, 1] = 0;
		self[3, 2] = 0;
		self[3, 3] = 1;
	}

	/*
		About look-at matrix:
		
		https://github.com/google/mathfu
			File include/mathfu/matrix.h
			Function LookAtHelper
		
		https://github.com/g-truc/glm
			File glm/ext/matrix_transform.inl
			Function lookAtRH

		My code is working fine for Opengl.
		Considers a right-handed world coordinate system.
	*/

	constexpr void set_look_at (const Vector<T, 3>& eye,
	                            const Vector<T, 3>& at,
								const Vector<T, 3>& world_up
								) noexcept
	{
		static_assert(nrows == ncols && nrows == 4);

		const Vector<T, 3> direction = normalize(at - eye);
		const Vector<T, 3> right = normalize(cross_product(world_up, direction));
		const Vector<T, 3> up = cross_product(direction, right);

		auto& m = *this;

		m[0, 0] = -right.x;
		m[0, 1] = -right.y;
		m[0, 2] = -right.z;
		m[0, 3] = 0; //-dot_product(right, eye);

		m[1, 0] = up.x;
		m[1, 1] = up.y;
		m[1, 2] = up.z;
		m[1, 3] = 0; //-dot_product(up, eye);

		m[2, 0] = -direction.x;
		m[2, 1] = -direction.y;
		m[2, 2] = -direction.z;
		m[2, 3] = 0; //dot_product(direction, eye);

		m[3, 0] = 0;
		m[3, 1] = 0;
		m[3, 2] = 0;
		m[3, 3] = 1;

		// You can choose between:
		// Uncomment the dot products and comment the below translation.
		// Or leave as it is.
		// As as it is is less eficient, but it's easier to understand.

		m *= translate(-eye);
	}

	constexpr void transpose () noexcept
	{
		static_assert(nrows == ncols);

		auto& m = *this;
		for (uint32_t i = 0; i < nrows; i++) {
			for (uint32_t j = i + 1; j < ncols; j++)
				std::swap(m[i, j], m[j, i]);
		}
	}

	// ---------------------------------------------------

	static constexpr Matrix zero () noexcept
	{
		Matrix m;
		m.set_zero();
		return m;
	}

	static constexpr Matrix identity () noexcept
	{
		static_assert(nrows == ncols);
		Matrix m;
		m.set_identity();
		return m;
	}

	template <uint32_t vector_dim>
	static constexpr Matrix scale (const Vector<T, vector_dim>& v) noexcept
	{
		static_assert(nrows == ncols);
		static_assert(vector_dim <= nrows);
		Matrix m;
		m.set_scale(v);
		return m;
	}

	template <uint32_t vector_dim>
	static constexpr Matrix translate (const Vector<T, vector_dim>& v) noexcept
	{
		static_assert(nrows == ncols);
		static_assert(vector_dim < nrows);

		Matrix m;
		m.set_translate(v);
		return m;
	}

	static constexpr Matrix perspective (const T fovy,
								         const T screen_width,
								         const T screen_height,
								         const T znear,
								         const T zfar
								         ) noexcept
	{
		static_assert(nrows == ncols && nrows == 4);
		Matrix m;
		m.set_perspective(fovy, screen_width, screen_height, znear, zfar);
		return m;
	}

	static constexpr Matrix look_at (const Vector<T, 3>& eye,
							         const Vector<T, 3>& at,
							         const Vector<T, 3>& world_up
							         ) noexcept
	{
		static_assert(nrows == ncols && nrows == 4);
		Matrix m;
		m.set_look_at(eye, at, world_up);
		return m;
	}

	static constexpr Matrix rotation (const Vector<T, nrows>& axis, const T angle) noexcept
	{
		static_assert(nrows == ncols);
		Matrix m;
		m.set_rotation_matrix(axis, angle);
		return m;
	}
};

// ---------------------------------------------------


//template <typename T, uint32_t dim>
//Matrix<T, dim, dim> gen_rotation_matrix (const Vector<T, dim>& axis, const T angle) noexcept;

// ---------------------------------------------------

template <typename T, uint32_t dim>
Vector<T, dim> rotate (const Vector<T, dim>& axis, const T angle, const Vector<T, dim>& v) noexcept
	requires (dim >= 2 && dim <= 3)
{
	Vector<T, dim> rotated;
	rotated = Matrix<T, dim, dim>::rotation(axis, angle) * v;
	return rotated;
}

// ---------------------------------------------------

#undef MYLIB_MATH_BUILD_OPERATION
#define MYLIB_MATH_BUILD_OPERATION(OP) \
	template <typename T, uint32_t nrows, uint32_t ncols> \
	Matrix<T, nrows, ncols> operator OP (const Matrix<T, nrows, ncols>& ma, const Matrix<T, nrows, ncols>& mb) noexcept \
	{ \
		Matrix<T, nrows, ncols> r; \
		for (uint32_t i = 0; i < (nrows*ncols); i++) \
			r[i] = ma[i] OP mb[i]; \
		return r; \
	} \
	template <typename T, uint32_t nrows, uint32_t ncols> \
	Matrix<T, nrows, ncols> operator OP (const Matrix<T, nrows, ncols>& ma, const T v) noexcept \
	{ \
		Matrix<T, nrows, ncols> r; \
		for (uint32_t i = 0; i < (nrows*ncols); i++) \
			r[i] = ma[i] OP v; \
		return r; \
	}
	
MYLIB_MATH_BUILD_OPERATION( + )
MYLIB_MATH_BUILD_OPERATION( - )

// ---------------------------------------------------

#undef MYLIB_MATH_BUILD_OPERATION
#define MYLIB_MATH_BUILD_OPERATION(OP) \
	template <typename T, uint32_t nrows, uint32_t ncols> \
	Matrix<T, nrows, ncols> operator OP (const Matrix<T, nrows, ncols>& m, const T v) noexcept \
	{ \
		Matrix<T, nrows, ncols> r; \
		for (uint32_t i = 0; i < (nrows*ncols); i++) \
			r[i] = m[i] OP v; \
		return r; \
	}
	
MYLIB_MATH_BUILD_OPERATION( * )
MYLIB_MATH_BUILD_OPERATION( / )

// ---------------------------------------------------

template <typename T, uint32_t nrows_a, uint32_t ncols_a, uint32_t ncols_b>
constexpr Matrix<T, nrows_a, ncols_b> operator* (const Matrix<T, nrows_a, ncols_a>& a,
                                                 const Matrix<T, ncols_a, ncols_b>& b) noexcept
{
	Matrix<T, nrows_a, ncols_b> r;

	r.set_zero();

	for (uint32_t i = 0; i < nrows_a; i++) {
		for (uint32_t k = 0; k < ncols_a; k++) {
			const T v = a[i, k];

			for (uint32_t j = 0; j < ncols_b; j++)
				r[i, j] += v * b[k, j];
		}
	}
	
	return r;
}

// ---------------------------------------------------

template <typename T, uint32_t dim>
Vector<T, dim> operator* (const Matrix<T, dim, dim>& m, const Vector<T, dim>& v) noexcept
{
	Vector<T, dim> r;

	for (uint32_t i = 0; i < dim; i++) {
		r[i] = 0;
		for (uint32_t j = 0; j < dim; j++)
			r[i] += m[i, j] * v[j];
	}
	
	return r;
}

// ---------------------------------------------------

template <typename T, uint32_t nrows, uint32_t ncols>
constexpr Matrix<T, ncols, nrows> transpose (const Matrix<T, nrows, ncols>& m) noexcept
{
	Matrix<T, ncols, nrows> r;

	for (uint32_t i = 0; i < nrows; i++) {
		for (uint32_t j = 0; j < ncols; j++)
			r[j, i] = m[i, j];
	}
	
	return r;
}

// ---------------------------------------------------

template <typename T, uint32_t nrows, uint32_t ncols>
std::ostream& operator << (std::ostream& out, const Matrix<T, nrows, ncols>& m)
{
	for (uint32_t i = 0; i < nrows; i++) {
		for (uint32_t j = 0; j < ncols; j++)
			out << m[i, j] << ", ";
		out << std::endl;
	}

	return out;
}

// ---------------------------------------------------

#undef MYLIB_MATH_BUILD_OPERATION

} // end namespace Math
} // end namespace Mylib

#endif