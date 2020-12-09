#include "algorithms.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#pragma region  FIFO
tester* fifo(list* proc_list){
    if(proc_list == NULL){
        printf("ERROR: Null process list.");
        return NULL;
    }
    if(is_empty(proc_list))
        return get_new_tester("FIFO", init(), 0, 0, 0, 0,0,0,init(),0);
    
    //Lista de los procesos que se ha ejecutado.
    list* executed = init();

    //Variables de TT y RT
    int max_turnaround_time = -1;
    int sum_turnaround_time = 0;
    int max_response_time = -1;
    int sum_response_time = 0;

    //Tiempo
    int time = 0;

    for(int i = 0; i < proc_list->size; i++)
    {
        //Proceso ha ejecutar
        process* current = (process*)(get(proc_list, i)->data);
        if(current == NULL){
            printf("ERROR: Null process.\n");
            return NULL;
        }
        int temp_tt, temp_rt;

        //Añade el proceso a los ejecutados.
        append(executed, current);

        //Si el tiempo es menor que el tiempo de arribo del proceso a ejecutar adelanta el tiempo.
        if(time < current->time_arrival)
            time += current->time_arrival - time;

        // actualiza el turnaround time de este proceso.
        temp_tt = time + current->duration - current->time_arrival;
        
        //actualiza el maximo turnaround time y la suma de los turnaround time.
        if(max_turnaround_time < temp_tt)
            max_turnaround_time = temp_tt;
        sum_turnaround_time += temp_tt;

        //actualiza el response time de este proceso.
        temp_rt = time - current->time_arrival;

        //actualiza el maximo response time y la suma de los response time.
        if(max_response_time < temp_rt)
            max_response_time = temp_rt;
        sum_response_time += temp_rt;

        //Actualiza el tiempo con el tiempo de ejecución del proceso.
        time += current->duration ;
    }

    //Calcula el promedio de las metricas.
    double prom_tt = (double)sum_turnaround_time / (double)proc_list->size;
    double prom_rt = (double)sum_response_time / (double)proc_list->size;

    tester* tester = get_new_tester("FIFO", executed, max_turnaround_time, prom_tt, max_response_time, prom_rt,0,0,init(), 0);
    return tester;
}
#pragma endregion

#pragma region STF
//Compara dos procesos según su duración. Recibe dos punteros a void.
int ascending_duration(void* a, void*b){
    if(((process*)a)->duration > ((process*)b)->duration)
        return 1;
    if(((process*)a)->duration < ((process*)b)->duration)
        return -1;
    return 0;
}
//Ordena una el intervalo de una lista especificado por [beg, end], utilizando el comparador definido en compare y el método de ordenación por Burbuja.
int bubble_sort(list* list,unsigned int beg,unsigned int end, int (*compare)(void* a, void* b)){
    if(beg >= list->size)
    {
        printf("ERROR: Index out of range.\n");
        return -1;
    }
    if(end >= list->size)
    {   
        printf("ERROR: Index out of range.\n");
        return -2;
    }
    if(beg > end){
        printf("ERROR: Bad interval (beg > end).\n");
        return -3;
    }
    int isOrder = 0;
    int i, j;
    while(!isOrder){

        isOrder = 1;
        i = beg;
        j = end;
        while(i < j){

            node* ai = get(list, i);
            node* aj = get(list, i + 1);
            int result = (*compare)(ai->data, aj->data);
            
            if(result==1){ // Esto es mierda hacer puntero a funciones.
                void* temp = ai->data;
                ai->data = aj->data;
                aj->data = temp;

                isOrder = 0;
            }
            i++;
        }
    }
    return 0;
}

