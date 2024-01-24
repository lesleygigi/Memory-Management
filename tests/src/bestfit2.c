/* use best fit free space for allocation */
#include <assert.h>
#include <stdlib.h>

#include "mem.h"

int main() {
    assert(Mem_Init(4096) == 0);
    void *ptr[4];
    void *first, *best, *worst;

    assert(Mem_Alloc(8, M_FIRSTFIT) != NULL);
    ptr[0] = Mem_Alloc(40, M_FIRSTFIT);
    assert(Mem_Alloc(8, M_FIRSTFIT) != NULL);
    ptr[1] = Mem_Alloc(56, M_FIRSTFIT);
    assert(Mem_Alloc(8, M_FIRSTFIT) != NULL);
    first = Mem_Alloc(256, M_FIRSTFIT);
    assert(Mem_Alloc(8, M_FIRSTFIT) != NULL);
    best = Mem_Alloc(128, M_FIRSTFIT);
    assert(Mem_Alloc(8, M_FIRSTFIT) != NULL);
    ptr[2] = Mem_Alloc(32, M_FIRSTFIT);
    assert(Mem_Alloc(8, M_FIRSTFIT) != NULL);
    worst = Mem_Alloc(512, M_FIRSTFIT);
    assert(Mem_Alloc(8, M_FIRSTFIT) != NULL);
    ptr[3] = Mem_Alloc(32, M_FIRSTFIT);

    while (Mem_Alloc(128, M_FIRSTFIT) != NULL)
        ;
    assert(m_error == E_NO_SPACE);

    assert(Mem_Free(ptr[2]) == 0);
    assert(Mem_Free(ptr[3]) == 0);
    assert(Mem_Free(first) == 0);
    assert(Mem_Free(best) == 0);
    assert(Mem_Free(ptr[1]) == 0);
    assert(Mem_Free(worst) == 0);
    assert(Mem_Free(ptr[0]) == 0);

    assert(Mem_Alloc(128, M_BESTFIT) == best);

    exit(0);
}
