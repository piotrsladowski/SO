#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
    printf("Main process PID: %i \n\n", getpid());

    // create new processes and replace the code
    int n1 = fork();
    if(n1 == 0){
        execl("new1.out", "new1.out", (char*) NULL);
    }
    printf("Child1 PID: %i \n\n", n1);
    int n2 = fork();
    if(n2 == 0){
        execl("new2.out", "new2.out", (char*) NULL);
    }
    printf("Child2 PID: %i \n\n", n2);

    char signalAndReturnVaules_1[4]; 
    char signalAndReturnVaules_2[4]; 
    int signal_1 = 0;
    int signal_2 = 0;
    // wait for child process and get childPID and interrupt signal code
    int childPID_1 = wait(&signal_1);
    if(childPID_1 != 1){
        int number_1 = signal_1;
        for(int i=0; i < 4; i++){
            int tmp = number_1 % 16;
            signalAndReturnVaules_1[i] = tmp;
            number_1 = number_1/16;
        
        }
        printf("\nChild process ended\n");
        printf("childPID: %i \n", childPID_1);
        printf("End code: %i \n", signalAndReturnVaules_1[2] + signalAndReturnVaules_1[3]*16);
        printf("Signal number: %i \n", signalAndReturnVaules_1[0] + signalAndReturnVaules_1[1]*16);
    }
    int childPID_2 = wait(&signal_2);
    if(childPID_2 != 1){
        int number = signal_2;
        // Convert hex to decimal
        for(int i=0; i < 4; i++){
            int tmp = number % 16;
            signalAndReturnVaules_2[i] = tmp;
            number = number/16;
        }
        printf("\nChild process ended\n");
        printf("childPID: %i \n", childPID_2);
        printf("End code: %i \n", signalAndReturnVaules_2[2] + signalAndReturnVaules_2[3]*16);
        printf("Signal number: %i \n", signalAndReturnVaules_2[0] + signalAndReturnVaules_2[1]*16);
    }
}