tester* stf(list* proc_list){
    if(proc_list == NULL){
        printf("ERROR: Null process list\n");
        return NULL;
    }
    if(is_empty(proc_list))
        return get_new_tester("STF", init(), 0, 0, 0, 0,0,0,init(), 0);
    
    //Lista de los procesos ejecutados.
    list* executed = init();

    //Variables del TT y RT.
    int max_turnaround_time = -1;
    int sum_turnaround_time = 0;
    int max_response_time = -1;
    int sum_response_time = 0;

    //Espacio de la cola de procesos a ordenar
    int length = 0;
    //Tiempo de la simulación.
    int time = 0;

    for(int i = 0; i < proc_list->size; i++)
    {
        // actualizar time cuando no se esta ejecutando nadie.
        if(time < ((process*)get(proc_list, i)->data)->time_arrival)
            time += ((process*)get(proc_list, i)->data)->time_arrival - time;
        
        //buscar cuántos procesos que llegan después del i-ésimo tienen un tiempo menor que el time.
        for(int j = i + 1; j < proc_list->size; j++)
        {
            process* temp_c = ((process*)get(proc_list, j)->data);
            if(temp_c->time_arrival > time)
                break;
            length++;
        }
        //ordenar ese intervalo.
        if(bubble_sort(proc_list, i, i + length, ascending_duration) < 0)
            return NULL;
        
        //proceso a ejecutar.
        process* current = (process*)(get(proc_list, i)->data);
        if(current == NULL){
            printf("ERROR: Null process.\n");
            return NULL;
        }
        int temp_tt, temp_rt;

        append(executed, current);

        // actualiza el turnaround time de este proceso
        temp_tt = time + current->duration - current->time_arrival;
        
        //actualiza el maximo turnaround time y la suma de los turnaround time
        if(max_turnaround_time < temp_tt)
            max_turnaround_time = temp_tt;
        sum_turnaround_time += temp_tt;

        //actualiza el response time de este proceso
        temp_rt = time - current->time_arrival;

        //actualiza el maximo response time y la suma de los response time
        if(max_response_time < temp_rt)
            max_response_time = temp_rt;
        sum_response_time += temp_rt;

        //Actualiza el tiempo de simulacion
        time += current->duration;
        
        //Resetea el tamaño de la cola a ordenar.
        length = 0;
    }

    //Calcula el promedio de las metricas
    double prom_tt = (double)sum_turnaround_time / (double)proc_list->size;
    double prom_rt = (double)sum_response_time / (double)proc_list->size;

    tester* tester = get_new_tester("STF", executed, max_turnaround_time, prom_tt, max_response_time, prom_rt,0,0,init(), 0);
    return tester;
}
#pragma endregion STF

#pragma region STCF
//Comparador de procesos por el tiempo de finalizacion.
int ascending_finish_time(void* a, void*b){
    if(((process*)a)->time_to_finish > ((process*)b)->time_to_finish)
        return 1;
    if(((process*)a)->time_to_finish < ((process*)b)->time_to_finish)
        return -1;
    return 0;
}
//Comparador de procesos por el tiempo de bloqueo.
int ascending_blocked_time(void* a, void*b){
    if(((process*)a)->time_blocked > ((process*)b)->time_blocked)
        return 1;
    if(((process*)a)->time_blocked < ((process*)b)->time_blocked)
        return -1;
    return 0;
}

