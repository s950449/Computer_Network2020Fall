#include <cstring>
#include <cstdio>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <netinet/in.h>
#include <iostream>
#include "handler.hpp"
#define SOCKET_FAILURE 7
#define BIND_FAILURE 8
#define LISTEN_FAILURE 9
#define ACCEPT_FAILURE 10
#define SELECT_FAILURE 11
#define READ_FAILURE 12
const char* hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
class HTTPD{
    private:
        int server_fd;
        int new_socket;
        int addr_len;
        int max_fd;
        fd_set client_set;
        struct sockaddr_in address;
        void continuousServer();
    public:
        HTTPD(){
        };
        ~HTTPD(){
        }
        int http_init(int fd);

};

void HTTPD::continuousServer(){
        RequestHandler Incoming;
#ifdef DEBUG
    std::cerr<<"Waiting for connection\n";
#endif
    FD_ZERO(&client_set);
    FD_SET(server_fd,&client_set);
    max_fd = server_fd;
    while(1){
        int ret;
        struct timeval tv;
        fd_set read_fds;
        tv.tv_sec =10;
        tv.tv_usec=0;
        read_fds =client_set;
        ret = select(max_fd+1,&read_fds,NULL,NULL,&tv);
        if(ret == -1){
            std::cerr<<"Error in select\n";
            exit(SELECT_FAILURE);
        }
        else if(ret == 0){
#ifdef DEBUG
            std::cerr<<"Timeout\n";
#endif
            continue;
        }
        else{
            for(int i = 0;i<FD_SETSIZE;i++){
                if(FD_ISSET(i,&read_fds)){
                    if(i == server_fd){
                        struct sockaddr_in client_addr;
                        new_socket = accept(server_fd,(struct sockaddr *)&client_addr,(socklen_t*)&addr_len);
                        if(new_socket < 0){
                            std::cerr<<"Failed to accept\n";
                            exit(ACCEPT_FAILURE);   
                        }
                        else{
                            FD_SET(new_socket,&client_set);
                            if(new_socket > max_fd){
                                max_fd = new_socket;
                            }
                        }
                    }
                    else{
                        char buf[BUFFERSIZE]={0};                       
                        int read_from_client=read(new_socket,buf,BUFFERSIZE);
                        std::cerr<<"Read from client "<<read_from_client<<'\n';
                        if(read_from_client < 0){
                            continue;
                        }
                        else if(read_from_client == 0){
                            std::cerr<<"Client disconnected\n";
                        }
                        else{
                        std::string msg(buf);

                        int status=Incoming.HTTPRequest(msg,new_socket);
#ifdef DEBUG
                        std::cerr<<buf<<'\n';
#endif
                        }
                        close(i);
                        FD_CLR(i,&client_set);

                    }
                }
            }
        }
        continue;
    }
}
int HTTPD::http_init(int fd){
    server_fd = fd;
    std::cerr<<server_fd<<'\n';
    continuousServer();
    return 0;
}
