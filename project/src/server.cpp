#include <iostream>
#include <fstream>
#include <sys/wait.h>
#include "httpd.hpp"
#include "signal.hpp"
int main(int argc,char** argv){
    HTTPD server;
    signal(SIGINT,SignalHandler);
#ifndef LOCAL
    std::string pub("/etc/letsencrypt/archive/sim2.csie.org/cert1.pem");
    std::string pri("/etc/letsencrypt/archive/sim2.csie.org/privkey1.pem");
#else
    std::string pub("/tmp/ssl/cert.pem");
    std::string pri("/tmp/ssl/key.pem");   
#endif
    server.init_key(pub,pri);
    std::ifstream config_file("./config");
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