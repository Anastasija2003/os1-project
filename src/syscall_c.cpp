#include "../h/syscall_c.hpp"
#include "../test/printing.hpp"

void* mem_alloc (size_t size){
    size_t volatile sz = size;
    if(sz%MEM_BLOCK_SIZE != 0){
        sz = sz + MEM_BLOCK_SIZE - sz%MEM_BLOCK_SIZE;
    }
    sz = sz / MEM_BLOCK_SIZE;
    __asm__ volatile("mv a1,%0": : "r"(sz));
    __asm__ volatile("li a0, 0x01");
    void* adrr = (void *)callRoutine();
    return adrr;
}

int mem_free (void* adrr){
    __asm__ volatile("mv a1,%0": : "r"(adrr));
    __asm__ volatile("li a0, 0x02");

    int status = (int) callRoutine();
    return status;
}

int thread_create(thread_t* handle,void(*start_routine)(void*),void* arg){
    void *stack = nullptr;
    if(start_routine != nullptr) {
        stack = mem_alloc(DEFAULT_STACK_SIZE);
    }
    __asm__ volatile("mv a4,%0": : "r"(stack));
    __asm__ volatile("mv a3,%0": : "r"(arg));
    __asm__ volatile("mv a2,%0": : "r"(start_routine));
    __asm__ volatile("mv a1,%0": : "r"(handle));
    __asm__ volatile("li a0, 0x11");
    int status = (int) callRoutine();
    return status;
}

int thread_exit (){
    __asm__ volatile("li a0, 0x12");
    int status = (int) callRoutine();
    return status;
}

void thread_dispatch (){
    __asm__ volatile("li a0, 0x13");
    callRoutine();
}

int sem_open (sem_t* handle,unsigned init){
    __asm__ volatile("mv a2,%0": : "r"(init));
    __asm__ volatile("mv a1,%0": : "r"(handle));
    __asm__ volatile("li a0, 0x21");
    int status = (int) callRoutine();
    return status;
}

int sem_close (sem_t handle){
    __asm__ volatile("mv a1,%0": : "r"(handle));
    __asm__ volatile("li a0, 0x22");
    int status = (int) callRoutine();
    return status;
}

int sem_wait (sem_t id){
    __asm__ volatile("mv a1,%0": : "r"(id));
    __asm__ volatile("li a0, 0x23");
    int status = (int) callRoutine();
    return status;
}

int sem_signal (sem_t id){
    __asm__ volatile("mv a1,%0": : "r"(id));
    __asm__ volatile("li a0, 0x24");
    int status = (int) callRoutine();
    return status;
}

int sem_timedwait(sem_t id,time_t timeout){
    __asm__ volatile("mv a2,%0": : "r"(timeout));
    __asm__ volatile("mv a1,%0": : "r"(id));
    __asm__ volatile("li a0, 0x25");
    int status = (int) callRoutine();
    return status;
}

int sem_trywait(sem_t id){
    __asm__ volatile("mv a1,%0": : "r"(id));
    __asm__ volatile("li a0, 0x26");
    int status = (int) callRoutine();
    return status;
}

int time_sleep (time_t t){
    __asm__ volatile("mv a1,%0": : "r"(t));
    __asm__ volatile("li a0, 0x31");
    int status = (int) callRoutine();
    return status;
}

char getc (){
    __asm__ volatile("li a0, 0x41");
    volatile char c = (char) callRoutine();
    return c;
}

void putc (char c){
    __asm__ volatile("mv a1,%0": : "r"(c));
    __asm__ volatile("li a0, 0x42");
    callRoutine();
}


uint64 callRoutine(){
    __asm__ volatile("ecall");
    uint64 volatile status;
    __asm__ volatile("mv %0,a0": "=r"(status));
    return status;
}
