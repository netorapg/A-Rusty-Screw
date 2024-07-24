#ifndef __MY_LIB_MACROS_HEADER_H__
#define __MY_LIB_MACROS_HEADER_H__

#include <type_traits>
#include <utility>

// ---------------------------------------------------

#define OO_ENCAPSULATE_SCALAR_READONLY(TYPE, VAR) \
	protected: \
		TYPE VAR; \
	public: \
		constexpr TYPE get_##VAR () const noexcept { \
			return this->VAR; \
		} \
	protected:

#define OO_ENCAPSULATE_SCALAR(TYPE, VAR) \
	OO_ENCAPSULATE_SCALAR_READONLY(TYPE, VAR) \
	public: \
		constexpr void set_##VAR (const TYPE VAR) noexcept { \
			this->VAR = VAR; \
		} \
	protected:

// ---------------------------------------------------

#define OO_ENCAPSULATE_SCALAR_REACT(TYPE, VAR, REACT) \
	OO_ENCAPSULATE_SCALAR_READONLY(TYPE, VAR) \
	public: \
		inline void set_##VAR (const TYPE VAR) noexcept { \
			this->VAR = VAR; \
			REACT; \
		} \
	protected:

// ---------------------------------------------------

#define OO_ENCAPSULATE_SCALAR_CONST_READONLY(TYPE, VAR) \
	protected: \
		const TYPE VAR; \
	public: \
		constexpr TYPE get_##VAR () const noexcept { \
			return this->VAR; \
		} \
	protected:

// ---------------------------------------------------

#define OO_ENCAPSULATE_SCALAR_INIT_READONLY(TYPE, VAR, DATA) \
	protected: \
		TYPE VAR = (DATA); \
	public: \
		constexpr TYPE get_##VAR () const noexcept { \
			return this->VAR; \
		} \
	protected:

#define OO_ENCAPSULATE_SCALAR_INIT(TYPE, VAR, DATA) \
	OO_ENCAPSULATE_SCALAR_INIT_READONLY(TYPE, VAR, DATA) \
	public: \
		constexpr void set_##VAR (const TYPE VAR) noexcept { \
			this->VAR = VAR; \
		} \
	protected:

// ---------------------------------------------------

#define OO_ENCAPSULATE_SCALAR_CONST_INIT_READONLY(TYPE, VAR, DATA) \
	protected: \
		const TYPE VAR = (DATA); \
	public: \
		constexpr TYPE get_##VAR () const noexcept { \
			return this->VAR; \
		} \
	protected:

// ---------------------------------------------------

#define OO_ENCAPSULATE_PTR_READONLY(TYPE, VAR) \
	protected: \
		TYPE VAR; \
	public: \
		constexpr TYPE get_##VAR () noexcept { \
			return this->VAR; \
		} \
	protected:

#define OO_ENCAPSULATE_PTR(TYPE, VAR) \
	OO_ENCAPSULATE_PTR_READONLY(TYPE, VAR) \
	public: \
		constexpr void set_##VAR (TYPE VAR) noexcept { \
			this->VAR = VAR; \
		} \
	protected:

// ---------------------------------------------------

#define OO_ENCAPSULATE_PTR_INIT_READONLY(TYPE, VAR, DATA) \
	protected: \
		TYPE VAR = DATA; \
	public: \
		constexpr TYPE get_##VAR () noexcept { \
			return this->VAR; \
		} \
	protected:

#define OO_ENCAPSULATE_PTR_INIT(TYPE, VAR, DATA) \
	OO_ENCAPSULATE_PTR_INIT_READONLY(TYPE, VAR, DATA) \
	public: \
		constexpr void set_##VAR (TYPE VAR) noexcept { \
			this->VAR = VAR; \
		} \
	protected:

// ---------------------------------------------------

#define OO_ENCAPSULATE_OBJ_READONLY_FUNCTIONS(TYPE, VAR) \
	public: \
		constexpr const TYPE& get_ref_##VAR () const noexcept { \
			return this->VAR; \
		} \
		inline TYPE get_value_##VAR () const { \
			return this->VAR; \
		} \
	protected:

#define OO_ENCAPSULATE_OBJ_FUNCTIONS(TYPE, VAR) \
	public: \
		constexpr TYPE& get_ref_##VAR () noexcept { \
			return this->VAR; \
		} \
		inline void set_##VAR (const TYPE& VAR) { \
			this->VAR = VAR; \
		} \
		inline void set_##VAR (TYPE&& VAR) { \
			this->VAR = std::move(VAR); \
		} \
	protected:

#define OO_ENCAPSULATE_OBJ_READONLY(TYPE, VAR) \
	protected: \
		TYPE VAR; \
	OO_ENCAPSULATE_OBJ_READONLY_FUNCTIONS(TYPE, VAR)

#define OO_ENCAPSULATE_OBJ(TYPE, VAR) \
	OO_ENCAPSULATE_OBJ_READONLY(TYPE, VAR) \
	OO_ENCAPSULATE_OBJ_FUNCTIONS(TYPE, VAR)

#define OO_ENCAPSULATE_OBJ_INIT_READONLY(TYPE, VAR, DATA) \
	protected: \
		TYPE VAR = (DATA); \
	OO_ENCAPSULATE_OBJ_READONLY_FUNCTIONS(TYPE, VAR)

#define OO_ENCAPSULATE_OBJ_INIT(TYPE, VAR, DATA) \
	OO_ENCAPSULATE_OBJ_INIT_READONLY(TYPE, VAR, DATA) \
	OO_ENCAPSULATE_OBJ_FUNCTIONS(TYPE, VAR)

// ---------------------------------------------------

#define MYLIB_DELETE_COPY_MOVE_CONSTRUCTOR(CLASS) \
	CLASS (const CLASS&) = delete; \
	CLASS (CLASS&&) = delete;

#define MYLIB_DELETE_COPY_MOVE_ASSIGN(CLASS) \
	CLASS& operator= (const CLASS&) = delete; \
	CLASS& operator= (CLASS&&) = delete;

#define MYLIB_DELETE_COPY_MOVE_CONSTRUCTOR_ASSIGN(CLASS) \
	MYLIB_DELETE_COPY_MOVE_CONSTRUCTOR(CLASS) \
	MYLIB_DELETE_COPY_MOVE_ASSIGN(CLASS)

// ---------------------------------------------------

#if defined(__GNUC__) || defined(__clang__)
	#define MYLIB_ALIGN_STRUCT(V)   __attribute__((aligned(V)))
#elif defined(_MSC_VER)
	#define MYLIB_ALIGN_STRUCT(V)   __declspec(align(V))
#else
	#error "Unknown compiler. Can't define MYLIB_ALIGN_STRUCT"
#endif

#endif