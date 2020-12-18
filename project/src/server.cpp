#include <iostream>
#include <fstream>
#include <sys/wait.h>
#include "socket.hpp"
#include "signal.hpp"
int main(int argc,char** argv){
    SOCKETD server;
    signal(SIGINT,SignalHandler);
#ifndef LOCAL
    std::string pub("/etc/letsencrypt/archive/sim2.csie.org/cert1.pem");
    std::string pri("/etc/letsencrypt/archive/sim2.csie.org/privkey1.pem");
#else
    std::string pub("/tmp/ssl/cert.pem");
    std::string pri("/tmp/ssl/key.pem");   
#endif
    server.pass_key(pub,pri);
    std::ifstream config_file("./config"); // No op now
    SOCKETD::server type;
    if(argv[2] == NULL){
        type = SOCKETD::server::OTHER;
    }
    else if(strcmp(argv[2],"http")==0){
        type = SOCKETD::server::HTTP;
    }
    else if(strcmp(argv[2],"https") == 0){
        type = SOCKETD::server::HTTPS;
    }
    else{
        type = SOCKETD::server::OTHER;
    }
    pid_t PID;
    PID=fork();

    if(PID == 0){
        server.init_server(atoi(argv[1]),type);
        _exit(0);
    }
    else{
        wait(0);
    }
    return 0;
}