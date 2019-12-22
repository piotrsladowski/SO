#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
    //__pid_t gf = getpid();
    //int exitcode = 54;
    printf("Main process PID: %i \n\n", getpid());

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

    char waitReturn_1[4]; 
    char waitReturn_2[4]; 
    int signal_1 = 0;
    int signal_2 = 0;
    int childPID_1 = wait(&signal_1);
    if(childPID_1 != 1){
        int number_1 = signal_1;
        for(int i=0; i < 4; i++){
            int tmp = number_1 % 16;
            waitReturn_1[i] = tmp;
            number_1 = number_1/16;
        }
        printf("\nChild process ended\n");
        printf("childPID: %i \n", childPID_1);
        printf("End code: %i \n", waitReturn_1[2] + waitReturn_1[3]*16);
        printf("Signal number: %i \n", waitReturn_1[0] + waitReturn_1[1]*16);
    }
    int childPID_2 = wait(&signal_2);
    if(childPID_2 != 1){
        int number = signal_2;
        for(int i=0; i < 4; i++){
            int tmp = number % 16;
            waitReturn_2[i] = tmp;
            number = number/16;
        }
        printf("\nChild process ended\n");
        printf("childPID: %i \n", childPID_2);
        printf("End code: %i \n", waitReturn_2[2] + waitReturn_2[3]*16);
        printf("Signal number: %i \n", waitReturn_2[0] + waitReturn_2[1]*16);
    }
}