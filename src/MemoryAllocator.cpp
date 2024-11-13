#include "../h/MemoryAllocator.hpp"
#include "../test/printing.hpp"
FreeMem* MemoryAllocator::free = nullptr;
bool MemoryAllocator::made = false;

void MemoryAllocator::init() {
    if(!made){
        free = (FreeMem*)((size_t)HEAP_START_ADDR + MEM_BLOCK_SIZE-((size_t)HEAP_START_ADDR)%MEM_BLOCK_SIZE);
        free->next = nullptr;
        free->prev = nullptr;
        free->used = false;
        free->size = ((char*)HEAP_END_ADDR - (char*)free - MEM_BLOCK_SIZE);
        made = true;
    }
}

void* MemoryAllocator::mem_alloc(size_t sz) {
    init();
    size_t wanted = sz*MEM_BLOCK_SIZE;
    if(wanted <= 0) return nullptr;

    FreeMem *curr = MemoryAllocator::free;
    for(;curr!= nullptr;curr = curr->next){
        if(curr->size>=wanted) break;
    }

    if(!curr) return nullptr;

    size_t remaining = curr->size-wanted;
    if(remaining>=MEM_BLOCK_SIZE){
        curr->size = wanted;
        size_t offset = MEM_BLOCK_SIZE + wanted;
        FreeMem* newBlk = (FreeMem*)((char*)curr+offset);
        if(curr != free) curr->prev->next = newBlk;
        else free = newBlk;
        newBlk->prev = curr->prev;
        newBlk->next = curr->next;
        newBlk->size = remaining - MEM_BLOCK_SIZE;
        if(curr->next!= nullptr) curr->next->prev = newBlk;
    }else{
        if(curr != free) curr->prev->next = curr->next;
        else free = curr->next;
        if(curr->next != nullptr) curr->next->prev = curr->prev;
    }
    curr->next = nullptr;
    curr->prev = nullptr;
    curr->used = true;
    return (char*)curr+MEM_BLOCK_SIZE;
    /*void *start = nullptr;
    while(curr){
        if(curr->size >= wanted){
            start = (void*)((char*)curr+MEM_BLOCK_SIZE);
            curr->used = true;
            if(!curr->prev){
                if(curr->size==wanted){
                    free = curr->next;
                    curr->size = wanted;
                    break;
                }
                size_t sz = free->size;
                free = (FreeMem*)((size_t)curr + wanted + MEM_BLOCK_SIZE);
                free->size = sz - (wanted + MEM_BLOCK_SIZE);
                free->prev = nullptr;
                free->next = curr->next;
                if(curr->next) curr->next->prev = free;
                free->used = false;
            } else{
                if(curr->size == wanted){
                    curr->prev->next = curr->next;
                    if(curr->next) curr->next->prev = curr->prev;
                } else {
                    FreeMem *newBlock = (FreeMem*)((size_t)curr + wanted + MEM_BLOCK_SIZE);
                    newBlock->size = curr->size - (wanted + MEM_BLOCK_SIZE);
                    newBlock->used = false;
                    newBlock->next = curr->next;
                    newBlock->prev = curr->prev;
                    curr->prev->next = newBlock;
                    if(curr->next) curr->next->prev = newBlock;
                }
            }
            curr->next = nullptr;
            curr->prev = nullptr;
            curr->size = wanted;
            break;
        }
        curr = curr->next;
    }
    return start;*/
}

int MemoryAllocator::mem_free(void *adrr) {
    if(adrr<HEAP_START_ADDR || adrr>=HEAP_END_ADDR || adrr == nullptr){
        return -1;
    }
    FreeMem *block = nullptr;
    if(!free || adrr<(char*)free){
        block = nullptr;
    }else{
        for(block = free;block->next;block = block->next){
            if(adrr<=(char *)(block->next)) break;
        }
    }
    FreeMem *newBlock = (FreeMem*)((size_t)adrr - MEM_BLOCK_SIZE);
    newBlock->prev = block;
    newBlock->used = false;
    if(block) newBlock->next = block->next;
    else newBlock->next = free;
    if(newBlock->next) newBlock->next->prev = newBlock;
    if(block) block->next = newBlock;
    else free = newBlock;

    join(newBlock);
    join(block);
    return 0;
}

bool MemoryAllocator::join(FreeMem *block) {
    if(!block) return false;
    if(block->next && (uint64)((char*)block + block->size) + MEM_BLOCK_SIZE == (uint64)(block->next)){
        block->size += block->next->size + MEM_BLOCK_SIZE;
        block->next = block->next->next;
        if(block->next) block->next->prev = block;
        return true;
    }
    return false;
}

int MemoryAllocator::printMemory() {
    FreeMem *curr = free;
    while(curr){
        printInt((uint64)curr);
        curr = curr->next;
        putc('\n');
    }
    return 0;
}

uint64 MemoryAllocator::mem_in_blocks(size_t size) {
    size_t volatile sz = size;
    if(sz%MEM_BLOCK_SIZE != 0){
        sz = sz + MEM_BLOCK_SIZE - sz%MEM_BLOCK_SIZE;
    }
    sz = sz / MEM_BLOCK_SIZE;
    return sz;
}
