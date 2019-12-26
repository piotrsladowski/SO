#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

key_t mem_key;
key_t sem_key;
size_t mem_size;
int shmID;
int *shm_ptr_int;
int *arr;
char *shm_ptr_char;
int info;

int semID;

struct sembuf sem_operation;

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short int* array;
    struct seminfo *__buf;
} arg;

int readFromMemory(){
    shm_ptr_char = (char *) shmat(shmID, NULL, 0);
    if ((int) shm_ptr_char == -1) {
        printf("*** shmat error (server) ***\n");
        exit(1);
    }

    // Lock a semaphore

    printf("Semaphore value: %i\n", semctl(semID, 0, GETVAL));
    sem_operation.sem_num = 0;
    sem_operation.sem_op = -1;
    sem_operation.sem_flg = SEM_UNDO | IPC_NOWAIT; // Raise error if can't do an operation

    if(semop(semID, &sem_operation, 1) == -1){
        printf("ERROR: Can't lock semaphore\n");
        exit(1);
    }
    else{
        printf("Semaphore successfully locked\n");
    }


    for(int i=0; i<1; i++){
        printf( "\nProducer entered: ");
        puts(shm_ptr_char );
    }
    shm_ptr_int = shm_ptr_char;

    for(int i=4; i<8; i++){
        sleep(1);
        printf("%i, ", shm_ptr_int[i]);
        printf("Adres pod ktorym lezy shm_ptr_int: %ld \n", &shm_ptr_int);
        printf("Adres: %ld \n", &shm_ptr_int[i]);
        //printf("%i, ", shm_ptr_int[i]);
    }

    // Unlock a semaphore

    sem_operation.sem_num = 0;
    sem_operation.sem_op = 1;
    sem_operation.sem_flg = SEM_UNDO | IPC_NOWAIT; // Raise error if can't do an operation

    //printf("Semaphore value: %i\n", semctl(semID, 0, GETVAL));

    if(semop(semID, &sem_operation, 1) == -1){
        printf("ERROR: Can't unlock a semaphore\n");
        exit(1);
    }
    else{
        printf("Semaphore successfully unlocked\n");
    }

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

    //printf("Semaphore value: %i\n", semctl(semID, 0, GETVAL));

    // Wait until consumer made an action on shared memory
    sleep(2);
    while(semctl(semID, 0, GETVAL) != 1){
        printf("Waiting for producer\n");
        sleep(1);
    }
    while (readFromMemory() == 1){
        while(semctl(semID, 0, GETVAL) != 1){
            printf("Waiting for producer\n");
            sleep(1);
        }
        if(shm_ptr_int[7] != info)
            printf("New round\n");
        else
            break;
    }

    return 0;
}