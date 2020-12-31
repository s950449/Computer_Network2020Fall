#include <cstring>
#include <cstdio>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <netinet/in.h>
#include <iostream>
#include <thread>
#include "https.hpp"
#include "httpd.hpp"
#define SOCKET_FAILURE 7
#define BIND_FAILURE 8
#define LISTEN_FAILURE 9
#define ACCEPT_FAILURE 10
#define SELECT_FAILURE 11
#define READ_FAILURE 12
class SOCKETD{
    private:
        int server_fd;
        int addr_len;
        struct sockaddr_in address;
        int createSocket();
        int bindSocket();
        int listenSocket();
        std::string pub;
        std::string pri;
    public:
        SOCKETD(){
        };
        ~SOCKETD(){
        };
        enum class server{
            HTTP,HTTPS,OTHER
        };
        void init_server(unsigned short port,SOCKETD::server type);
        void pass_key(std::string pubin,std::string priin);

};
int SOCKETD::createSocket(){
    server_fd =  socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == 0){
        std::cerr<<"Failed to create socket\n";
        exit(SOCKET_FAILURE);
    }
    return 0;    
}
int SOCKETD::bindSocket(){
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        std::cerr<<"Failed to bind\n";
        exit(BIND_FAILURE);
    }     
    return 0;
}
int SOCKETD::listenSocket(){
    if(listen(server_fd,10) < 0){
        std::cerr<<"Failed to listen\n";
        exit(LISTEN_FAILURE);
    }
    return 0;
}
void SOCKETD::pass_key(std::string pubin,std::string priin){
    pub = pubin;
    pri = priin;
    return;
}
void SOCKETD::init_server(unsigned short port,SOCKETD::server type){
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
    struct timeval tv;
    tv.tv_sec=10;
    tv.tv_usec=0;
    setsockopt(server_fd,SOL_SOCKET,SO_SNDTIMEO | SO_RCVTIMEO,(char*)&tv,sizeof(struct timeval));
    HTTPD http_server;
    HTTPS https_server;
    std::thread t1;
    std::thread t2;
    switch(type){
        case server::HTTPS:
            std::cerr<<"https mode\n";
            https_server.init_key(pub,pri);
            https_server.https_serve(server_fd);
            break;
        case server::HTTP:
            std::cerr<<"http mode\n";
            //http_server.http_init(server_fd);
            t1 = http_server.multi_http_init(server_fd);
            t2 = http_server.multi_http_init(server_fd);
            t1.join();
            t2.join();
            break;
        case server::OTHER:
        default:
            std::cerr<<"Default mode, using http\n";
            t1 = http_server.multi_http_init(server_fd);
            t2 = http_server.multi_http_init(server_fd);
            t1.join();
            t2.join();

    }
    return;
}
