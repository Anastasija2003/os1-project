#ifndef PROJECT_BASE_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_MEMORYALLOCATOR_HPP

#include "../lib/hw.h"

struct FreeMem{
    FreeMem *next;
    FreeMem *prev;
    bool used;
    size_t size;
};

class MemoryAllocator{
private:
    static bool made;
    static FreeMem* free;
    static void init();
public:
    static int printMemory();
    static void* mem_alloc(size_t size);
    static int mem_free(void* adrr);
    static uint64 mem_in_blocks(size_t size);
    static bool join(FreeMem *block);
};
#endif