tester* stcf(list* proc_list, int quantum){
    if(proc_list == NULL){
        printf("ERROR: Null process list\n");
        return NULL;
    }
    if(is_empty(proc_list))
        return get_new_tester("STCF", init(), 0, 0, 0, 0,0,0,init(), 0);

    //Lista de los procesos completados.
    list* executed = init();
    //Lista de los procesos en cola de ejecución.
    list* execution = init();
    //Lista de los procesos bloqueados.
    list* blocked = init();


    //Variables de las metricas TT y RT
    int max_turnaround_time = -1;
    int sum_turnaround_time = 0;
    int max_response_time = -1;
    int sum_response_time = 0;

    int temp_tt, temp_rt;
    
    //Tiempo de simulacion
    int time = 0;
    //Tamaño de la cola de procesos a simular.
    int size = proc_list->size;

    //Se simula mientras en la lista de ejecutados tenga menos elementos que size
    while(executed->size < size){
        //Adelanta el tiempo mientras no tengas a quien ejecutar y todavia queden procesos por ejecutar y no ha llegado su tiempo
        while( is_empty(execution) && !is_empty(proc_list) && time < ((process*)proc_list->first->data)->time_arrival )
        {
            time++;
            //Desbloquea los procesos que terminan durante este tiempo
            if(!is_empty(blocked))
            {
                for(int i = 0; i < blocked->size; i++)
                {
                    process* temp = (process*)(get(blocked, i)->data);

                    if (temp->time_to_finish <= 0)
                    {
                        // actualiza el turnaround time de este proceso
                        temp_tt = time - temp->time_arrival;

                        //actualiza el maximo turnaround time y la suma de los turnaround time
                        if (max_turnaround_time < temp_tt)
                            max_turnaround_time = temp_tt;
                        sum_turnaround_time += temp_tt;

                        append(executed, temp);
                        popfirst(blocked);
                        i--;
                    }
                    else if(temp->time_blocked <= 1)
                    {
                        append(execution, temp);
                        popfirst(blocked);
                        i--;
                    }
                }

                for(int i = 0; i < blocked->size; i++)
                {
                    process* temp = (process*)(get(blocked, i)->data);
                
                    temp->time_to_finish--;
                    temp->time_blocked--;
                }
            }
        }
        //Toma el proceso que tiene el mismo tiempo que el actual.
        while(!is_empty(proc_list) && time == ((process*)proc_list->first->data)->time_arrival)
        {
            append(execution, proc_list->first->data);
            popfirst(proc_list);
        }

        //Si ordenar da error termina.
        if(!is_empty(execution) && bubble_sort(execution, 0, execution->size-1, ascending_finish_time)<0)
            return NULL;

        //Toma un procesos nuevo para simular durante el quantum.
        process* current = NULL;
        if(!is_empty(execution))
            current = ((process*)get(execution, 0)->data);

        //Proxima interrupcion del quantum.
        int t_quantum = time + quantum;
        //Mientras no llegue la interrupcion
        while(time < t_quantum)
        {
            //Si no tenemos a quien simular en current y existen en la lista de ejecucion tomalo.
            if(current == NULL && !is_empty(execution))
                current = ((process*)get(execution, 0)->data);
            //Toma los proceso que llegan en este instante de tiempo y añadelos a la lista de ejecucion.
            while(!is_empty(proc_list) && time == ((process*)proc_list->first->data)->time_arrival)
            {
                append(execution, proc_list->first->data);
                popfirst(proc_list);
            }
            //Si no esta vacia la lista de ejecucion current no es null y es la primera vez que lo tomamos actualiza su RT
            if(!is_empty(execution) && current != NULL){
            if(current->time_to_finish == current->duration)
            {
                //actualiza el response time de este proceso
                temp_rt = time - current->time_arrival;
                //actualiza el maximo response time y la suma de los response time
                if(max_response_time < temp_rt)
                    max_response_time = temp_rt;
                sum_response_time += temp_rt;
            }
            }

            //Revisa si termina su tiempo de bloqueo algun proceso
            for(int i = 0; i < blocked->size; i++){
                process* temp = (process*)(get(blocked, i)->data);

                if (temp->time_to_finish <= 0)
                {
                    // actualiza el turnaround time de este proceso
                    temp_tt = time - temp->time_arrival;

                    //actualiza el maximo turnaround time y la suma de los turnaround time
                    if (max_turnaround_time < temp_tt)
                        max_turnaround_time = temp_tt;
                    sum_turnaround_time += temp_tt;

                    append(executed, temp);
                    popfirst(blocked);
                    i--;
                }
                else if(temp->time_blocked <= 1)
                {
                    append(execution, temp);
                    popfirst(blocked);
                    i--;
                }
            }
            //Actualiza los tiempo de bloqueo y finalizacion de los bloqueados.
            for(int i = 0; i < blocked->size; i++){
                process* temp = (process*)(get(blocked, i)->data);
                
                temp->time_to_finish--;
                temp->time_blocked--;
            }
            //Si tiene in/out y le toca ahora hacerla bloquealo.
            if (current != NULL && current->i_o->size)
            {
                int mark = 0;
                for (int i = 0; i < current->i_o->size; i++)
                {
                    i_o_struct* temp = (i_o_struct*)(get(current->i_o, i)->data);
                    if(temp->time == current->duration - current->time_to_finish){
                        current->time_to_finish--;
                        append(blocked, current);
                        popfirst(execution);
                        current->time_blocked = temp->duration-1;
                        if(bubble_sort(blocked, 0, blocked->size-1, ascending_blocked_time)<0)
                            return NULL;
                        mark = 1;
                        break;
                    }
                }
                if(mark){
                    time++;
                    break;
                }
            }
            //Actualiza el tiempo de finalizacion del proceso en cuestion
            if (current != NULL)
                //Disminuye el tiempo y reviso si existe uno que termino en los bloqueados.
                current->time_to_finish--;
            
            //Si es tiempo de terminar la ejecucion del proceso actual actualiza su TT
            if(current != NULL && current->time_to_finish <= 0){
                append(executed, current);
                popfirst(execution);

                // actualiza el turnaround time de este proceso
                temp_tt = time - current->time_arrival;
        
                //actualiza el maximo turnaround time y la suma de los turnaround time
                if(max_turnaround_time < temp_tt)
                    max_turnaround_time = temp_tt;
                sum_turnaround_time += temp_tt;
                time++;
                break;
            }
            time++;
        }

        if(!is_empty(execution) && bubble_sort(execution, 0, execution->size-1, ascending_finish_time)<0)
           return NULL;
    }
    double prom_tt = (double)sum_turnaround_time / (double)size;
    double prom_rt = (double)sum_response_time / (double)size;

    tester* tester = get_new_tester("STCF", executed, max_turnaround_time, prom_tt, max_response_time, prom_rt,quantum,0,init(), 0);
    return tester;
}
#pragma endregion STCF

