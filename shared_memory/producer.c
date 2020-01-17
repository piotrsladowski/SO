#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>

#include "semaphore.h"

key_t mem_key;
key_t sem_key;
size_t mem_size;
int shmID;
int *shm_ptr_int;
char *shm_ptr_char;
int info;

int semID;
char choice;

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short int* array;
    struct seminfo *__buf;
} arg;

void initSharedMemory(){
    shm_ptr_char = (char *) shmat(shmID, NULL, 0);
    shm_ptr_int = (int *) shm_ptr_char;
    shm_ptr_int[7] = 0;
    if ((int) shm_ptr_int == -1) {
        printf("*** shmat error (server) ***\n");
        exit(1);
    }
}

int writeToMemory(){
    // Lock a semaphore
    if(P(semID, 0) != 0){
        return 1;
    }

    printf("Enter new values? [y/n]: ");
    choice = getchar() ;
    getchar(); // catch new line char
    if(choice != 'y'){
        if(V(semID, 0) != 0){
            return 1;
        }
        return 1;
    }

    // write char array to shared memory
    printf( "Enter a new value (max 15 characters): ");
    char c;
    int n=0;
    while ((c = getchar()) != '\n' && c != EOF) { // flush input stream
        if(n < 15){ // get only 15 chars
            shm_ptr_char[n] = c;
            n++;
        }

    }
    shm_ptr_char[n] = '\0'; //add terminating 0 at the end
    printf( "\nYou entered: %s \n", shm_ptr_char);

    // write ints to shared memory
    for(int i=4; i<7; i++){
        shm_ptr_int[i] = i;
        printf("%i, ", shm_ptr_int[i]);
    }

    // Flip control value
    if(shm_ptr_int[7] == 0)
        shm_ptr_int[7] = 1;
    else if(shm_ptr_int[7] == 1)
        shm_ptr_int[7] = 0;
    printf("Control value: %i\n", shm_ptr_int[7]);
    // Unlock a semaphore
    if(V(semID, 0) != 0){
        return 1;
    }

    return 0;
}

int main(){
    mem_key = ftok(".", 90);
    sem_key = ftok(".", 91);
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

    initSharedMemory();

    while(writeToMemory() == 0) {
        // Wait until consumer made an action on shared memory
        sleep(2);
        while (semctl(semID, 0, GETVAL) == 0) {
            printf("Waiting for customer\n");
            sleep(1);
        }
    }
    printf("\n");

    // Detach shared memory
    if(shmdt(shm_ptr_char) == 0){
        printf("Successfully detached shared memory\n");
    } else{
        exit(1);
    }

    // Remove shared memory
    if(shmctl(shmID, IPC_RMID, 0) == 0){
        printf("Successfully removed shared memory\n");
    } else{
        printf("Error during removing shared memory.\n");
        exit(1);
    }
    return 0;
}