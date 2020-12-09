#include "tester_data.h"

//Ejecuta el algorimo FIFO sobre una lista de procesos.
tester* fifo(list* proc_list);
//Ordena los procesos de la lista desde la posición beg hasta end utilizando el comparador.
int bubble_sort(list* list, unsigned int beg, unsigned int end, int (*compare)(void*a, void*b));
//Ejecuta el algoritmo STF sobre una lista de procesos.
tester* stf(list* proc_list);
//Ejecuta el algoritmo STCF sobre una lista de procesos, utilizando un quantum deseado.
tester* stcf(list* proc_list, int quantum);
//Ejecuta el algoritmo Round Robin (RR) sobre una lista de procesos, utilizando un quantum deseado.
tester* round_robin(list* proc_list, int quantum);
//Ejecuta el algoritmo MLFQ sobre una lista de procesos, utilizando un quantum deseado, una cantidad de colas, un tiempo de reinicio, y un tiempo en cada cola.
tester* mlfq(list* proc_list,unsigned int quantum,unsigned int count_queues, list* allotment,unsigned int time_reset);