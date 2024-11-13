//
// Created by marko on 20.4.22..
//

#include "../h/tcb.hpp"
void idleBody(void *arg){
    while(1) thread_dispatch();
}
TCB *TCB::running = nullptr;
TCB *TCB::headSleep = nullptr;
uint64 TCB::timeSliceCounter = 0;
TCB *TCB::headWait = nullptr;
TCB *TCB::idle = nullptr;

TCB *TCB::createThread(Body body,void *stack,void *arg)
{
    return new TCB(body, DEFAULT_TIME_SLICE,stack,arg);
}

void TCB::yield()
{
    thread_dispatch();
}

void TCB::dispatch()
{
    TCB *old = running;
    Context *oldContext = &old->context;
    if (!old->isFinished()) { Scheduler::put(old); }
    else { delete old; }
    running = Scheduler::get();
    TCB::timeSliceCounter = 0;
    if(!running){
        if(!idle) {
            printString("a\n");
            thread_create(&idle,idleBody, nullptr);
        }
        running = idle;
    }
    TCB::contextSwitch(oldContext, &running->context);
}

void TCB::threadWrapper()
{
    if(running->body != nullptr
    ){
        Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
    }
    Riscv::popSppSpie();
    running->body(running->arguments);
    running->setFinished(true);
    thread_exit();
}

int TCB::exit() {
    if(running == nullptr || running->body == nullptr) return -1;
    running->setFinished(true);
    TCB::yield();
    return 0;
}

int TCB::sleep(time_t t) {
    if(t<1) return 0;
    if(headSleep == nullptr){
        headSleep = TCB::running;
        running->periodsSleeping = t;
    } else{
        TCB *tmp = headSleep;
        TCB *prev = nullptr;
        time_t periods = t;
        while(tmp && periods){
            if(periods<tmp->periodsSleeping) break;
            periods = periods-tmp->periodsSleeping;
            prev = tmp;
            tmp = tmp->sleepNext;
        }
        if(tmp) tmp->periodsSleeping -= periods;
        if(prev) prev->sleepNext = TCB::running;
        else headSleep = TCB::running;
        TCB::running->sleepNext = tmp;
        TCB::running->periodsSleeping = periods;
    }
    TCB *old = running;
    running = Scheduler::get();
    TCB::contextSwitch(&old->context, &running->context);
    return 0;
}

void TCB::wake() {
    if(headSleep == nullptr) return;
    headSleep->periodsSleeping--;
    while(headSleep && headSleep->periodsSleeping == 0){
        TCB *tmp = headSleep;
        headSleep = headSleep->sleepNext;
        tmp->sleepNext = nullptr;
        Scheduler::put(tmp);
    }
}

