#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

void sig_handler(int sig){
    if(sig == SIGINT){
        printf("Recived INT signal\n");
        exit(7);
    }
    if(sig == SIGTERM){
        printf("Recived TERM signal\n");
        exit(4);
    }
}

int main(){
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    printf("New1.out: test\n");
    sleep(20);

    return 0;
}