#include <climits>
#include <stdexcept>

#include "MemManager.hpp"

MemManager::MemManager(void *mem, size_t size) : _mem(mem),
                                                 _size(size)
{
    if (_mem == nullptr || _size == 0)
        throw std::runtime_error("Memory is invalid");

    _freeList = (Header *)_mem;
    _freeList->s.size = _remain = _size / sizeof(Header);
}

void *MemManager::bestFit(size_t nunits)
{
    Header *p = _freeList, *prevp = _freeList;
    Header *best = nullptr, *prevBest = nullptr;
    size_t remain = INT_MAX;

    while (true) {
        if (p->s.size == nunits) { /* Exactly */
            prevp->s.next = p->s.next;
            prevBest = prevp;
            best = p;
            break;
        }

        if (p->s.size > nunits && p->s.size - nunits < remain) { /* A better block */
            prevBest = prevp;
            best = p;
        }

        if (p == _freeList) /* Already wrapped around */
            break;

        prevp = p;
        p = p->s.next;
    }

    if (best != nullptr) {
        best->s.size -= nunits;
        best += p->s.size;
        best->s.size = nunits;

        _freeList = prevBest;
        _remain -= nunits;

        return (void *)(best + 1);
    }

    return nullptr;
}

void *MemManager::alloc(size_t size)
{
    if (size == 0 || size > capacity())
        return nullptr;

    return bestFit((size + sizeof(Header) - 1) / sizeof(Header) + 1);
}

void MemManager::free(void *ptr)
{
    Header *bp, *p;

    if (ptr == nullptr)
        return;

    if (ptr < _mem || ptr > ((char *)_mem + _size)) /* Out of scope */
        return;

    bp = (Header *)ptr - 1; /* Get block header */
    if (bp->s.size == 0) /* Free block is zero in size */
        return;

    _remain += bp->s.size;

    for (p = _freeList; !(bp > p && bp < p->s.next); p = p->s.next)
        if (p >= p->s.next && (bp > p || bp < p->s.next))
            break; /* Free block at start or end of arena */

    if (bp + bp->s.size == p->s.next) { /* Join to upper neighbour */
        bp->s.size += p->s.next->s.size;
        bp->s.next = p->s.next->s.next;
    } else {
        bp->s.next = p->s.next;
    }

    if (p + p->s.size == bp) { /* Join to lower neighbour */
        p->s.size += bp->s.size;
        p->s.next = bp->s.next;
    } else {
        p->s.next = bp;
    }

    _freeList = p;
}

size_t MemManager::capacity()
{
    return _remain > 1 ? (_remain - 1) * sizeof(Header) : 0;
}
