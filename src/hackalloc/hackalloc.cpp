#include "hackalloc.h"
#include "duckdb/common/allocator.hpp"

#include <atomic>
#include <iostream>
#include <mutex>

namespace duckdb {

static uint64_t next_alloc_id;
static std::mutex alloc_map_lock;
static std::unordered_map<void *, uint64_t> alloc_map;

data_ptr_t hackalloc_allocate(PrivateAllocatorData *private_data, idx_t size) {
	auto default_allocate_result = malloc(size);
	if (!default_allocate_result) {
		throw std::bad_alloc();
	}
	auto ret = data_ptr_cast(default_allocate_result);
	{
		alloc_map_lock.lock();
		auto id = ++next_alloc_id;
		std::cout << "A" << id << std::endl;
		alloc_map.emplace(ret, id);
		alloc_map_lock.unlock();
	}
	return ret;
}

void hackalloc_free(PrivateAllocatorData *private_data, data_ptr_t pointer, idx_t size) {
	{
		alloc_map_lock.lock();
		auto id = alloc_map[pointer];
		std::cout << "F" << id << std::endl;
		alloc_map_lock.unlock();
	}
	free(pointer);
}

data_ptr_t hackalloc_reallocate(PrivateAllocatorData *private_data, data_ptr_t pointer, idx_t old_size, idx_t size) {
	auto new_alloc = hackalloc_allocate(private_data, size);
	memmove(new_alloc, pointer, std::min(size, old_size));
	hackalloc_free(private_data, pointer, old_size);
	return new_alloc;
}
} // namespace duckdb