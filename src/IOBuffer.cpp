//
// Created by os on 5/16/24.
//
#include "../h/IOBuffer.hpp"
#include "../test/printing.hpp"
void IOBuffer::put(char c) {
    spaceAvailable->wait();
    buffer[tail] = c;
    tail = (tail + 1) % size;
    itemAvailable->signal();
}

char IOBuffer::get() {
    itemAvailable->wait();
    char volatile c = buffer[head];
    head = (head + 1) % size;
    spaceAvailable->signal();
    return c;
}


