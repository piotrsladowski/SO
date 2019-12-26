#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

void sig_handler(int sig){
    if(sig == SIGINT){
        printf("Catched INT signal\n");
        exit(70);
    }
    if(sig == SIGTERM){
        printf("Catched TERM signal\n");
        exit(40);
    }
}

int main(){
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    printf("New2.out: test\n");
    sleep(30);
    
    return 0;
}