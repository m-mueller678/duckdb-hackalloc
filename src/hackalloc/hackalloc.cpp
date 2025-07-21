#include "hackalloc.h"
#include "duckdb/common/allocator.hpp"

#include <atomic>
#include <iostream>
#include <mutex>

namespace duckdb {

static uint64_t alloc_size = 0;
static constexpr uint64_t alloc_alignment = 16;
static std::atomic<data_ptr_t> global_memory {nullptr};
static std::atomic<uint64_t> global_memory_offset {0};
std::mutex alloc_lock;

constexpr uint64_t local_batch_size = 1 << 24;

static uint64_t hackalloc_config() {
	const auto env = getenv("HACK_ALLOC");
	return env ? std::stoul(env) : 0;
}

static data_ptr_t maybe_init_global() {
	alloc_lock.lock();
	data_ptr_t ptr = global_memory.load(std::memory_order_relaxed);
	if (ptr == nullptr) {
		alloc_size = hackalloc_config() << 30;
		ptr = data_ptr_cast(malloc(alloc_size));
		for (uint64_t i = 0; i < alloc_size; i += (1 << 12)) {
			ptr[i] = 0;
		}
		memset(ptr, 0, alloc_size);
		global_memory.store(ptr, std::memory_order_relaxed);
	}
	alloc_lock.unlock();
	return ptr;
}

static data_ptr_t hackalloc_allocate_global(idx_t size) {
	data_ptr_t ptr = global_memory.load(std::memory_order_relaxed);
	if (ptr == nullptr) [[unlikely]] {
		ptr = maybe_init_global();
	}
	uint64_t offset = global_memory_offset.fetch_add(size, std::memory_order_relaxed);
	if (offset + size > alloc_size) [[unlikely]] {
		std::cout << "hackalloc out of memory" << std::endl;
		abort();
	}
	return ptr + offset;
}

thread_local uint64_t local_remaining = 0;
thread_local data_ptr_t local_bump = nullptr;

data_ptr_t hackalloc_allocate(PrivateAllocatorData *private_data, idx_t size) {
	size += alloc_alignment - 1;
	size -= size % alloc_alignment;
	if (local_remaining < size) {
		if (size > local_batch_size / 2) {
			return hackalloc_allocate_global(size);
		}
		local_bump = hackalloc_allocate_global(local_batch_size) + local_batch_size;
		local_remaining = local_batch_size;
	}
	local_bump -= size;
	local_remaining -= size;
	return local_bump;
}

void hackalloc_free(PrivateAllocatorData *private_data, data_ptr_t pointer, idx_t size) {
}

data_ptr_t hackalloc_reallocate(PrivateAllocatorData *private_data, data_ptr_t pointer, idx_t old_size, idx_t size) {
	auto new_alloc = hackalloc_allocate(private_data, size);
	memmove(new_alloc, pointer, std::min(size, old_size));
	hackalloc_free(private_data, pointer, old_size);
	return new_alloc;
}
bool use_hack_alloc() {
	return hackalloc_config() != 0;
}
} // namespace duckdb