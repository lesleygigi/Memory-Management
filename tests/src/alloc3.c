/* many odd sized allocations */
#include <assert.h>
#include <stdlib.h>

#include "mem.h"

int main() {
    assert(Mem_Init(4096) == 0);
    assert(Mem_Alloc(1, M_FIRSTFIT) != NULL);
    assert(Mem_Alloc(5, M_FIRSTFIT) != NULL);
    assert(Mem_Alloc(14, M_FIRSTFIT) != NULL);
    assert(Mem_Alloc(8, M_FIRSTFIT) != NULL);
    assert(Mem_Alloc(1, M_FIRSTFIT) != NULL);
    assert(Mem_Alloc(4, M_FIRSTFIT) != NULL);
    assert(Mem_Alloc(9, M_FIRSTFIT) != NULL);
    assert(Mem_Alloc(33, M_FIRSTFIT) != NULL);
    assert(Mem_Alloc(55, M_FIRSTFIT) != NULL);
    exit(0);
}
