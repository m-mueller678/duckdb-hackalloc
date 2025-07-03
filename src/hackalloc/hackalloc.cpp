#include "hackalloc.h"
#include "duckdb/common/allocator.hpp"

#include <iostream>

namespace duckdb {
data_ptr_t hackalloc_allocate(PrivateAllocatorData *private_data, idx_t size) {
	auto default_allocate_result = malloc(size);
	std::cout << "A" << size << std::endl;
	if (!default_allocate_result) {
		throw std::bad_alloc();
	}
	return data_ptr_cast(default_allocate_result);
}

void hackalloc_free(PrivateAllocatorData *private_data, data_ptr_t pointer, idx_t size) {
	std::cout << "F" << size << std::endl;
	free(pointer);
}

data_ptr_t hackalloc_reallocate(PrivateAllocatorData *private_data, data_ptr_t pointer, idx_t old_size, idx_t size) {
	std::cout << "R" << old_size << ">" << size << std::endl;
	return data_ptr_cast(realloc(pointer, size));
}
} // namespace duckdb