#pragma once

#include <cstdlib>

typedef long Align;

class MemManager {
private:
    union _Header {
        struct {
            union _Header *next;
            size_t size;
        } s;

        Align x;
    };

    typedef union _Header Header;

    void *_mem;
    size_t _size;

    Header *_freeList;
    size_t _remain;

    void *bestFit(size_t nunits);

public:
    MemManager(void *mem, size_t size);

    void *alloc(size_t size);
    void free(void *ptr);

    size_t capacity();
};
