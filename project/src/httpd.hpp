#include <cstring>
#include <cstdio>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <netinet/in.h>
#include <iostream>
#define SOCKET_FAILURE 7
#define BIND_FAILURE 8
#define LISTEN_FAILURE 9
class HTTPD{
    private:
        int server_fd;
        struct sockaddr_in address;
        int createSocket();
        int bindSocket();
        int listenSocket();
    public:
        HTTPD(){

        };
        void init_server(unsigned short port);
};
int HTTPD::createSocket(){
    server_fd =  socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == 0){
        std::cerr<<"Failed to create socket\n";
        exit(SOCKET_FAILURE);
    }
    return 0;    
}
int HTTPD::bindSocket(){
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        std::cerr<<"Failed to bind\n";
        exit(BIND_FAILURE);
    }     
    return 0;
}
int HTTPD::listenSocket(){
    if(listen(server_fd,10) < 0){
        std::cerr<<"Failed to listen\n";
        exit(LISTEN_FAILURE);
    }
    return 0;
}
void HTTPD::init_server(unsigned short port){
    int addr_len = sizeof(address);
    if(createSocket()!=0){
        return;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = port;
    if(bindSocket()!=0){
        return;
    }
    if(listenSocket()!=0){
        return;
    }
}