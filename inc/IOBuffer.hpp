//
// Created by os on 5/16/24.
//

#ifndef PROJECT_BASE_IOBUFFER_HPP
#define PROJECT_BASE_IOBUFFER_HPP
#include "MemoryAllocator.hpp"
#include "../lib/hw.h"
#include "kSemaphore.hpp"

class IOBuffer{
private:
    char *buffer;
    size_t head;
    size_t tail;
    size_t size;
    kSemaphore *spaceAvailable;
    kSemaphore *itemAvailable;
public:
    IOBuffer(int capacity):head(0),tail(0),size(capacity){
        buffer = (char *)mem_alloc(capacity* sizeof(char));
        sem_open(&this->spaceAvailable,capacity);
        sem_open(&this->itemAvailable,0);
    }

    ~IOBuffer(){
        delete[] buffer;
    }
    void put(char c);

    char get();

    bool isFull() const { return (tail + 1) % size == head; }

    bool isEmpty() const { return head == tail; }

    void *operator new(size_t n){ return MemoryAllocator::mem_alloc(MemoryAllocator::mem_in_blocks(n)); }

    void *operator new[](size_t n){ return MemoryAllocator::mem_alloc(MemoryAllocator::mem_in_blocks(n)); }

    void operator delete(void *p)  { MemoryAllocator::mem_free(p); }

    void operator delete[](void *p)  { MemoryAllocator::mem_free(p); }

};
#endif //PROJECT_BASE_IOBUFFER_HPP
