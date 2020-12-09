# Proyecto SCHEDULER


## Integrantes:
Carlos Toledo Silva C-211
Ariel Alfonso Triana Pérez C-211


### ¿Cómo compilar nuestro proyecto?
```gcc list.h list.c process.h process.c tester_data.h tester_data.c algorithms.h algorithms.c __init__.c -o scheduler```

### ¿Cómo ejecutar nuestro proyecto?
```./scheduler <dirección del caso> <nombre en mayúscula del algoritmo> <param1 param2 ... paramN>```

#### Ejemplo: 

1. Tenemos un caso de prueba en un .txt llamado caso.txt cuya dirección es ./caso.txt y queremos ejecutarlo con FIFO:

```./scheduler ./caso.txt FIFO```

2. Si queremos ejecutarlo con STF:

```./scheduler ./caso.txt STF```

3. Si queremos ejecutarlo con STCF, necesitamos el quantum de tiempo, que sería el primer parámetro:
	
```./scheduler ./caso.txt STCF 5```
En este caso el quantum es 5 unidades de tiempo.

4. Si queremos ejecutarlo con RR, al igual que en STCF necesitamos el quantum de tiempo quedando

```./scheduler ./caso.txt RR 5```

5. Si queremos ejecutarlo con MLFQ, necesitamos el quantum, el tiempo de reseteo, la cantidad de colas y las restricciones de tiempo para cada cola, pasaremos los argumentos en ese orden.

```./scheduler ./caso.txt MLFQ 5 20 3 10 20 30```

### ¿Qué formato debe tener los casos?
En cada línea debe estar uno y solo un proceso, con el siguiente formato:

```<tiempo de arribo> <duración> <<tiempo de interrupcion1> <duración1>> ..... <tiempo de interrupcionN> <duraciónN>>```

### NOTA ACLARATORIA: 
Asumimos que si tenemos un proceso que realiza interrupciones de entrada y salida el tiempo que duran las mismas está incluido en el tiempo de duración del proceso.

