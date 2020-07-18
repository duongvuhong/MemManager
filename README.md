# MemManager

A memory manager

## Run test

```
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./testmm
```

## How to use

```cpp
#include "MemManager.hpp"

#define MEM_SIZE (10 * 1024)

char mem[MEM_SIZE];

MemManager mm(mem, MEM_SIZE);

...

void foo()
{
    size_t cap   = mm.capacity();
    char *buffer = (char *)mm.alloc(100);
    struct A *a  = (struct A *)mm.alloc(sizeof(struct A));

    ...

    mm.free(buffer);
    mm.free(a);
}
```