#pragma region RR
//Devuelve un indice random del tamaño de la lista que se pasa como parametro
int get_random_index(list* list){
    if(list == NULL){
        printf("ERROR: It's impossible give you an index, because list is NULL reference.\n");
        return -2;
    }
    if(is_empty(list)){
        printf("WARNING: List empty. Index -1.\n");
        return -1;
    }
    return rand() % list->size;
}
tester* round_robin(list* proc_list, int quantum)
{
    if(proc_list == NULL){
        printf("ERROR: Null process list\n");
        return NULL;
    }
    if(is_empty(proc_list))
        return get_new_tester("STCF", init(), 0, 0, 0, 0,0,0,init(), 0);

    //Lista de los procesos completados.
    list* executed = init();
    //Lista de los procesos en cola de ejecución.
    list* execution = init();
    //Lista de los procesos bloqueados.
    list* blocked = init();

    //Variables para mantener las metricas TT y RT
    int max_turnaround_time = -1;
    int sum_turnaround_time = 0;
    int max_response_time = -1;
    int sum_response_time = 0;
    int temp_tt, temp_rt;
    
    //Variables para manejar la semilla del random
    long int sec = time(NULL);
    srand(sec);

    //Tiempo de simulacion
    int timer = 0;
    //Tamaño de la lista de procesos que se recibe como parametro
    int size = proc_list->size;

    //Simularemos mientras la lista de los ejecutados tenga menos elementos que size
    while(executed->size < size)
    {
        //Adelanta el tiempo mientras no tengas a quien ejecutar y todavia queden procesos por ejecutar y no ha llegado su tiempo
        while( is_empty(execution) && !is_empty(proc_list) && timer < ((process*)proc_list->first->data)->time_arrival )
        {
            timer++;
            //Desbloquea los procesos que terminan durante este tiempo
            if(!is_empty(blocked))
            {
                for(int i = 0; i < blocked->size; i++)
                {
                    process* temp = (process*)(get(blocked, i)->data);
                    if (temp->time_to_finish <= 0)
                    {
                        // actualiza el turnaround time de este proceso
                        temp_tt = timer - temp->time_arrival;

                        //actualiza el maximo turnaround time y la suma de los turnaround time
                        if (max_turnaround_time < temp_tt)
                            max_turnaround_time = temp_tt;
                        sum_turnaround_time += temp_tt;

                        append(executed, temp);
                        popfirst(blocked);
                        i--;
                    }
                    else if(temp->time_blocked <= 1)
                    {
                        append(execution, temp);
                        popfirst(blocked);
                        i--;
                    }
                }
                for(int i = 0; i < blocked->size; i++)
                {
                    process* temp = (process*)(get(blocked, i)->data);
                
                    temp->time_to_finish--;
                    temp->time_blocked--;
                }
            }
        }
        //Toma el proceso que tiene el mismo tiempo que el actual.
        while(!is_empty(proc_list) && timer == ((process*)proc_list->first->data)->time_arrival)
        {
            append(execution, proc_list->first->data);
            popfirst(proc_list);
        }
        //Actualiza la semilla del random.
        if(sec != time(NULL)){
            sec = time(NULL);
            srand(sec);
        }

        //Actualiza el proceso a ejecutar dado el random.
        int rand_index ;
        process* current = NULL;

        if(!is_empty(execution))
        {
            rand_index =get_random_index(execution);
            current = ((process*)get(execution, rand_index)->data);
        }

        //Proxima interrupcion del quantum.
        int t_quantum = timer + quantum;
        //Mientras no llegue la interrupcion
        while(timer < t_quantum)
        {
            //Si no tenemos a quien simular en current y existen en la lista de ejecucion tomalo.
            if(current == NULL && !is_empty(execution))
                current = ((process*)get(execution, 0)->data);
            //Toma los proceso que llegan en este instante de tiempo y añadelos a la lista de ejecucion.
            while(!is_empty(proc_list) && timer == ((process*)proc_list->first->data)->time_arrival)
            {
                append(execution, proc_list->first->data);
                popfirst(proc_list);
            }
            //Si no esta vacia la lista de ejecucion current no es null y es la primera vez que lo tomamos actualiza su RT
         
            if(current != NULL && !is_empty(execution)){
                if(current->time_to_finish == current->duration)
                {
                    //actualiza el response time de este proceso
                    temp_rt = timer - current->time_arrival;
                    //actualiza el maximo response time y la suma de los response time
                    if(max_response_time < temp_rt)
                        max_response_time = temp_rt;
                    sum_response_time += temp_rt;
                }
            }
            //Revisa si termina su tiempo de bloqueo algun proceso
            for(int i = 0; i < blocked->size; i++){
                process* temp = (process*)(get(blocked, i)->data);
                if (temp->time_to_finish <= 0)
                {
                    // actualiza el turnaround time de este proceso
                    temp_tt = timer - temp->time_arrival;

                    //actualiza el maximo turnaround time y la suma de los turnaround time
                    if (max_turnaround_time < temp_tt)
                        max_turnaround_time = temp_tt;
                    sum_turnaround_time += temp_tt;

                    append(executed, temp);
                    popfirst(blocked);
                    i--;
                    
                }
                else if(temp->time_blocked <= 1)
                {
                    append(execution, temp);
                    popfirst(blocked);
                    i--;
                }
            }
            //Actualiza los tiempo de bloqueo y finalizacion de los bloqueados.
            for(int i = 0; i < blocked->size; i++){
                process* temp = (process*)(get(blocked, i)->data);
                
                temp->time_to_finish--;
                temp->time_blocked--;
            }
            //Si tiene in/out y le toca ahora hacerla bloquealo.
            if(current != NULL && current->i_o->size){
                int mark = 0;
                for (int i = 0; i < current->i_o->size; i++)
                {
                    i_o_struct* temp = (i_o_struct*)(get(current->i_o, i)->data);
                    if(temp->time == current->duration - current->time_to_finish ){
                        current->time_to_finish--;
                        append(blocked, current);
                        remove_at(execution, rand_index);
                        current->time_blocked = temp->duration-1;
                        if(bubble_sort(blocked, 0, blocked->size-1, ascending_blocked_time)<0)
                            return NULL;
                        mark = 1;
                        break;
                    }
                }
                if(mark){
                    timer++;
                    break;
                }
            }

            if (current != NULL)
                //Disminuye el tiempo y reviso si existe uno que termino en los bloqueados.
                current->time_to_finish--;
            //Si es tiempo de terminar la ejecucion del proceso actual actualiza su TT
            if(current != NULL && current->time_to_finish <= 0){
                append(executed, current);
                remove_at(execution, rand_index);
                // actualiza el turnaround time de este proceso
                temp_tt = timer - current->time_arrival;
        
                //actualiza el maximo turnaround time y la suma de los turnaround time
                if(max_turnaround_time < temp_tt)
                    max_turnaround_time = temp_tt;
                sum_turnaround_time += temp_tt;
                timer++;
                break;
            }
            timer++;
        }
    }
    double prom_tt = (double)sum_turnaround_time / (double)size;
    double prom_rt = (double)sum_response_time / (double)size;

    tester* tester = get_new_tester("RR", executed, max_turnaround_time, prom_tt, max_response_time, prom_rt,quantum,0,init(), 0);
    return tester;
}
#pragma endregion

