//
// Created by marko on 20.4.22..
//

#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/kSemaphore.hpp"
#include "../test/printing.hpp"
#include "../h/kConsole.hpp"
#include "../lib/mem.h"
using Body = void (*)(void *);
void Riscv::popSppSpie()
{
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

void Riscv::handleSupervisorTrap()
{
    uint64 volatile code;
    uint64 volatile a1;
    uint64 volatile a2;
    uint64 volatile a3;
    uint64 volatile a4;
    uint64 volatile a5;
    uint64 volatile a6;
    uint64 volatile a7;
    __asm__ volatile("mv %0,a0":"=r"(code));
    __asm__ volatile("mv %0,a1":"=r"(a1));
    __asm__ volatile("mv %0,a2":"=r"(a2));
    __asm__ volatile("mv %0,a3":"=r"(a3));
    __asm__ volatile("mv %0,a4":"=r"(a4));
    __asm__ volatile("mv %0,a5":"=r"(a5));
    __asm__ volatile("mv %0,a6":"=r"(a6));
    __asm__ volatile("mv %0,a7":"=r"(a7));
    uint64 scause = r_scause();
    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL)
    {
        // interrupt: no; cause code: environment call from U-mode(8) or S-mode(9)
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();
        if(code == 0x01){                               //mem_alloc
            size_t size = (size_t) a1;
            void *addr = MemoryAllocator::mem_alloc(size);
            __asm__ volatile("mv a0,%0"::"r"(addr));
            __asm__ volatile("sd a0, 8 * 10 (fp)");
        } else if (code == 0x02){                       //mem_free
            void *addr = (void *)a1;
            int status = MemoryAllocator::mem_free(addr);
            __asm__ volatile("mv a0,%0"::"r"(status));
            __asm__ volatile("sd a0, 8 * 10(fp)");
        }else if(code == 0x11){                         //thread_create
            TCB **thread = (TCB**) a1;
            Body body = (Body) a2;
            void *arg = (void *) a3;
            void *stack = (void *) a4;
            int status = 0;
            *thread = (TCB::createThread(body,stack,arg));
            if(*thread == nullptr) status = -1;
            __asm__ volatile("mv a0,%0"::"r"(status));
            __asm__ volatile("sd a0, 8 * 10(fp)");
        }else if(code == 0x12){                         //thread_exit
            int status = TCB::exit();
            __asm__ volatile("mv a0,%0"::"r"(status));
            __asm__ volatile("sd a0, 8 * 10(fp)");
        }
        else if(code == 0x13){                          //thread_dispatch
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
        }else if(code == 0x21){                        //sem_open
            kSemaphore **sem = (kSemaphore**) a1;
            int val = (int)  a2;
            *sem = kSemaphore::create_sem(val);
            int status = 0;
            if(*sem == nullptr) status = -1;
            __asm__ volatile("mv a0,%0"::"r"(status));
            __asm__ volatile("sd a0, 8 * 10(fp)");
        }else if(code == 0x22){                         //sem_close
            kSemaphore *sem = (kSemaphore*) a1;
            int status = 0;
            if(sem == nullptr) status = -1;
            else status = sem->close();
            //MemoryAllocator::mem_free(sem);
            __asm__ volatile("mv a0,%0"::"r"(status));
            __asm__ volatile("sd a0, 8 * 10(fp)");
        }else if(code == 0x23){                         //sem_wait
            kSemaphore *sem = (kSemaphore*) a1;
            int status = 0;
            if(sem == nullptr) status = -1;
            else status = sem->wait();
            __asm__ volatile("mv a0,%0"::"r"(status));
            __asm__ volatile("sd a0, 8 * 10(fp)");
        }else if(code == 0x24){                         //sem_signal
            kSemaphore *sem = (kSemaphore*) a1;
            int status = 0;
            if(sem == nullptr) status = -1;
            else status = sem->signal();
            __asm__ volatile("mv a0,%0"::"r"(status));
            __asm__ volatile("sd a0, 8 * 10(fp)");
        }else if(code == 0x25){                         //sem_timedwait
            kSemaphore *sem = (kSemaphore*)a1;
            time_t t = (time_t) a2;
            int status = 0;
            if(sem == nullptr) status = -1;
            else status = sem->timedWait(t);
            __asm__ volatile("mv a0,%0"::"r"(status));
            __asm__ volatile("sd a0, 8 * 10(fp)");
        }else if(code == 0x26){                         //sem_trywait
            kSemaphore *sem = (kSemaphore*) a1;
            int status = 0;
            if(sem == nullptr) status = -1;
            else status = sem->trywait();
            __asm__ volatile("mv a0,%0"::"r"(status));
            __asm__ volatile("sd a0, 8 * 10(fp)");
        }
        else if(code == 0x31){                         //time_sleep
            uint64 time = a1;
            int status = TCB::sleep(time);
            __asm__ volatile("mv a0,%0"::"r"(status));
            __asm__ volatile("sd a0, 8 * 10(fp)");
        }
        else if(code == 0x41){  //getc
            char volatile status = *((char *)CONSOLE_STATUS);
            while( (CONSOLE_RX_STATUS_BIT & status) && !kConsole::getInput()->isFull()){
                char* volatile rx = (char *)CONSOLE_RX_DATA;
                char volatile c = *rx;
                //putc(c);
                kConsole::getInput()->put(c);
                status = *((char *)CONSOLE_STATUS);
            }
            char volatile c = kConsole::getc();
            __asm__ volatile("mv a0,%0"::"r"(c));
            __asm__ volatile("sd a0, 8 * 10(fp)");
        }
        else if(code == 0x42){ //putc
            volatile char c = (char) a1;
            kConsole::putc(c);
            char volatile status = *((char *)CONSOLE_STATUS);
            while ((status &  CONSOLE_TX_STATUS_BIT ) && !kConsole::getOutput()->isEmpty()) {
                char  c = kConsole::getOutput()->get();
                *((uint64 *) CONSOLE_TX_DATA)= c;
                status = *((char *)CONSOLE_STATUS);
            }
        }
        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    else if (scause == 0x8000000000000001UL)
    {
        // interrupt: yes; cause code: supervisor software interrupt (CLINT; machine timer interrupt)

        mc_sip(SIP_SSIP);
        TCB::wake(); // skloni i iz wait timer dodaj bool
        kSemaphore::getWaitTimer();
        TCB::timeSliceCounter++;
        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice())
        {
            uint64 volatile sepc = r_sepc();
            uint64 volatile sstatus = r_sstatus();
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
            w_sstatus(sstatus);
            w_sepc(sepc);
        }
    }
    else if (scause == 0x8000000000000009UL)
    {
        // interrupt: yes; cause code: supervisor external interrupt (PLIC; could be keyboard)
        uint64 num = plic_claim();
        if( num == CONSOLE_IRQ){
            char volatile status = *((char *)CONSOLE_STATUS);
            while( (CONSOLE_RX_STATUS_BIT & status) && !kConsole::getInput()->isFull()){
                char* volatile rx = (char *)CONSOLE_RX_DATA;
                char volatile c = *rx;
                putc(c);
                kConsole::getInput()->put(c);
                status = *((char *)CONSOLE_STATUS);
            }
            status = *((char *)CONSOLE_STATUS);
            while ((status &  CONSOLE_TX_STATUS_BIT ) && !kConsole::getOutput()->isEmpty()) {
                char  c = kConsole::getOutput()->get();
                *((uint64 *) CONSOLE_TX_DATA)= c;
                status = *((char *)CONSOLE_STATUS);
            }

        }
        plic_complete((int) num);
    }
    else
    {
        uint64 volatile sepc = r_sepc();
        uint64 volatile scause = r_scause();
        printString("scause ");
        printInt(scause);
        printString("\n");
        printString("sepc ");
        printInt(sepc,16);
        printString("\n");
        while(true);
    }
}