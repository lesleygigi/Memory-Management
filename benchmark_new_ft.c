/* use first free space for allocation */
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "mem.h"

int main() {
    freopen("out_new.txt","w",stdout);
    struct timeval start_time, end_time;
    struct timeval new_start_time, new_end_time;
    int num_allocations = 1000; // 测试分配次数
    void* ptr[2*num_allocations];
    double elapsed_time ;
    Mem_Init(409600000);
    //Worst Case
    printf("-----Worst Case-----\n");

    gettimeofday(&start_time, NULL);
    for (int i = 0; i < num_allocations; i++) {
        ptr[i] = Mem_Alloc(100*i, M_FIRSTFIT);
    }
    gettimeofday(&end_time, NULL);
    elapsed_time = (end_time.tv_sec - start_time.tv_sec) +
                         (end_time.tv_usec - start_time.tv_usec) / 1e6;
    printf("new_FT took %.6f seconds\n", elapsed_time);

    //FREE
    for(int i=0;i<num_allocations;i++)
    {
        if((i&1)==1){
            Mem_Free(ptr[i]);
        }   
    }

    //Best Case
    printf("-----Best Case-----\n");
    gettimeofday(&new_start_time, NULL);
    for (int i = num_allocations; i < 2*num_allocations; i++) {
        ptr[i] = Mem_Alloc(200*i, M_FIRSTFIT);
    }
    gettimeofday(&new_end_time, NULL);
    elapsed_time = (new_end_time.tv_sec - new_start_time.tv_sec) +
                         (new_end_time.tv_usec - new_start_time.tv_usec) / 1e6;
    printf("new_FT took %.6f seconds\n", elapsed_time);
    return 0;
}
