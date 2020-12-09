#include "process.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

process* get_new_process(int time_arrival, int id, int duration, list* i_o){
    process* new_process = (process*)malloc(sizeof(process));
    SAFE_ALLOCATED_POINTER(new_process);
    
    new_process->id = id;
    new_process->time_arrival = time_arrival;
    new_process->duration = duration;
    new_process->i_o = i_o;
    new_process->time_to_finish = duration;
    new_process->time_blocked = 0;

    return new_process;
}

void print_process(process* pprocess){
    printf("(*) proc %i:\t time_arrival= %i\t dur= %i \t i/o= [", pprocess->id, pprocess->time_arrival, pprocess->duration);
    for(int i = 0; i < pprocess->i_o->size; i++){
        i_o_struct* current = (i_o_struct*)(get(pprocess->i_o, i)->data);
        if(i == pprocess->i_o->size -1)
            printf("(%i, %i)]\n",current->time, current->duration);
        else
            printf("(%i, %i) ",current->time, current->duration);
    }
    if(!pprocess->i_o->size)
        printf("]\n");
}

i_o_struct* get_io(int time, int duration){
    i_o_struct* io = malloc(sizeof(i_o_struct*));
    SAFE_ALLOCATED_POINTER(io);
    
    io->time = time;
    io->duration = duration;

    return io;
}