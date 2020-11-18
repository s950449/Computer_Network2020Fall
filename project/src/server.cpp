#include <iostream>
#include <sys/wait.h>
#include "httpd.hpp"
#include "signal.hpp"
int main(int argc,char** argv){
    HTTPD server;
    signal(SIGINT,SignalHandler);
    pid_t PID;
    PID=fork();
    if(PID == 0){
        server.init_server(atoi(argv[1]));
        _exit(0);
    }
    else{
        wait(0);
    }
    return 0;
}