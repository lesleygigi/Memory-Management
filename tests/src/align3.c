/* many odd sized allocations checked for alignment */
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "mem.h"

int main() {
    assert(Mem_Init(4096) == 0);
    void* ptr[9];
    ptr[0] = Mem_Alloc(1, M_FIRSTFIT);
    ptr[1] = (Mem_Alloc(5, M_FIRSTFIT));
    ptr[2] = (Mem_Alloc(14, M_FIRSTFIT));
    ptr[3] = (Mem_Alloc(8, M_FIRSTFIT));
    ptr[4] = (Mem_Alloc(1, M_FIRSTFIT));
    ptr[5] = (Mem_Alloc(4, M_FIRSTFIT));
    ptr[6] = (Mem_Alloc(9, M_FIRSTFIT));
    ptr[7] = (Mem_Alloc(33, M_FIRSTFIT));
    ptr[8] = (Mem_Alloc(55, M_FIRSTFIT));

    assert((uintptr_t)(ptr[0]) % 8 == 0);
    assert((uintptr_t)(ptr[1]) % 8 == 0);
    assert((uintptr_t)(ptr[2]) % 8 == 0);
    assert((uintptr_t)(ptr[3]) % 8 == 0);
    assert((uintptr_t)(ptr[4]) % 8 == 0);
    assert((uintptr_t)(ptr[5]) % 8 == 0);
    assert((uintptr_t)(ptr[6]) % 8 == 0);
    assert((uintptr_t)(ptr[7]) % 8 == 0);
    assert((uintptr_t)(ptr[8]) % 8 == 0);

    exit(0);
}
