#include <algorithm>

#include <my-lib/memory-pool.h>

namespace Mylib
{
namespace Memory
{

// ---------------------------------------------------

PoolCore::PoolCore (const size_t type_size_, const uint32_t chunks_per_block_, const uint32_t align_)
	: type_size(type_size_), chunks_per_block(chunks_per_block_), align(align_),
	  chunk_size((type_size_ < lowest_chunk_size()) ? lowest_chunk_size() : type_size_)
{
	// we need space to store at least a pointer in each chunk, for the linked list of free chunks
}

PoolCore::~PoolCore ()
{
	Block *block, *next;

	for (block = this->blocks; block != nullptr; block = next) {
		next = block->next_block;
		m_deallocate(block->chunks, this->chunk_size * this->chunks_per_block, this->align);
		delete block;
	}
}

void PoolCore::alloc_new_block ()
{
	Block *new_block;
	
	new_block = new Block;
	this->alloc_chunks_for_block(new_block);
	new_block->next_block = this->blocks;
	this->blocks = new_block;
}

void PoolCore::alloc_chunks_for_block (Block *block)
{
	Chunk *chunk;

	// First, we allocate memory for #chunks_per_block elements.
	// Remember that we don't use sizeof(T) because we need memory for at least a pointer.
	// When the chunk is empty, it is part of the free_chunks linked_list.
	// When we remove an element, we remove its memory from the free_chunks linked list
	//   and use the memory to store user data instead.

	block->chunks = static_cast<Chunk*>( m_allocate(this->chunk_size * this->chunks_per_block, this->align) );
	mylib_assert_exception(block->chunks != nullptr);

	chunk = block->chunks;

	static_assert(sizeof(uint8_t) == 1);

	for (uint32_t i = 0; i < this->chunks_per_block-1; i++) {
		chunk->next_chunk = reinterpret_cast<Chunk*>( reinterpret_cast<uint8_t*>(chunk) + this->chunk_size );
		chunk = chunk->next_chunk;
	}

	/*
		now, we setup the last allocated chunk

		when free_chunks is empty, last chunk points to nothing
		when free_chunks still has chunks, we merge the lists
	*/
	
	chunk->next_chunk = this->free_chunks;
	this->free_chunks = block->chunks;
}

// ---------------------------------------------------

PoolManager::PoolManager (std::vector<size_t>& list_type_sizes, const size_t max_block_size)
{
	this->load(list_type_sizes, max_block_size);
}

PoolManager::PoolManager (std::initializer_list<size_t> list_type_sizes, const size_t max_block_size)
{
	std::vector<size_t> v = list_type_sizes;
	this->load(v, max_block_size);
}

PoolManager::PoolManager (const size_t max_type_size, const size_t step_size, const size_t max_block_size)
{
	std::vector<size_t> list_type_sizes;

	list_type_sizes.reserve(max_type_size / step_size + 5); // 1...2...5... whatever

	for (size_t type_size = step_size; type_size < max_type_size; type_size += step_size)
		list_type_sizes.push_back(type_size);
	list_type_sizes.push_back(max_type_size);
	
	this->load(list_type_sizes, max_block_size);
}

PoolManager::~PoolManager ()
{
	for (PoolCore *allocator: this->allocators)
		delete allocator;
}

void PoolManager::load (std::vector<size_t>& list_type_sizes, const size_t max_block_size)
{
	// we remove values lower than the minimum
	std::for_each(list_type_sizes.begin(), list_type_sizes.end(),
		[] (size_t& v) -> void {
			if (v < PoolCore::lowest_chunk_size())
				v = PoolCore::lowest_chunk_size();
		}
	);

	// we need the allocators to be sorted in order to create the index
	std::sort(list_type_sizes.begin(), list_type_sizes.end(),
		[] (const size_t a, const size_t b) -> bool {
			return (a < b);
		}
	);

	// let's also remove any duplicate entries
	auto last = std::unique(list_type_sizes.begin(), list_type_sizes.end());
	list_type_sizes.erase(last, list_type_sizes.end());

	this->allocators.reserve( list_type_sizes.size() );

	for (const size_t type_size : list_type_sizes) {
		const size_t chunks_per_block = max_block_size / type_size;
		PoolCore *allocator = new PoolCore(type_size, chunks_per_block, __STDCPP_DEFAULT_NEW_ALIGNMENT__);
		this->allocators.push_back(allocator);
	}

	this->max_type_size = (*(this->allocators.end() - 1))->get_chunk_size();

#if 0
	for (Core *allocator: this->allocators)
		std::cout << "alloc size " << allocator->get_chunk_size() << std::endl;
	std::cout << "max size is " << this->max_size << std::endl;
#endif

	// now, let's create an index for a O(1) time complexity

	this->allocators_index.resize(this->max_type_size + 1, nullptr);

	size_t type_size = 1;
	for (PoolCore *allocator : this->allocators) {
		while (type_size <= allocator->get_chunk_size()) {
			this->allocators_index[type_size] = allocator;
			type_size++;
		}
	}

#if 0
	for size_t i=0; i<this->allocators_index.size(); i++) {
		size_t s = (this->allocators_index[i] == nullptr) ? 0 : this->allocators_index[i]->get_chunk_size();
		std::cout << "index " << i << " size " << s << std::endl;
	}
#endif
}

// ---------------------------------------------------

#if 0

datablock_general_alloc_t::datablock_general_alloc_t (size_t target_chunk_size)
{
	std::cout << "still under devepment!" << std::endl;
	assert(0);

	this->target_chunk_size = target_chunk_size;

	this->chunks = nullptr;
	this->free_chunks = nullptr;
}

datablock_general_alloc_t::~datablock_general_alloc_t ()
{
	Block *chunk, *next;

	for (chunk=this->chunks; chunk!=nullptr; chunk=next) {
		next = chunk->next_chunk;
		::operator delete(chunk->chunks);
		delete chunk;
	}
}

void* datablock_general_alloc_t::alloc (size_t size)
{
	Chunk *block;

	if (this->free_chunks == nullptr) [[unlikely]]
		block = this->alloc_new_chunk(size);
	else {
		block = this->find_free_block(size);

		if (block == nullptr)
			block = this->alloc_new_chunk(size);
	}

	Chunk *new_block = this->split_block(block, size);
	new_block->status = block_status_t::occupied;

	// +1 because payload data follows the block metadata
	return static_cast<void*>(new_block + 1);
}

datablock_general_alloc_t::Chunk* datablock_general_alloc_t::alloc_new_chunk (size_t payload_size)
{
	const size_t required_space = calculate_required_space(payload_size);
	const size_t chunk_size = (required_space > this->target_chunk_size) ? required_space : this->target_chunk_size;

	Block *new_chunk = new Block;
	new_chunk->chunk_size = chunk_size;
	new_chunk->chunks = static_cast<Chunk*>(::operator new(chunk_size));
	new_chunk->next_chunk = this->chunks;
	this->chunks = new_chunk;

	// Since we just allocated a new chunk, this chunk has only one block initially.
	// Therefore, we setup it's linked list accordingly.

	Chunk *block = new_chunk->chunks;
	block->chunk = new_chunk;
	block->left = nullptr;
	block->right = nullptr;
	block->payload_capacity = calculate_payload_size(chunk_size);
	block->status = block_status_t::free;

	// we also add the block to the free_chunks double-linked list
	block->previous_free_block = nullptr;
	block->next_free_block = this->free_chunks;
	this->free_chunks = block;

	return block;
}

datablock_general_alloc_t::Chunk* datablock_general_alloc_t::find_free_block (size_t payload_size)
{
	Chunk *target_block = nullptr;

	for (Chunk *block=this->free_chunks; block!=nullptr; block=block->next_free_block) {
		if (block->payload_capacity >= payload_size && block->status == block_status_t::free) {
			target_block = block;
			break;
		}
	}

	return target_block;
}

datablock_general_alloc_t::Chunk* datablock_general_alloc_t::split_block (Chunk *block, size_t payload_size)
{
	return nullptr;
}

void datablock_general_alloc_t::release (void *p)
{
	bool need_to_insert_in_free_list = true;
	bool merged_left;

	// -1 because payload data follows the block metadata
	Chunk *block = static_cast<Chunk*>(p) - 1;
	block->status = block_status_t::free;

	// lets check if we can merge with the left block
	if (block->left != nullptr && block->left->status == block_status_t::free) {
		Chunk *block_left = block->left;
		Chunk *block_right = block->right;

		block_left->payload_capacity += calculate_required_space(block->payload_capacity);
		
		block_left->right = block_right;

		if (block_right != nullptr)
			block_right->left = block_left;

		block = block_left; // so we can check if we will also merge with the right block
		need_to_insert_in_free_list = false; // since the left block is already in free_chunks list
		merged_left = true;
	}
	else
		merged_left = false;
	
	// lets check if we can merge with the right block
	if (block->right != nullptr && block->right->status == block_status_t::free) {
		Chunk *block_left = block->left;
		Chunk *block_right = block->right;

		// the merge must preserve th left block, since the
		// block metadata must be in the beginning of the block storage


	
		need_to_insert_in_free_list = false; // since the left block is already in free_chunks list

		// check if we are performing a double merge (with left and right)
		// in this case, since both left and right were originally in the free_chunks list,
		// we, need to remove the duplicate entry from free_chunks
		// lets remove the right block then
		if (merged_left) {
			//Block *chunk = block->chunk;
			Chunk *previous = block->right->previous_free_block;
			Chunk *next = block->right->next_free_block;

			
		}
		merged_left = true;
	}

	if (need_to_insert_in_free_list) {
		block->next_free_block = this->free_chunks;
		block->previous_free_block = nullptr;
		this->free_chunks = block;
	}
}

#endif

// ---------------------------------------------------

} // end namespace Memory
} // end namespace Mylib
