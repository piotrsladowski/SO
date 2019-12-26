#ifndef SHARED_MEMORY_SEMAPHORE_H
#define SHARED_MEMORY_SEMAPHORE_H

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>


void V(int semID, int semNum){
    struct sembuf sem_operation = {semNum, 1, SEM_UNDO | IPC_NOWAIT};
    if(semop(semID, &sem_operation, 1) == -1){
        printf("ERROR: Can't unlock a semaphore\n");
        exit(1);
    }
    else{
        printf("Semaphore successfully unlocked\n");
    }
}

void P(int semID, int semNum){
    struct sembuf sem_operation = {semNum, -1, SEM_UNDO | IPC_NOWAIT};
    if(semop(semID, &sem_operation, 1) == -1){
        printf("ERROR: Can't lock semaphore\n");
        exit(1);
    }
    else{
        printf("Semaphore successfully locked\n");
    }
}


#endif //SHARED_MEMORY_SEMAPHORE_H
