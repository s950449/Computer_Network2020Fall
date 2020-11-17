#include <iostream>
#include "httpd.hpp"
int main(int argc,char** argv){
    HTTPD server;
    server.init_server(atoi(argv[1]));
    return 0;
}