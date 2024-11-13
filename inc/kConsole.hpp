//
// Created by os on 5/16/24.
//

#ifndef PROJECT_BASE_KCONSOLE_HPP
#define PROJECT_BASE_KCONSOLE_HPP

#include "IOBuffer.hpp"
#include "../lib/mem.h"
class kConsole{
    static IOBuffer* input;
    static IOBuffer* output;
public:
    void *operator new(size_t n){ return MemoryAllocator::mem_alloc(MemoryAllocator::mem_in_blocks(n)); }

    void *operator new[](size_t n){ return MemoryAllocator::mem_alloc(MemoryAllocator::mem_in_blocks(n)); }

    void operator delete(void *p)  { MemoryAllocator::mem_free(p); }

    void operator delete[](void *p)  { MemoryAllocator::mem_free(p); }

    static void  init();

    static IOBuffer *getInput() { return input; }

    static IOBuffer *getOutput() { return output; }

    static char getc();

    static void putc(char c);

};

#endif //PROJECT_BASE_KCONSOLE_HPP
