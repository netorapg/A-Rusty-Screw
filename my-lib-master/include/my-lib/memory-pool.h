#ifndef __MY_LIB_MEMORY_POOL_HEADER_H__
#define __MY_LIB_MEMORY_POOL_HEADER_H__

#include <iostream>
#include <initializer_list>
#include <vector>
//#include <mutex>

#include <cstdint>
#include <cstdlib>
#include <cassert>

#include <my-lib/macros.h>
#include <my-lib/std.h>
#include <my-lib/memory.h>

namespace Mylib
{
namespace Memory
{

// ---------------------------------------------------

/*
	Sources where I based:

	https://github.com/erincatto/box2d
	https://github.com/mtrebi/memory-allocators
	Boost Pool allocator
*/

// ---------------------------------------------------

inline constexpr size_t default_block_size = 16 * 1024; // 16KB

// ---------------------------------------------------

class PoolCore
{
private:
	struct Chunk {
		// We can store two things in the chunks.
		// When the chunk is free, it stores the free_chunks linked link.
		// Otherwise, it stores the actual user data.
		Chunk *next_chunk;
	};

	struct Block {
		Chunk *chunks;
		
		// When we use all the allocted memory, we allocate another block.
		Block *next_block;
	};

	const size_t type_size;
	const uint32_t chunks_per_block;
	const uint32_t align;

	OO_ENCAPSULATE_SCALAR_CONST_READONLY(size_t, chunk_size)

	Block *blocks = nullptr;
	Chunk *free_chunks = nullptr;

	// The mutex had a huge impact on performance.
	// Let's leave it off while I try to find a better solution.
	// One possible solution is to have an allocator per thread.
	// Problem: if one thread deallocates memory of another thread.
	//std::mutex mutex;

public:
	PoolCore (const size_t type_size_, const uint32_t chunks_per_block_, const uint32_t align_);
	~PoolCore ();

	// allocates one element of size chunk_size
	
	[[nodiscard]] inline void* allocate ()
	{
		void *free_chunk;

		//this->mutex.lock();

		if (this->free_chunks == nullptr) [[unlikely]]
			this->alloc_new_block();

		free_chunk = this->free_chunks;
		this->free_chunks = this->free_chunks->next_chunk;

		//this->mutex.unlock();

		return free_chunk;
	}

	// free one element of size chunk_size

	inline void deallocate (void *p)
	{
		Chunk *chunk = static_cast<Chunk*>(p);

		//this->mutex.lock();

		// we just add the just-freed chunk as the new head of the free_chunks list
		chunk->next_chunk = this->free_chunks;
		this->free_chunks = chunk;

		//this->mutex.unlock();
	}

	static consteval size_t lowest_chunk_size ()
	{
		return sizeof(void*);
	}

private:
	void alloc_new_block ();
	void alloc_chunks_for_block (Block *block);
};

// ---------------------------------------------------

template <typename T>
class PoolCoreSameType : public PoolCore
{
public:
	PoolCoreSameType (const uint32_t chunks_per_block_)
		: PoolCore(sizeof(T), chunks_per_block_, calculate_alignment<T>())
	{
	}

	[[nodiscard]] inline T* allocate ()
	{
		return static_cast<T*>( this->PoolCore::allocate() );
	}

	inline void deallocate (T *p)
	{
		this->PoolCore::deallocate( static_cast<void*>(p) );
	}
};

// ---------------------------------------------------

class PoolManager : public Manager
{
private:
	// Maximum type_size handled by the allocator.
	// Any size greater than it will be directly forwarded to malloc/free.
	size_t max_type_size;
	
	std::vector<PoolCore*> allocators;
	std::vector<PoolCore*> allocators_index;

	void load (std::vector<size_t>& list_type_sizes, const size_t max_block_size);

public:
	// max_block_size: max amount of memory to be allocated per malloc
	PoolManager (std::vector<size_t>& list_type_sizes, const size_t max_block_size = default_block_size);
	PoolManager (std::initializer_list<size_t> list_type_sizes, const size_t max_block_size = default_block_size);
	PoolManager (const size_t max_type_size, const size_t step_size, const size_t max_block_size = default_block_size);

	~PoolManager ();

	[[nodiscard]] void* allocate (const size_t type_size, const size_t count, const uint32_t align) override final
	{
		mylib_assert_exception_msg(count == 1, "PoolManager only supports allocation of one element at a time. Given ", count);
//std::cout << "allocating..." << std::endl;

		void *p;

		if (type_size <= this->max_type_size) [[likely]]
			p = this->allocators_index[type_size]->allocate();
		else {
			p = m_allocate(type_size, align);
			mylib_assert_exception(p != nullptr);
		}

		return p;
	}

	void deallocate (void *p, const size_t type_size, const size_t count, const uint32_t align) override final
	{
		mylib_assert_exception_msg(count == 1, "PoolManager only supports deallocation of one element at a time. Given ", count);
//std::cout << "deallocating..." << std::endl;

		if (type_size <= this->max_type_size) [[likely]]
			this->allocators_index[type_size]->deallocate(p);
		else
			m_deallocate(p, type_size, align);
	}
};

// ---------------------------------------------------

// still under development

#if 0
class datablock_general_alloc_t
{
public:
	using size_t = size_t;

private:
	enum class block_status_t {
		free,
		occupied
	};

	/*
		This class handle chunks differently from Core.
		In Core, the pre-allocated chunks:
			- when a block is free, it stores a linked list of free chunks;
			- when a blodk is occupied, it is 100% used (except when
			  element size < sizeof(void*)) to store the payload.
		Therefore, when a block is occupied, no memory is "wasted".
		The block and payload occupies the same address.

		However, this class (datablock_general_alloc_t) can't work like that,
		because I decided to keep a permanent double-linked list for each chunk's chunks.
		The reason is that I want to be abl to merge consecutive free chunks into one
		when a deallocation is performed, in order to reduce fragmentation.
		Therefore, each block first contains its metadata (Chunk), followed by the payload.
	*/

	struct Block;

	struct Chunk {
		Block *chunk;
		Chunk *left;
		Chunk *right;
		Chunk *previous_free_block;
		Chunk *next_free_block;
		size_t payload_capacity;
		block_status_t status;
	};

	struct Block {
		Chunk *chunks;
		Block *next_chunk;
		uint32_t chunk_size; // chunk_size can be higher than target_chunk_size to allow large allocations
	};

	Block *chunks;
	Chunk *free_chunks;

	OO_ENCAPSULATE_READONLY(size_t, target_chunk_size)

public:
	datablock_general_alloc_t (size_t target_chunk_size=(1024*128));
	~datablock_general_alloc_t ();

	void* alloc (size_t size);
	void release (void *p);

private:
	Chunk* alloc_new_chunk (size_t payload_size);
	Chunk* find_free_block (size_t payload_size);
	Chunk* split_block (Chunk *block, size_t payload_size);

	constexpr size_t calculate_required_space (size_t payload_size)
	{
		return payload_size + sizeof(Chunk);
	}

	constexpr size_t calculate_payload_size (size_t allocated_capacity)
	{
		return allocated_capacity - sizeof(Chunk);
	}
};
#endif

// ---------------------------------------------------

} // end namespace Memory
} // end namespace Mylib

#endif