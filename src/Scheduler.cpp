#include "../h/Scheduler.hpp"
#include "../h/tcb.hpp"


TCB* Scheduler::head = nullptr;
TCB* Scheduler::tail = nullptr;



TCB *Scheduler::get()
{
    if (!head) { return nullptr; }

    TCB *elem = head;
    head = head->schedulerNext;
    if (!head) { tail = 0; }
    elem->schedulerNext = nullptr;
    elem->schedulerPrev = nullptr;
    return elem;

}

void Scheduler::put(TCB *pcb)
{
    if(pcb->schedulerNext!= nullptr || pcb->schedulerPrev != nullptr) return;
    if (tail)
    {
        tail->schedulerNext = pcb;
        pcb->schedulerPrev = tail;
        tail = pcb;
    } else
    {
        head = tail = pcb;
    }
}


void Scheduler::printWaiting() {
    TCB *curr = head;
    while(curr){
        printInt((uint64)curr);
        printString("\n");
        curr = curr->schedulerNext;
    }
}
