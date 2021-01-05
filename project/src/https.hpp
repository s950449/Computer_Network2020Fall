#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <netdb.h>
#include <thread>
#include <mutex>
#define SSL_EXIT_FAILURE -2
class HTTPS{
    private:
        void https_cleanup();
        void https_configure_context(SSL_CTX *ctx,std::string pub,std::string priv);
        SSL_CTX* https_create_context();
        std::string pub;
        std::string pri;
        SSL_CTX* ctx;
        std::mutex g_lock;
    public:
        HTTPS(){};
        void https_init();
        std::string https_serve(int socketfd);
        std::string https_read(SSL* ssl);
        int https_write(SSL* ssl,char* str,int length);    
        void init_key(std::string pubin,std::string priin);
        std::thread multi_https_init(int fd){
            return std::thread(&HTTPS::https_serve,this,fd);
        };
};
