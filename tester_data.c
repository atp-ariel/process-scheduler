#include "tester_data.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>



void print(tester* tester,char* name_case){
    if(tester == NULL)
        printf("ERROR: Null tester.\n");
    else{
        printf("\t\t  TESTER: %s\n", name_case);
        printf("==========================================================\n");
        printf(">>> Algoritmo empleado: %s\n", tester->methods);
        if(!strcmp(tester->methods, "STCF") || !strcmp(tester->methods, "RR") ||!strcmp(tester->methods, "MLFQ"))
            printf(">>>>>> Quantum de tiempo: %i\n", tester->quantum);
        if(!strcmp(tester->methods, "MLFQ"))
        {
            printf(">>>>>> Cantidad de Colas: %i\n", tester->queues);
            printf(">>>>>> Tiempo de Ejecución por cola: ");
            for (int i = 0; i < tester->allotments->size; i++){
                int t = *((int*)(get(tester->allotments, i)->data));
                printf("[%iº: %d]  ", i, t); 
            }
            printf("\n");
            printf(">>>>>> Tiempo de Reset: %i\n", tester->time_reset);       
        }
        printf("\n>>> Turnaround Time Máximo: %i\n", tester->max_tt);
        printf(">>> Turnaround Time Promedio: %lf\n", tester->prom_tt);
        printf(">>> Response Time Máximo: %i\n", tester->max_rt);
        printf(">>> Response Time Promedio: %lf\n", tester->prom_rt);
        printf("\n>>> Orden de finalización de los procesos:\n");
        for (int i = 0; i < tester->executed_order->size; i++)
        {
            process* current = (process*)(get(tester->executed_order, i)->data);
            print_process(current);
        }
        printf("==========================================================\n");
        printf("\t\t\tEND TESTER\n");
    }
}

tester* get_new_tester(char* methods, list* executed, int max_tt, double prom_tt, int max_rt, double prom_rt, int quantum,int queues, list* allotments, int time_reset){
    tester* rtester = malloc(sizeof(tester));
    
    SAFE_ALLOCATED_POINTER(rtester);
    
    rtester->methods = methods;
    rtester->executed_order = executed;
    rtester->max_tt = max_tt;
    rtester->prom_tt = prom_tt;
    rtester->max_rt = max_rt;
    rtester->prom_rt = prom_rt;
    rtester->quantum = quantum;
    rtester->queues = queues;
    rtester->allotments = allotments;
    rtester->time_reset = time_reset;


    return rtester;
}