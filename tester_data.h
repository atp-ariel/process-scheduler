#include "process.h"


//Define la estructura de las estadísticas de los resultados luego de un algoritmo de scheduling.
typedef struct tester{
    //Nombre del algoritmo con que obtuvieron dichas estadísticas.
    char* methods;
    //Orden de ejecución de los procesos.
    list* executed_order;
    //Máximo Turnaround Time del scheduling.
    int max_tt;
    //Turnaround Time promedio del scheduling.
    double prom_tt;
    //Máximo Response Time del scheduling.
    int max_rt;
    //Response Time promedio del scheduling.
    double prom_rt;
    //Quantum de Tiempo
    int quantum;
    //Tiempo de reseteo del scheduling solo es válido para el MLFQ.
    int time_reset;
    //Tiempo de ejecución de los procesos en cada cola, solo MLFQ.
    list* allotments;
    //Cantidad de colas del algoritmo, solo MLFQ.
    int queues;
}tester;

//Escribe en pantalla las estadísticas de los resultados de un algoritmo y un juego de datos.
void print(tester* tester, char* name_case);
//Devuelve una nueva instancia de tester.
tester* get_new_tester(char* methods, list* executed, int max_tt, double prom_tt, int max_rt, double prom_rt, int quantum, int queues, list* allotments, int time_reset);