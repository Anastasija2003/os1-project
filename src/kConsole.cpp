#include "../h/kConsole.hpp"


IOBuffer* kConsole::input = nullptr;
IOBuffer* kConsole::output = nullptr;

char kConsole::getc() {
    //flush input
    return input->get();
}

void kConsole::putc(char c) {
    output->put(c);
    //flush output
}

void  kConsole::init() {
    input =  new IOBuffer(256);
    output = new IOBuffer(256);
}
