#include <cstring>
#include <cstdio>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <netinet/in.h>
#include <iostream>
#include "status.hpp"
#include "handler.hpp"
#define SOCKET_FAILURE 7
#define BIND_FAILURE 8
#define LISTEN_FAILURE 9
#define ACCEPT_FAILURE 10
const char* hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
class HTTPD{
    private:
        int server_fd;
        int new_socket;
        int addr_len;
        struct sockaddr_in address;
        int createSocket();
        int bindSocket();
        int listenSocket();
        void continuousServer();
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
void HTTPD::continuousServer(){
#ifdef DEBUG
    std::cerr<<"Waiting for connection\n";
#endif
    while(1){
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addr_len);
        if (new_socket <0)
        {
            std::cerr<<"Failed to accept\n";
            exit(ACCEPT_FAILURE);
        }
        char buf[BUFSIZ*5]={0};
        read(new_socket,buf,BUFSIZ*5);
        std::string msg(buf);
#ifdef DEBUG
        std::cerr<<buf<<'\n';
        std::cerr<<buf<<'\n';
#endif
        write(new_socket , hello, strlen(hello));
        close(new_socket);
    }
}
void HTTPD::init_server(unsigned short port){
    addr_len = sizeof(address);
    if(createSocket()!=0){
        return;
    }
    std::cerr<<server_fd<<'\n';
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    if(bindSocket()!=0){
        return;
    }
    if(listenSocket()!=0){
        return;
    }
    continuousServer();
    return;
}