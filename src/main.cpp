//
// Created by marko on 20.4.22..
//
#include "../h/tcb.hpp"
#include "../test/printing.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/kConsole.hpp"
#include "../h/MemoryAllocator.hpp"

extern void userMain();



//static sem_t waitAll;

void wrapperTest(void* arg){
   userMain();
}

int main() {

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    TCB* main;
    kConsole::init();
    thread_create(&main, nullptr, nullptr);
    TCB::running=main;
    TCB* user;
    thread_create(&user, wrapperTest, nullptr);
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    while(!(user->isFinished())) thread_dispatch();



    printString("Finished\n");
    asm volatile("li t0, 0x5555");
    asm volatile("li t1, 0x100000");
    asm volatile("sw t0, 0(t1)");
    return 0;

}
