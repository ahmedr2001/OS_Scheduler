#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int mem_blocks[1024];
int mp_ff[1024];

void init_mem_blocks()
{
    for(int i=0;i<1024;i++){
        mem_blocks[i]=0;
    }
    mem_blocks[0]=1024;
}

void init_mp_ff()
{
    for(int i=0;i<1024;i++){
        mp_ff[i]=-1;
    }
}

int ff_allocate(int req_size, int *mem_end_idx)
{
    for(int i=0;i<1024;i++){
        if(mem_blocks[i] > 0){
            if(mem_blocks[i]>=req_size){
                int st_idx=i;
                *mem_end_idx=i+req_size-1;
                if(*mem_end_idx+1<1024)mem_blocks[*mem_end_idx+1]=mem_blocks[i]-req_size;
                mem_blocks[i]=0;
                mp_ff[st_idx]=req_size;
                return st_idx;
            }
        }
    }
    return -1;
}

void repair()
{
    int i=0;
    while(i<1024){
        if(mem_blocks[i]){
            int j=i+mem_blocks[i];
            while(j<1024 && mem_blocks[j]){
                int sz=mem_blocks[j];
                mem_blocks[i]+=mem_blocks[j];
                mem_blocks[j]=0;
                j+=sz;
            }
            i=j;
        }
        else i++;
    }
}

int ff_deallocate(int req_size, int mem_st_idx, int mem_end_idx)
{
    if(~mp_ff[mem_st_idx]){
        mp_ff[mem_st_idx]=-1;
        mem_blocks[mem_st_idx]=req_size;
        repair();
        return 0;
    }
    return -1;
}
