#include "list.h"

//Define la estructura de un proceso en la simulación.
typedef struct process{
    //Tiempo de llegada del proceso a la simulación.
    int time_arrival;
    //ID del proceso, es necesario que el programador mantenga este campo de forma que no exista para dos procesos un mismo ID.
    int id;
    //Duración del proceso, incluye en su tiempo los in/out.
    int duration;
    //Lista de in/out del proceso.
    list* i_o;
    //Tiempo que resta para terminar
    int time_to_finish;
    //Tiempo que debe estar bloqueado
    int time_blocked;
}process;

//Define la estructura de una in/out.
typedef struct i_o_struct{
    //Tiempo en que se comienza la in/out.
    int time;
    //Duración de la in/out.
    int duration;
}i_o_struct;

//Inicializa un proceso, dado los parámetros necesarios.
process* get_new_process(int time_arrival, int id, int duration, list* i_o);
//Escribe en pantalla las características de un proceso.
void print_process(process* process);
//Inicializa una in/out dado el tiempo y su duración.
i_o_struct* get_io(int time, int duration);