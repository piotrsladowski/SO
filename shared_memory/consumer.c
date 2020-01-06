#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "semaphore.h"

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

int readFromMemory(){
    shm_ptr_char = (char *) shmat(shmID, NULL, 0);
    if ((int) shm_ptr_char == -1) {
        printf("*** shmat error (client) ***\n");
        exit(1);
    }

    // Lock a semaphore
    P(semID, 0);

    printf( "\nProducer entered: ");
    puts(shm_ptr_char );

    shm_ptr_int = shm_ptr_char;

    for(int i=4; i<7; i++){
        sleep(1);
        printf("%i, ", shm_ptr_int[i]);
        printf("Adres pod ktorym lezy shm_ptr_int: %ld \n", &shm_ptr_int);
        printf("Adres: %ld \n", &shm_ptr_int[i]);
    }

    // Unlock a semaphore
    V(semID, 0);

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
    else {
        printf("Successfully connected to semaphore\n");
    }
    printf("Semaphore ID: %i \n", semID);

    //SHARED MEMORY
    shmID = shmget(mem_key, mem_size, IPC_CREAT | 0666);
    if(shmID < 0){
        printf("shmget error\n");
    }
    printf("Shared memory ID: %i \n", shmID);

    info = 0;
    //END SHARED MEMORY

    // Wait until producer made an action on shared memory
    sleep(2);
    while(semctl(semID, 0, GETVAL) == 0){
        printf("Waiting for producer\n");
        sleep(1);
    }
    while (readFromMemory() == 1){
        while(semctl(semID, 0, GETVAL) == 0){
            printf("Waiting for producer\n");
            sleep(1);
        }
/*        if(shm_ptr_int[7] != info) {
            info = shm_ptr_int[7];
            printf("New round\n");
            sleep(2);
        }
        else{
            printf("Producer hasn't entered new values\n");
            break;
        }*/

    }

    return 0;
}