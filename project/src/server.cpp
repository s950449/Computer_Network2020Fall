#include <iostream>
#include <fstream>
#include <sys/wait.h>
#include "httpd.hpp"
#include "signal.hpp"
#define PORTNUM 8080
int main(int argc,char** argv){
    HTTPD server;
    signal(SIGINT,SignalHandler);
    std::ifstream config_file("./config");
    int portnum = PORTNUM;
    if(argc >= 2){
        portnum = atoi(argv[1]);
    }
    pid_t PID;
    PID=fork();
    if(PID == 0){
        server.init_server(portnum);
        _exit(0);
    }
    else{
        wait(0);
    }
    return 0;
}