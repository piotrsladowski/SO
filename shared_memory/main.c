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
int shmID;
int *shm_ptr_int;
int *arr;
char *shm_ptr_char;

int semID;

struct sembuf sem_operation;

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short int* array;
    struct seminfo *__buf;
} arg;

int main(){
	mem_key = ftok(".", 50);
	sem_key = ftok(".", 51);
    semID = semget(sem_key, 1, IPC_CREAT | 0600);
	if(semID == -1){
	    printf("ERROR during creating semaphore\n");
	}
	printf("%i \n", semID);

	arg.val = 1;
    if (semctl(semID, 0, SETVAL, arg) == -1)
    {
        printf("ERROR during initializing semaphore\n");
        exit(1);
    }

    shmID = shmget(mem_key, 8 * sizeof(int), IPC_CREAT | 0666);
	if(shmID < 0){
		printf("shmget error\n");
	}
	printf("ID: %i \n", shmID);


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
    }
    else{
        printf("Semaphore successfully locked\n");
    }

/*	for(int i=0; i<4; i++){
		shm_ptr_int[i] = i;
		sleep(1);
		printf("%i, ", shm_ptr_int[i]);
		printf("Adres pod ktorym lezy shm_ptr_int: %ld \n", &shm_ptr_int);
		printf("Adres: %ld \n", &shm_ptr_int[i]);
		//printf("%i, ", shm_ptr_int[i]);
	}*/


    for(int i=0; i<1; i++){
        printf( "Enter a new value (max 16 characters) :");
        fgets(shm_ptr_char, 16, stdin);
        printf( "\nYou entered: ");
        puts(shm_ptr_char );
    }

	// Unlock a semaphore

    sem_operation.sem_num = 0;
    sem_operation.sem_op = 1;
    sem_operation.sem_flg = SEM_UNDO | IPC_NOWAIT; // Raise error if can't do an operation

    //printf("Semaphore value: %i\n", semctl(semID, 0, GETVAL));

    if(semop(semID, &sem_operation, 1) == -1){
        printf("ERROR: Can't unlock a semaphore\n");
    }
    else{
        printf("Semaphore successfully unlocked\n");
    }
    //printf("Semaphore value: %i\n", semctl(semID, 0, GETVAL));

    // Wait until consumer made an action on shared memory
    sleep(2);
    printf("Enter new values: \n");
    while(semctl(semID, 0, GETVAL) != 1){
        printf("waiting\n");
        sleep(1);
    }
	return 0;
}