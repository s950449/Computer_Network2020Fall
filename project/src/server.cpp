#include <iostream>
#include "httpd.hpp"
#include "signal.hpp"
int main(int argc,char** argv){
    HTTPD server;
    signal(SIGINT,SignalHandler);
    server.init_server(atoi(argv[1]));
    return 0;
}