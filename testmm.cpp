/*
 * A simple unit test for Memory Manager
 * Boundary testing
 */

#include <iostream>
#include <vector>
#include <cassert>
#include <stdexcept>

#include "MemManager.hpp"

#define MEM_SIZE (10 * 1024)

static char mem[MEM_SIZE];

static MemManager *mm;

typedef void (*testFunc)();

void init()
{
    mm = new MemManager(mem, MEM_SIZE);
}

void cleanup()
{
    delete mm;
    mm = nullptr;
}

/* Test constructor: for normal case */
void testMM1()
{
    assert(mm != nullptr);
}

/* Test constructor: mem is invalid */
void testMM2()
{
    try {
        MemManager(nullptr, MEM_SIZE);
    } catch (std::runtime_error &e) {
        return;
    }

    assert(1 == 0);
}

/* Test constructor: mem size is 0 */
void testMM3()
{
    try {
        MemManager(mem, 0);
    } catch (std::runtime_error &e) {
        return;
    }

    assert(1 == 0);
}

/* Test allocator: size is 0 */
void testMM4()
{
    size_t cap = mm->capacity();
    void *ptr = mm->alloc(0);

    assert(ptr == nullptr);
    assert(mm->capacity() == cap);
}

/* Test allocator: size is a value in range from 0 - capacity */
void testMM5()
{
    size_t cap = mm->capacity();
    void *ptr = mm->alloc(100);

    assert(ptr != nullptr);
    assert(ptr >= mem && ptr < mem + MEM_SIZE);
    assert(mm->capacity() < cap - 100);
}

/* Test allocator: size is a value equaling capacity */
void testMM6()
{
    void *ptr = mm->alloc(mm->capacity());

    assert(ptr != nullptr);
    assert(ptr >= mem && ptr < mem + MEM_SIZE);
    assert(mm->capacity() == 0);
}

/* Test allocator: size is a value is over capacity */
void testMM7()
{
    size_t cap = mm->capacity();
    void *ptr = mm->alloc(mm->capacity() + 1);

    assert(ptr == nullptr);
    assert(mm->capacity() == cap);
}

/* Test allocator: many allocations */
void testMM8()
{
    void *ptr;

    for (int i = 0; i < 100; i++) {
        ptr = mm->alloc(32);
        assert(ptr != nullptr);
        assert(ptr >= mem && ptr < mem + MEM_SIZE);
    }

    assert(mm->capacity() >= 0);
}

/* Test allocator: many allocations until no memory available */
void testMM9()
{
    void *ptr;
    const size_t size = 32;

    while (mm->capacity() >= size) {
        ptr = mm->alloc(size);
        assert(ptr != nullptr);
        assert(ptr >= mem && ptr < mem + MEM_SIZE);
    }
}

/* Test allocator: call over maximum capacity */
void testMM10()
{
    void *ptr;

    for (int i = 0; i < 100; i++)
        ptr = mm->alloc(1024);

    assert(ptr == nullptr);
}

/* Test free: free nullptr */
void testMM11()
{
    size_t cap = mm->capacity();

    mm->free(nullptr);

    assert(mm->capacity() == cap);
}

/* Test free: free memory that is out of scope */
void testMM12()
{
    char ptr[10];
    size_t cap = mm->capacity();

    mm->free(ptr);

    assert(mm->capacity() == cap);
}

/* Test free: free memory that allocated by alloc() */
void testMM13()
{
    size_t cap = mm->capacity();
    void *ptr = mm->alloc(10);

    mm->free(ptr);

    assert(mm->capacity() == cap);
}

/* Test free: free many memory blocks that allocated by alloc() */
void testMM14()
{
    size_t cap = mm->capacity();
    void *ptr[10];
    
    for (int i = 0; i < 10; i++)
        ptr[i] = mm->alloc(10);

    for (int i = 0; i < 10; i++)
        mm->free(ptr[i]);

    assert(mm->capacity() == cap);
}

/* Test free: alternate allocate and free */
void testMM15()
{
    size_t cap = mm->capacity();
    void *ptr[10];
    
    for (int i = 0; i < 10; i++) {
        ptr[i] = mm->alloc(10);
        mm->free(ptr[i]);
    }

    assert(mm->capacity() == cap);
}

int main(void)
{
    testFunc testSuite[] = {
        testMM1,
        testMM2,
        testMM3,
        testMM4,
        testMM5,
        testMM6,
        testMM7,
        testMM8,
        testMM9,
        testMM10,
        testMM11,
        testMM12,
        testMM13,
        testMM14,
        testMM15
    };

    for (size_t i = 0; i < sizeof(testSuite) / sizeof(testFunc); i++) {
        init();

        testSuite[i]();
        std::cout << "testMM" << i + 1 << ": " << "Pass" << std::endl;

        cleanup();
    }

    return 0;
}
