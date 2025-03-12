#ifndef __MY_LIB_BIT_HEADER_H__
#define __MY_LIB_BIT_HEADER_H__

#include <concepts>
#include <type_traits>
#include <ostream>

#include <cstdint>

#include <my-lib/std.h>

namespace Mylib
{

#ifdef MYLIB_BUILD_OPERATION
#error nooooooooooo
#endif

// ---------------------------------------------------

struct BitField
{
	uint16_t bpos;     // bit start position
	uint16_t blength;  // bit length
};

// ---------------------------------------------------

template <typename T>
constexpr T extract_bits (const T v, const std::size_t bpos, const std::size_t blength) noexcept
{
	const T mask = (1 << blength) - 1;

	return (v >> bpos) & mask;
}

template <typename T>
constexpr T extract_bits (const T v, const BitField field) noexcept
{
	return extract_bits(v, field.bpos, field.blength);
}

template <typename T>
constexpr T extract_bits (const T v, const auto bpos, const auto blength) noexcept
	requires std::is_enum_v<decltype(bpos)> && std::is_enum_v<decltype(blength)>
{
	return extract_bits(v, std::to_underlying(bpos), std::to_underlying(blength));
}

// ---------------------------------------------------

template <typename T>
constexpr T set_bits (const T src, const std::size_t bpos, const std::size_t blength, const std::integral auto value) noexcept
{
	const T mask = (1 << blength) - 1;
	const T shifted_mask = mask << bpos;
	const T safe_value = static_cast<T>(value) & mask;

	return (src & ~shifted_mask) | (safe_value << bpos);
}

template <typename T>
constexpr T set_bits (const T src, const std::size_t bpos, const std::size_t blength, const auto value) noexcept
	requires (std::is_integral_v<decltype(value)> == false)
{
	const T value_ = value; // call conversion operator
	return set_bits(src, bpos, blength, value_);
}

template <typename T>
constexpr T set_bits (const T src, const BitField field, const auto value) noexcept
{
	return set_bits(src, field.bpos, field.blength, value);
}

template <typename T>
constexpr T set_bits (const T src, const auto bpos, const auto blength, const auto value) noexcept
	requires std::is_enum_v<decltype(bpos)> && std::is_enum_v<decltype(blength)>
{
	return set_bits(src, std::to_underlying(bpos), std::to_underlying(blength), value);
}

// ---------------------------------------------------

template <std::size_t storage_nbits>
class BitSetStorage__;

template <>
class BitSetStorage__<8>
{
public:
	using Type = uint8_t;
protected:
	Type storage__;
};

template <>
class BitSetStorage__<16>
{
public:
	using Type = uint16_t;
protected:
	Type storage__;
};

template <>
class BitSetStorage__<32>
{
public:
	using Type = uint32_t;
protected:
	Type storage__;
};

template <>
class BitSetStorage__<64>
{
public:
	using Type = uint64_t;
protected:
	Type storage__;
};

// ---------------------------------------------------

template <typename ParentType>
class BitSetStorage_ : public ParentType
{
protected:
	using Type = typename ParentType::Type;

	Type& storage () noexcept
	{
		return this->storage__;
	}

	const Type storage () const noexcept
	{
		return this->storage__;
	}
};

// ---------------------------------------------------

constexpr std::size_t calc_bit_set_storage_nbits__ (const std::size_t nbits) noexcept
{
	if (nbits <= 8)
		return 8;
	else if (nbits <= 16)
		return 16;
	else if (nbits <= 32)
		return 32;
	else if (nbits <= 64)
		return 64;
	else
		return 0;
}

// ---------------------------------------------------

/*
	BitSet__ is based on std::bitset.
	Differences:
		- Only works for up tp 64 bits, but it is faster.
		- Allows extraction and set of range of bits.
*/

template <typename ParentType, std::size_t nbits>
class BitSet__ : public ParentType
{
public:
	using Type = typename ParentType::Type;

	// --------------------------

	class reference
	{
	private:
		BitSet__& bitset;
		const uint32_t pos;
		const uint32_t length;

	public:
		reference () noexcept = delete;
		
		reference (BitSet__& bitset_, const std::size_t pos_, const std::size_t length_) noexcept
			: bitset(bitset_), pos(pos_), length(length_)
		{
		}

		reference (const reference& other) noexcept
			: bitset(other.bitset), pos(other.pos), length(other.length)
		{
		}

		reference& operator= (const Type value) noexcept
		{
			this->bitset.set(this->pos, this->length, value);
			return *this;
		}

		reference& operator= (const reference& other)
		{
			//mylib_assert_exception_msg(this->length == other.length, "The length of both references must be the same. Given ", this->length, " and ", other.length, ".");
			const Type value = other.bitset.extract_underlying(other.pos, other.length);
			this->bitset.set(this->pos, this->length, value);
			return *this;
		}

		template <typename Tother, std::size_t nbits_other>
		reference& operator= (const BitSet__<Tother, nbits_other>& other)
		{
			//mylib_assert_exception_msg(this->length == other.size(), "The length of both references must be the same. Given ", this->length, " and ", other.size(), ".");
			this->bitset.set(this->pos, this->length, other.to_underlying());
			return *this;
		}

		Type operator~ () const noexcept
		{
			const Type mask = (1 << this->length) - 1;
			return (~this->bitset.extract_underlying(this->pos, this->length)) & mask;
		}

		operator Type() const noexcept
		{
			return this->bitset.extract_underlying(this->pos, this->length);
		}
	};

	// --------------------------

	constexpr std::size_t get_storage_nbits () const noexcept
	{
		return sizeof(Type) * 8;
	}

