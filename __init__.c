#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<fcntl.h>
#include<string.h>
#include"algorithms.h"

void Run(char* algorithm, int quantum, int timeToReset,int queue,list *allotment,int count,char** arraybuf, char* test_case);
list* GetProcess(int count,char** arraybuf);
int main(int argc,char** argv)
{
    #pragma region
    int indexCol = 0;
    int maxSizeCol = 64;
    int indexRow = 0;
    int maxSizeRow = 64;
    char *buf = calloc(maxSizeCol, sizeof(char));
    char letter;
    char **arraybuf = calloc(maxSizeRow, sizeof(char *));
    FILE *stream = fopen(argv[1], "r");
    while (fread(&letter, sizeof(char), 1, stream) > 0)
    {
        buf = calloc(maxSizeCol, sizeof(char));
        arraybuf[indexRow++] = buf;
        buf[indexCol++] = letter;
        while (fread(&letter, sizeof(char), 1, stream) > 0 && letter != '\n')
        {
            buf[indexCol++] = letter;
            if (indexCol == maxSizeCol - 1)
            {
                maxSizeCol *= 2;
                if (!(buf = realloc(buf, sizeof(char)*maxSizeCol)))
                {
                    printf("Error while allocating memory\n");
                    exit(1);
                }
            }
        }
        if (indexRow == maxSizeRow - 1)
        {
            maxSizeRow *= 2;
            if (!(arraybuf = realloc(arraybuf, sizeof(char*)*maxSizeRow)))
            {
                printf("Error while allocating memory\n");
                exit(1);
            }
        }
        buf[indexCol] = '\0';
        maxSizeCol = 64;
        indexCol = 0;
    }
    #pragma endregion
    char *algorithm=argv[2];
    int quantum;
    int timeToReset;
    int queues;
    list* allotments=init();
    int all[argc - 6];

    if(!strcmp(algorithm,"STCF")|| !strcmp(algorithm,"RR")||!strcmp(algorithm,"MLFQ"))
        quantum=atoi(argv[3]);
    if(!strcmp(algorithm,"MLFQ"))
    {
        timeToReset=atoi(argv[4]);
        queues=atoi(argv[5]);

        for (size_t i = 6; i<argc; i++)
        {
            int number=atoi(argv[i]);
            all[i-6] = number;
            append(allotments,&all[i-6]);
        }
    }
    Run(algorithm,quantum,timeToReset,queues,allotments,indexRow,arraybuf,argv[1]);
    return 0;
}
void Run(char *algorithm, int quantum, int timeToReset, int queue, list *allotments,int count, char **arraybuf,char* test_case)
{
    list* proclist=GetProcess(count,arraybuf);
    
    tester* result=malloc(sizeof(tester));
    if(!strcmp(algorithm,"FIFO"))
        result=fifo(proclist);
    else if(!strcmp(algorithm,"STF"))
        result=stf(proclist);
    else if(!strcmp(algorithm,"STCF"))
        result=stcf(proclist,quantum);
    else if(!strcmp(algorithm,"RR"))
        result=round_robin(proclist,quantum);
    else if(!strcmp(algorithm, "MLFQ"))
        result=mlfq(proclist, quantum, queue, allotments, timeToReset);
    print(result,test_case);
}
list* GetProcess(int count,char** arraybuf)
{
    list* proclist=calloc(count,sizeof(process));
    for (size_t i = 0; i < count; i++)
    {  
       list* numbers=init();
       char* first=arraybuf[i];
       int cant=0;
       for (size_t k = 0; arraybuf[i][k]; k++)
          if(arraybuf[i][k]==' ')
            cant++;
       int saveNumbers[cant+1];
       int j = 0;
       int k=0;
       cant=0;
       while(1)
       {
           if(arraybuf[i][j]!=' ' && arraybuf[i][j]!='\0')
                cant++;
           else
           {
               char *copy = calloc(cant, sizeof(char));
               strncpy(copy,first,cant);
               saveNumbers[k]=atoi(copy);
               append(numbers,&saveNumbers[k]);
               k++;
               cant=0;
               first = &arraybuf[i][j + 1];
           }
           if(arraybuf[i][j]=='\0')
               break;
           j++;
       }
       node* nodo0=get(numbers,0);
       int* timeArrival=nodo0->data;
       node* nodo1=get(numbers,1);
       int* timeDelay=nodo1->data;
       list* inputOutput=calloc(numbers->size-2,sizeof(int));
       for (size_t j = 2; j < numbers->size; j+=2)
       {
           int* ioTimeArrival=get(numbers,j)->data;
           int* ioTimeDelay=get(numbers,j+1)->data;
           i_o_struct* actual=get_io(*ioTimeArrival,*ioTimeDelay);
           append(inputOutput,actual);
       }
       process* new=get_new_process(*timeArrival,i,*timeDelay,inputOutput);
       append(proclist,new);
    }
    return proclist;
}
