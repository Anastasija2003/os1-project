//
// Created by os on 5/14/24.
//
#include "../h/kSemaphore.hpp"

kSemaphore *kSemaphore::create_sem(uint64  init) {
    return new kSemaphore(init);
}

int kSemaphore::close() {
    if(!open) return -1;
    while(blockedHead){
        blockedHead->returnStatus = -1;
        TCB* tmp = blockedHead;
        tmp->blockedNext = nullptr;
        if(tmp->timerBlocked){
            TCB *curr = TCB::headWait;
            TCB *prev = nullptr;
            while(1){
                if(curr == tmp) break;
                prev = curr;
                curr = curr->blockedNextTimer;
            }
            if(prev) prev->blockedNextTimer = curr->blockedNextTimer;
            else TCB::headWait = curr->blockedNextTimer;
            tmp->timerBlockedCounter = 0;
            tmp->blockedNextTimer = nullptr;
            tmp->timerBlocked = false;
            tmp->blockedOnSem = nullptr;
        }
        Scheduler::put(tmp);
        blockedHead = blockedHead->blockedNext;
    }
    this->open = false;
    return 0;
}

int kSemaphore::wait() {
    if(!open || TCB::running == nullptr) return -1;
    if((--val) < 0) {
       if(blockedHead){
            blockedTail->blockedNext = TCB::running;
            blockedTail = TCB::running;
        } else{
            blockedHead = blockedTail = TCB::running;
        }
        TCB *old = TCB::running;
        TCB::running = Scheduler::get();
        TCB::contextSwitch(&old->context, &TCB::running->context);
    }
    TCB::running->timerBlockedCounter = 0;
    return TCB::running->returnStatus;
}

int kSemaphore::signal() {
    if(!open || TCB::running == nullptr) return -1;
    if((++val)<=0){
        TCB *tmp= this->blockedHead;
        this->blockedHead = this->blockedHead->blockedNext;
        tmp->blockedNext = nullptr;
        if(this->blockedHead == nullptr) {
            blockedTail = nullptr;
        }
        if(tmp->timerBlocked){
            TCB *curr = TCB::headWait;
            TCB *prev = nullptr;
            while(1){
                if(curr == tmp) break;
                prev = curr;
                curr = curr->blockedNextTimer;
            }
            if(prev) prev->blockedNextTimer = curr->blockedNextTimer;
            else TCB::headWait = curr->blockedNextTimer;
            tmp->timerBlockedCounter = 0;
            tmp->blockedNextTimer = nullptr;
            tmp->timerBlocked = false;
            tmp->blockedOnSem = nullptr;
        }
        Scheduler::put(tmp);
    }
    return 0;
}



int kSemaphore::trywait() {
    if(!open || TCB::running == nullptr) return -1;
    if(val<=0) return 1;
    --val;
    return 0;
}

int kSemaphore::timedWait(time_t t) {
    if(!open || TCB::running == nullptr) return -1;
    if(--val<0){
        TCB::running->timerBlockedCounter = t;
        TCB::running->blockedOnSem = this;
        TCB::running->timerBlocked = true;
        if(blockedHead){
            blockedTail->blockedNext = TCB::running;
            blockedTail = TCB::running;
        } else{
            blockedHead = blockedTail = TCB::running;
        }
        if(TCB::headWait == nullptr){
            TCB::headWait = TCB::running;
            TCB::running->timerBlockedCounter = t;
        } else{
            TCB *tmp = TCB::headWait;
            TCB *prev = TCB::headWait;
            time_t periods = t;
            while(tmp && periods){
                if(periods<tmp->timerBlockedCounter) break;
                periods = periods-tmp->timerBlockedCounter;
                prev = tmp;
                tmp = tmp->blockedNextTimer;
            }
            if(tmp) tmp->periodsSleeping -= periods;
            if(prev) prev->blockedNextTimer = TCB::running;
            else TCB::headWait = TCB::running;
            TCB::running->blockedNextTimer = tmp;
            TCB::running->timerBlockedCounter = periods;
        }

        TCB *old = TCB::running;
        TCB::running = Scheduler::get();
        TCB::contextSwitch(&old->context, &TCB::running->context);
    }
    return TCB::running->returnStatus;
}

void kSemaphore::getWaitTimer() {
    if(TCB::headWait == nullptr) return;
    TCB::headWait->timerBlockedCounter--;
    while(TCB::headWait && TCB::headWait->timerBlockedCounter == 0){
        TCB *tmp = TCB::headWait;
        TCB::headWait = TCB::headWait->blockedNextTimer;
        tmp->returnStatus = -2;
        tmp->blockedNextTimer = nullptr;
        kSemaphore *sem = tmp->blockedOnSem;
        TCB *curr = sem->blockedHead;
        sem->val++;
        TCB *prev = nullptr;
        while(1){
            if(curr == tmp)  break;
            prev = curr;
            curr = curr->blockedNext;
        }
        if(prev) prev->blockedNext = curr->blockedNext;
        else sem->blockedHead = curr->blockedNext;
        if(!curr->blockedNext) sem->blockedTail = prev;
        tmp->timerBlocked = false;
        tmp->blockedOnSem = nullptr;
        Scheduler::put(tmp);
    }
}