	constexpr std::size_t size () const noexcept
	{
		return nbits;
	}

	// --------------------------

	constexpr BitSet__ () noexcept
	{
		this->storage() = 0;
	}

	constexpr BitSet__ (const BitSet__& other) noexcept
	{
		this->storage() = other.storage();
	}

	constexpr BitSet__ (const Type v) noexcept
	{
		this->storage() = v;
	}

	// --------------------------

	constexpr BitSet__& operator= (const BitSet__& other) noexcept
	{
		this->storage() = other.storage();
		return *this;
	}

	constexpr BitSet__& operator= (const Type v) noexcept
	{
		this->storage() = v;
		return *this;
	}

	// --------------------------

	constexpr Type to_underlying () const noexcept
	{
		return this->storage();
	}

	// --------------------------

	constexpr bool operator[] (const std::size_t pos) const noexcept
	{
		return (this->storage() >> pos) & 0x01;
	}

	template <typename Tenum>
	requires std::is_enum_v<Tenum>
	constexpr bool operator[] (const Tenum pos) const noexcept
	{
		return (*this)[std::to_underlying(pos)];
	}

	constexpr reference operator[] (const std::size_t pos) noexcept
	{
		return reference(*this, pos, 1);
	}

	template <typename Tenum>
	requires std::is_enum_v<Tenum>
	constexpr reference operator[] (const Tenum pos) noexcept
	{
		return (*this)[std::to_underlying(pos)];
	}

	// --------------------------

	constexpr Type operator[] (const BitField field) const noexcept
	{
		return extract_bits(this->storage(), field.bpos, field.blength);
	}

	constexpr reference operator[] (const BitField field) noexcept
	{
		return reference(*this, field.bpos, field.blength);
	}

	// --------------------------

	constexpr Type operator[] (const std::size_t pos, const std::size_t length) const noexcept
	{
		return extract_bits(this->storage(), pos, length);
	}

	constexpr reference operator[] (const std::size_t pos, const std::size_t length) noexcept
	{
		return reference(*this, pos, length);
	}

	// --------------------------

	constexpr BitSet__ operator() (const std::size_t pos, const std::size_t length) const noexcept
	{
		return BitSet__(extract_bits(this->storage(), pos, length));
	}

	template <typename TenumA, typename TenumB>
	requires std::is_enum_v<TenumA> && std::is_enum_v<TenumB>
	constexpr BitSet__ operator() (const TenumA pos, const TenumB length) const noexcept
	{
		return (*this)(std::to_underlying(pos), std::to_underlying(length));
	}

	// --------------------------

	constexpr BitSet__ operator() (const BitField field) const noexcept
	{
		return (*this)(field.bpos, field.blength);
	}

	// --------------------------

	constexpr BitSet__ extract (const std::size_t pos, const std::size_t length) const noexcept
	{
		return BitSet__( extract_bits(this->storage(), pos, length) );
	}

	constexpr Type extract_underlying (const std::size_t pos, const std::size_t length) const noexcept
	{
		return extract_bits(this->storage(), pos, length);
	}

	// --------------------------

	constexpr void set (const std::size_t pos, const std::size_t length, const auto v) noexcept
	{
		this->storage() = set_bits(this->storage(), pos, length, v);
	}

	// --------------------------

#ifdef MYLIB_BUILD_OPERATION
	#undef MYLIB_BUILD_OPERATION
#endif

	#define MYLIB_BUILD_OPERATION(OP) \
		constexpr BitSet__& operator OP (const BitSet__& other) noexcept \
		{ \
			this->storage() OP other.storage(); \
			return *this; \
		} \
		constexpr BitSet__& operator OP (const Type v) noexcept \
		{ \
			this->storage() OP v; \
			return *this; \
		}
	
	MYLIB_BUILD_OPERATION( &= )
	MYLIB_BUILD_OPERATION( |= )
	MYLIB_BUILD_OPERATION( ^= )

	#undef MYLIB_BUILD_OPERATION

	// --------------------------

	constexpr BitSet__ operator~ () const noexcept
	{
		return BitSet__(~this->storage());
	}
};

// ---------------------------------------------------

template <std::size_t nbits>
using BitSet = BitSet__<BitSetStorage_< BitSetStorage__<calc_bit_set_storage_nbits__(nbits)> >, nbits>;

// ---------------------------------------------------

template <typename ParentType>
using BitSetT = BitSet__<ParentType, sizeof(ParentType) * 8>;

// ---------------------------------------------------

template <typename T>
class BitSetWrapper__ : public T
{
public:
	using Type = typename T::Type;

protected:
	Type& storage () noexcept
	{
		return *reinterpret_cast<Type*>(this);
	}

	const Type& storage () const noexcept
	{
		return *reinterpret_cast<const Type*>(this);
	}
};

// ---------------------------------------------------

template <typename T>
using BitSetWrapper = BitSetT< BitSetWrapper__<T> >;

// ---------------------------------------------------

inline std::ostream& operator << (std::ostream& out, const BitField field)
{
	out << '{' << field.bpos << ',' << field.blength << '}';
	return out;
}

template <typename ParentType, std::size_t nbits>
std::ostream& operator << (std::ostream& out, const BitSet__<ParentType, nbits>& bitset)
{
	for (int32_t i = bitset.size() - 1; i >= 0; i--)
		out << bitset[i];
	return out;
}

// ---------------------------------------------------

#ifdef MYLIB_BUILD_OPERATION
	#undef MYLIB_BUILD_OPERATION
#endif

} // end namespace Mylib

#endif