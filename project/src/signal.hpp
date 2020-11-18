#include <csignal>
#include <iostream>
#include <unistd.h>
void SignalHandler(int signum){
    std::cerr<<"In Process:"<<getpid()<<'\n';
    std::cerr<<"Interrupt signal( "<<signum<<" ) Received\n";
    exit(signum);
}