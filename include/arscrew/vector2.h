// this code is part of a whole library that you can find at https://github.com/ehmcruz/my-lib

#ifndef __MY_LIB_MATH_VECTOR2_HEADER_H__
#define __MY_LIB_MATH_VECTOR2_HEADER_H__

#include <iostream>
#include <cmath>
#include <cstdint>
#include <ostream>
#include <algorithm>
#include <type_traits> // Necessário para remove_type_qualifiers
#include <concepts>    // Necessário para os concepts

namespace Mylib
{
namespace Math
{

// Forward declaration para a classe VectorStorage__
template <typename T, uint32_t dim>
class VectorStorage__;

// Definição do armazenamento para um vetor de 2 dimensões
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
    };

    constexpr VectorStorage__() noexcept = default;

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

// A classe Vector principal, com toda a lógica
template <typename T, uint32_t dim,
          typename TParent = VectorStorage__<T, dim>>
class Vector : public TParent
{
public:
    using Type = T;

    static_assert(sizeof(TParent) == (dim * sizeof(Type)));

    inline Type* get_raw () noexcept { return this->data; }
    inline const Type* get_raw () const noexcept { return this->data; }

    constexpr static uint32_t get_dim () noexcept { return dim; }

    // --- Construtores
    constexpr Vector () noexcept = default;
    using TParent::TParent;

    // --- Operadores de atribuição
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
    MYLIB_MATH_BUILD_OPERATION( *= )
    MYLIB_MATH_BUILD_OPERATION( /= )
    #undef MYLIB_MATH_BUILD_OPERATION

    // --- Acesso a elementos
    constexpr Type& operator[] (const uint32_t i) noexcept { return this->data[i]; }
    constexpr Type operator[] (const uint32_t i) const noexcept { return this->data[i]; }

    // --- Métodos
    constexpr Type length_squared () const noexcept
    {
        Type value = 0;
        for (uint32_t i = 0; i < dim; i++)
            value += this->data[i] * this->data[i];
        return value;
    }

    constexpr Type length () const noexcept
    {
        return std::sqrt(this->length_squared());
    }

    constexpr Type normalize () noexcept
    {
        const Type len = this->length();
        const Type inv_len = (len != 0) ? (static_cast<Type>(1) / len) : 0;
        for (uint32_t i = 0; i < dim; i++)
            this->data[i] *= inv_len;
        return len;
    }

    constexpr void set_zero () noexcept
    {
        for (uint32_t i = 0; i < dim; i++)
            this->data[i] = 0;
    }

    static constexpr Vector zero () noexcept
    {
        Vector v;
        v.set_zero();
        return v;
    }
};

// --- Alias para Point
template <typename T, uint32_t dim>
using Point = Vector<T, dim>;

// --- Operadores Aritméticos Globais
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
    } \
    template <typename T, uint32_t dim> \
    constexpr Vector<T, dim> operator OP (const T s, const Vector<T, dim>& a) noexcept \
    { \
        Vector<T, dim> r; \
        for (uint32_t i = 0; i < dim; i++) \
            r[i] = s OP a[i]; \
        return r; \
    }

MYLIB_MATH_BUILD_OPERATION( + )
MYLIB_MATH_BUILD_OPERATION( - )
MYLIB_MATH_BUILD_OPERATION( * )
MYLIB_MATH_BUILD_OPERATION( / )
#undef MYLIB_MATH_BUILD_OPERATION

// --- Operador Unário Negativo
template <typename T, uint32_t dim>
constexpr Vector<T, dim> operator- (const Vector<T, dim>& v) noexcept
{
    Vector<T, dim> r;
    for (uint32_t i = 0; i < dim; i++)
        r.data[i] = -v.data[i];
    return r;
}

// --- Operadores de Comparação
template <typename T, uint32_t dim>
constexpr bool operator== (const Vector<T, dim>& lhs, const Vector<T, dim>& rhs)
{
    for (uint32_t i = 0; i < dim; i++)
        if (lhs[i] != rhs[i])
            return false;
    return true;
}

template <typename T, uint32_t dim>
constexpr bool operator!= (const Vector<T, dim>& lhs, const Vector<T, dim>& rhs)
{
    return !(lhs == rhs);
}

// --- Funções Auxiliares Globais
template <typename T, uint32_t dim>
constexpr T dot_product (const Vector<T, dim>& a, const Vector<T, dim>& b) noexcept
{
    T value = 0;
    for (uint32_t i = 0; i < dim; i++)
        value += a[i] * b[i];
    return value;
}

template <typename T, uint32_t dim>
constexpr Vector<T, dim> normalize (const Vector<T, dim>& v) noexcept
{
    const T len = v.length();
    return v / len;
}

template <typename T, uint32_t dim>
constexpr T distance (const Point<T, dim>& a, const Point<T, dim>& b) noexcept
{
    return (a - b).length();
}

// --- Stream operator
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

// --- Alias e Concepts para 2D
using Vector2f = Vector<float, 2>;
using Point2f = Vector2f;

static_assert(sizeof(Vector2f) == (2 * sizeof(float)));

// Helper para concepts (caso não esteja no seu "std.h")
template< class T >
struct remove_type_qualifiers {
    using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template <typename T>
concept is_Vector2f = std::same_as< typename remove_type_qualifiers<T>::type, Vector2f >;

template <typename T>
concept is_Vector = is_Vector2f<T>;

template <typename T>
concept is_Point = is_Vector<T>;

} // end namespace Math
} // end namespace Mylib

#endif // __MY_LIB_MATH_VECTOR2_HEADER_H__