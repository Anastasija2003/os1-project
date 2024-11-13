//
// Created by marko on 20.4.22..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP

#include "../lib/hw.h"
#include "Scheduler.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../lib/mem.h"

class kSemaphore;

typedef unsigned long time_t;
class TCB
{
public:
    ~TCB() { mem_free(stack); }

    bool isFinished() const { return finished; }

    void setFinished(bool value) { finished = value; }

    uint64 getTimeSlice() const { return timeSlice; }

    using Body = void (*)(void *);

    static TCB *createThread(Body body,void *stack,void *arg);

    static void yield();

    static TCB *running;

    static int exit();

    static int sleep(time_t);

    static void wake();


    void *operator new(size_t n){ return MemoryAllocator::mem_alloc(MemoryAllocator::mem_in_blocks(n)); }

    void *operator new[](size_t n){ return MemoryAllocator::mem_alloc(MemoryAllocator::mem_in_blocks(n)); }

    void operator delete(void *p)  { MemoryAllocator::mem_free(p); }

    void operator delete[](void *p)  { MemoryAllocator::mem_free(p); }

    //void setKernelThread(bool kernelThread) { TCB::kernelThread = kernelThread; }

private:
    TCB(Body body, uint64 timeSlice,void *stack,void *arg) :
            body(body),
            stack(body != nullptr ? (char*) stack : nullptr),
            context({(uint64) &threadWrapper,
                     stack != nullptr ? (uint64) &this->stack[DEFAULT_STACK_SIZE] : 0
                    }),
            timeSlice(timeSlice),
            finished(false),
            arguments(arg)
    {
        if (body != nullptr) { Scheduler::put(this); }
    }

    struct Context
    {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    char *stack;
    Context context;
    uint64 timeSlice;
    bool finished;
    void *arguments;
    friend class Riscv;
    friend class Scheduler;
    friend class kSemaphore;
    static void threadWrapper();
    time_t periodsSleeping = 0;
    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();

    bool kernelThread = false;
    static uint64 timeSliceCounter;
    static TCB *headSleep;
    static TCB *headWait;
    TCB *sleepNext = nullptr;
    TCB *schedulerNext = nullptr;
    TCB *schedulerPrev = nullptr;
    TCB *blockedNext = nullptr;
    TCB *blockedNextTimer = nullptr;
    int returnStatus = 0;
    bool timerBlocked = false;
    static TCB* idle;
    kSemaphore *blockedOnSem = nullptr;
    uint64 timerBlockedCounter = 0;
};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
