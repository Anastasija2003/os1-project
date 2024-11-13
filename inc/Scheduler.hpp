
#ifndef PROJECT_BASE_SCHEDULER_HPP
#define PROJECT_BASE_SCHEDULER_HPP

class TCB;
#include "../test/printing.hpp"
class Scheduler
{
private:
    static TCB *head;
    static TCB* tail;
public:
    static TCB *get();

    static void put(TCB *tcb);

    static void printWaiting();

};
#endif
