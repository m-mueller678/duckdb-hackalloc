#ifndef HACKALLOC_H
#define HACKALLOC_H

#include "duckdb/common/allocator.hpp"

namespace duckdb {

data_ptr_t hackalloc_allocate(PrivateAllocatorData *private_data, idx_t size);

void hackalloc_free(PrivateAllocatorData *private_data, data_ptr_t pointer, idx_t size);

data_ptr_t hackalloc_reallocate(PrivateAllocatorData *private_data, data_ptr_t pointer, idx_t old_size, idx_t size);
bool use_hack_alloc();

} // namespace duckdb

#endif // HACKALLOC_H
