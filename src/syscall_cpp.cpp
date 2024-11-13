//
// Created by os on 5/21/24.
//
#include "../h/syscall_cpp.hpp"


void* operator new (size_t sz) {return mem_alloc(sz);}

void  operator delete (void* adrr) { mem_free(adrr);}

Thread::Thread(void (*body)(void *), void *arg) : myHandle(nullptr),body(body),arg(arg){}

Thread::~Thread() {}

int Thread::start() {
    if(myHandle) return -1;

    if(body == nullptr){
        thread_create(&myHandle,wrapper,this);
    } else{
        thread_create(&myHandle,body,arg);
    }
    return 0;
}

int Thread::sleep(time_t t) {
    return time_sleep(t);
}

void Thread::dispatch() { thread_dispatch(); }

Thread::Thread() : myHandle(nullptr),body(nullptr),arg(nullptr){ }

void Thread::wrapper(void *arg) {
    if(arg) ((Thread*)arg)->run();
}

Semaphore::Semaphore(unsigned int init) {
    sem_open(&myHandle,init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

int Semaphore::timedWait(time_t time) {
    return sem_timedwait(myHandle,time);
}

int Semaphore::tryWait() {
    return sem_trywait(myHandle);
}

void PeriodicThread::terminate() {
    period = 0;
}

PeriodicThread::PeriodicThread(time_t period):period(period) {}

void PeriodicThread::run(){
    while(period){
        periodicActivation();
        time_sleep(period);
    }
}
char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}
