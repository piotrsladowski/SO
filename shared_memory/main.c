#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>

#include "semaphore.h"
#include <stdio.h>

key_t mem_key;
key_t sem_key;
size_t mem_size;
int shmID;
int *shm_ptr_int;
char *shm_ptr_char;
int info;

int semID;

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short int* array;
    struct seminfo *__buf;
} arg;

int writeToMemory(){
    shm_ptr_char = (char *) shmat(shmID, NULL, 0);
    if ((int) shm_ptr_char == -1) {
        printf("*** shmat error (server) ***\n");
        exit(1);
    }
    // Lock a semaphore
    P(semID, 0);

    printf("Enter new values? [y/n]: ");
    int c = getchar();
    if(c != 'y'){
        V(semID, 0);
    }

    // write char array to shared memory
    printf( "Enter a new value (max 15 characters) :");
    scanf("%15s", shm_ptr_char);
    printf( "\nYou entered: %s \n", shm_ptr_char);

    shm_ptr_int = shm_ptr_char;
    // write ints to shared memory
    for(int i=4; i<7; i++){
        shm_ptr_int[i] = i;
        printf("%i, ", shm_ptr_int[i]);
    }
    // last integer informs customer that producer entered new values. It changes value between 0 and 1 on every iteration
    if(shm_ptr_int[8] == 0)
        shm_ptr_int[8] = 1;
    if(shm_ptr_int[8] == 1)
        shm_ptr_int[8] = 0;

    // Unlock a semaphore
    V(semID,0);

    return 1;
}

int main(){
    mem_key = ftok(".", 80);
    sem_key = ftok(".", 81);
    mem_size = 8 * sizeof(int); // on my platform int is occupied by 4 bytes
    semID = semget(sem_key, 1, IPC_CREAT | 0600);
    if(semID == -1){
        printf("ERROR during creating semaphore\n");
    }
    printf("SemID: %i \n", semID);

    arg.val = 1;
    if (semctl(semID, 0, SETVAL, arg) == -1)
    {
        printf("ERROR during initializing semaphore\n");
        exit(1);
    }

    shmID = shmget(mem_key, mem_size, IPC_CREAT | 0666);
    if(shmID < 0){
        printf("shmget error\n");
    }
    printf("Shared memory ID: %i \n", shmID);

    info = 0;
    while(writeToMemory() == 1) {
        //printf("Semaphore value: %i\n", semctl(semID, 0, GETVAL));
        // Wait until consumer made an action on shared memory
        //sleep(2);
        while (semctl(semID, 0, GETVAL) == 0) {
            printf("Waiting for customer\n");
            sleep(1);
        }
        //writeToMemory();
    }
    printf("\n");
    return 0;
}