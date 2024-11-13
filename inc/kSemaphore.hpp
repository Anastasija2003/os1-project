
#ifndef PROJECT_BASE_KSEMAPHORE_HPP
#define PROJECT_BASE_KSEMAPHORE_HPP

#include "tcb.hpp"
#include "../lib/hw.h"
#include "Scheduler.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../test/printing.hpp"
#include "../lib/mem.h"

class kSemaphore{
public:

    static kSemaphore* create_sem(uint64 init);

    int close();

    int wait();

    int signal();

    int timedWait(time_t time);

    int trywait();


    void *operator new(size_t n){ return MemoryAllocator::mem_alloc(MemoryAllocator::mem_in_blocks(n)); }

    void *operator new[](size_t n){ return MemoryAllocator::mem_alloc(MemoryAllocator::mem_in_blocks(n)); }

    void operator delete(void *p)  { MemoryAllocator::mem_free(p); }

    void operator delete[](void *p)  { MemoryAllocator::mem_free(p); }

    uint64 getVal() const { return val; }

    bool isOpen() const { return open; }

    static void getWaitTimer();

private:
    kSemaphore(uint64 val):blockedHead(nullptr),blockedTail(nullptr),val(val),open(true){}
    TCB *blockedHead;
    TCB *blockedTail;
    friend class Riscv;
    int val;

    bool volatile open = false;
};
#endif