#pragma region MLFQ
//Compara dos nodos de una lista que contiene proceso segun su tiempo de bloqueo
int ascending_list_blocked_time(void* a, void* b){
    process* pa = (process*)(((list*)a)->tail->data);
    process* pb = (process*)(((list*)b)->tail->data);

    return ascending_blocked_time(pa, pb);
}
//Devuelve 1 si las listas estan vacias devuelve 0 si tienen alguien.
int queues_empties(list** queue, int size){
    for (size_t i = 0; i < size; i++)
        if(queue[i]->size)
            return 0;
    return 1;    
}

//Realiza la operacion de poner todos los procesos hacia la primera cola y actualiza los counts de los procesos
//Si current no es null pone el proceso en la primera posicion de la primera cola.
void reset(list** queues, int size_queues, int* counts, int counts_size, process* current){
    for(int i = 0; i < queues[0]->size; i++)
    {
        process* temp = (process*)get(queues[0],i)->data;
        counts[temp->id] = 0;
        if(current != NULL && temp->id == current->id){
                remove_at(queues[0],i);
                insert(queues[0], temp);
        }
    }
    for (int i = 1; i < size_queues; i++)
    {
            for(int j = 0; j < queues[i]->size; j++)
            {
                process* temp = (process*)get(queues[i], j)->data;
                if(current != NULL && temp->id == current->id)
                    insert(queues[0], temp);
                else
                    append(queues[0], temp);
                counts[temp->id] = 0;
            }
    }
    for(int i = 1; i < size_queues; i++){
        while(!is_empty(queues[i]))
            popfirst(queues[i]);
    }
}
tester *mlfq(list *proc_list,unsigned int quantum,unsigned int count_queues, list *allotment,unsigned int time_reset)
{
    #pragma region IFS
    if (proc_list == NULL)
    {
        printf("ERROR: Null process list.\n");
        return NULL;
    }
    if (is_empty(proc_list))
        return get_new_tester("MLFQ", init(), 0, 0, 0, 0, 0, 0, init(), 0);
    
    if(quantum == 0)
    {
        printf("ERROR: Quantum time equals 0.\n");
        return NULL;
    }
    if(count_queues == 0)
    {
        printf("ERROR: Queues equals 0.\n");
        return NULL;
    }
    if(allotment == NULL){
        printf("ERROR: Allotments is NULL.\n");
        return NULL;
    }
    if(allotment->size != count_queues){
        printf("ERROR: Queues not is equals to Allotments size.\n");
        return NULL;
    }
    if(time_reset == 0){
        printf("ERROR: Invalid time to reset");
        return NULL;
    }
    #pragma endregion IFS
    
    //Lista de los ejecutados
    list* executed = init();

    //Colas de ejecucion
    list* queues[count_queues];
    for (int i = 0; i < count_queues; i++)
        queues[i] = init();
    //Restricciones sobre las colas
    int allotments[count_queues];
    for (size_t i = 0; i < count_queues; i++)
        allotments[i] = *(int*)(get(allotment, i)->data);
    //Lista de los procesos bloqueados
    list* blocked = init();

    //Variable de las metricas TT y RT
    int max_turnaround_time = -1;
    int sum_turnaround_time = 0;
    int max_response_time = -1;
    int sum_response_time = 0;
    int temp_tt, temp_rt;
    
    //Inicializa el array counts
    int counts[proc_list->size];
    for (int i = 0; i < proc_list->size; i++)
         counts[i] = 0;

    //Variables para modificar la semilla del random
    long int sec = time(NULL);
    srand(sec);

    //Tiempo de simulacion
    int timer = 0;
    //Tamaño de la lista de procesos a simular.
    int size = proc_list->size;
    //Proxima interrupcion para resetear.
    int t_time_reset = timer + time_reset;
    //Simular mientras la lista de ejecutados tenga menos elementos que size.
    while(executed->size<size)
    {
        //Mientras las colas esten vacias y exista alguien en la lista de procesos adelanta el tiempo hasta el primer procesos
        while (queues_empties(queues, count_queues) && !is_empty(proc_list) && timer < ((process*)proc_list->first->data)->time_arrival)
        {   
            timer++;
            //Si en ese intervalo alguien se desbloquea realiza esa operacion
            if(!is_empty(blocked)){
                 for(int i = 0; i < blocked->size; i++){
                        list* tuple = (list*)(get(blocked, i)->data);
                        process* temp = (process*)tuple->tail->data;
                        if (temp->time_to_finish <= 0)
                        {
                            // actualiza el turnaround time de este proceso
                            temp_tt = timer - temp->time_arrival;

                            //actualiza el maximo turnaround time y la suma de los turnaround time
                            if (max_turnaround_time < temp_tt)
                                max_turnaround_time = temp_tt;
                            sum_turnaround_time += temp_tt;

                            append(executed, temp);
                            popfirst(blocked);
                            i--;
                        }
                        else if(temp->time_blocked <= 1)
                        {
                            
                            append(queues[*(int*)tuple->first->data], temp);
                            popfirst(blocked);
                            i--;
                        }
                        temp->time_to_finish--;
                        temp->time_blocked--;
                    }   
            }
            //Si es tiempo de resetear entonces resetea.
            if(timer == t_time_reset)
            {
                reset(queues, count_queues, counts, size, NULL);
                t_time_reset = timer + time_reset;
            }
        }
        //Toma los procesos que llegan en este instante de tiempo
        while(!is_empty(proc_list) && timer == ((process*)proc_list->first->data)->time_arrival){
            append(queues[0], ((process*)proc_list->first->data));
            popfirst(proc_list);
        }

        //Crea las variable del proceso de ejecucion, el indice de la cola y el indice para Round Robin
        process* current=NULL;
        int rand_index;
        int index_queue;
        //Toma el proceso a ejecutar.
        for (int i = 0; i < count_queues; i++)
        {
            if(!is_empty(queues[i])){

                rand_index = get_random_index(queues[i]);
                current = (process*)get(queues[i], rand_index)->data;
                if(sec != time(NULL))
                {
                    sec = time(NULL);
                    srand(sec);
                }
                index_queue = i;
                break;
            }
        }
        
        //Proxima interrupcion del quantum
        int t_quantum = timer + quantum;
        int mark = 0;
       
        while(timer < t_quantum)
        {
            //Toma el proceso que llega a la simulacion en este instante de tiempo.
            while(!is_empty(proc_list) && timer == ((process*)proc_list->first->data)->time_arrival)
            {
                append(queues[0], proc_list->first->data);
                popfirst(proc_list);
            } 
            
            //Si es la primera vez que tomamos este proceso, actualiza su RT
            if(current!= NULL && current->time_to_finish == current->duration)
            {
                //actualiza el response time de este proceso
                temp_rt = timer - current->time_arrival;
                //actualiza el maximo response time y la suma de los response time
                if(max_response_time < temp_rt)
                    max_response_time = temp_rt;
                sum_response_time += temp_rt;
            }

            //Revisa si algun bloqueado termino su tiempo de bloqueo.
            for(int i = 0; i < blocked->size; i++)
            {
                list* tuple = (list*)(get(blocked, i)->data);
                process* temp = (process*)tuple->tail->data;
                if (temp->time_to_finish <= 0)
                {
                    // actualiza el turnaround time de este proceso
                    temp_tt = timer - temp->time_arrival;

                    //actualiza el maximo turnaround time y la suma de los turnaround time
                    if (max_turnaround_time < temp_tt)
                        max_turnaround_time = temp_tt;
                    sum_turnaround_time += temp_tt;
                    
                    append(executed, temp);
                    popfirst(blocked);
                    i--;
                }
                else if(temp->time_blocked <= 1)
                {
                    append(queues[*(int*)tuple->first->data], temp);
                    popfirst(blocked);
                    i--;
                }
                temp->time_to_finish--;
                temp->time_blocked--;
            } 
            //Si tiene in/out y le toca ahora hacerla bloquealo.
            if(current != NULL && current->i_o->size){
                for (int i = 0; i < current->i_o->size; i++)
                {
                    i_o_struct* temp = (i_o_struct*)(get(current->i_o, i)->data);
                    if(temp->time == current->duration - current->time_to_finish ){
                        current->time_to_finish--;
                        list* temp_blocked = init();

                        int temp_index = index_queue;
                        append(temp_blocked, &temp_index);
                        append(temp_blocked, current);

                        append(blocked, temp_blocked);
                        remove_at(queues[index_queue], rand_index);
                        current->time_blocked = temp->duration -1;
                        if(bubble_sort(blocked, 0, blocked->size-1, ascending_list_blocked_time)<0)
                            return NULL;
                        mark = 1;
                        break;
                    }
                }
                if(mark){
                    timer++;
                    if(timer == t_time_reset)
                    {
                        reset(queues, count_queues, counts, size, current);
                        t_time_reset = timer + time_reset;
                        index_queue=0;
                        rand_index=0;
                    }
                    break;
                }
            }
            if (current != NULL){
                //Disminuye el tiempo y reviso si existe uno que termino en los bloqueados.
                current->time_to_finish--;
                counts[current->id]++;
            }
            //Si es tiempo de que el proceso en ejecucion termina actualiza su TT
            if(current != NULL && current->time_to_finish <= 0){
                append(executed, current);
                remove_at(queues[index_queue], rand_index);
                // actualiza el turnaround time de este proceso
                temp_tt = timer - current->time_arrival;
        
                //actualiza el maximo turnaround time y la suma de los turnaround time
                if(max_turnaround_time < temp_tt)
                    max_turnaround_time = temp_tt;
                sum_turnaround_time += temp_tt;
                timer++;
                if(timer == t_time_reset)
                {
                    reset(queues, count_queues, counts, size, current);
                    t_time_reset = timer + time_reset;
                    index_queue=0;
                    rand_index=0;
                    if(!is_empty(queues[0]))
                    current = ((process*)queues[0]->first->data);
                }
                break;
            }
            timer++;
            if(timer >= t_time_reset)
            {
                reset(queues, count_queues, counts, size, current);
                t_time_reset = timer + time_reset;
                index_queue=0;
                rand_index=0;
                if(!is_empty(queues[0]))
                    current = ((process*)queues[0]->first->data);
                
            }
        }
        //Revisa si es tiempo de bajar algun proceso de cola
        if(!mark && current!=NULL && counts[current->id]>=allotments[index_queue]&&index_queue<count_queues-1&&current->time_to_finish>0)
        {
            append(queues[index_queue+1],current);
            remove_at(queues[index_queue],rand_index);
        }
    }
    double prom_tt = (double)sum_turnaround_time / (double)size;
    double prom_rt = (double)sum_response_time / (double)size;

    tester* tester = get_new_tester("MLFQ", executed, max_turnaround_time, prom_tt, max_response_time, prom_rt,quantum,count_queues,allotment, time_reset);
    return tester;
}

#pragma endregion MLFQ


