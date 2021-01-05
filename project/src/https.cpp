#include "https.hpp"
#include "handler.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <netdb.h>
void HTTPS::https_init(){
    SSL_library_init();
    SSL_load_error_strings();	
    OpenSSL_add_ssl_algorithms();
    return;
}
void HTTPS::https_cleanup(){
    EVP_cleanup();
    return;
}
void HTTPS::init_key(std::string pubin,std::string priin){
    pub = pubin;
    pri = priin;
    return ;
}
void HTTPS::https_configure_context(SSL_CTX *ctx,std::string pub,std::string priv)
{
    SSL_CTX_set_ecdh_auto(ctx, 1);

    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(ctx, pub.c_str(), SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
	exit(SSL_EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, priv.c_str(), SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
	exit(SSL_EXIT_FAILURE);
    }
}
SSL_CTX* HTTPS::https_create_context()
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = TLS_method();
    ctx = SSL_CTX_new(method);
    SSL_CTX_set_min_proto_version(ctx,TLS1_2_VERSION);
    SSL_CTX_set_mode(ctx,SSL_MODE_AUTO_RETRY);
    if (!ctx) {
	perror("Unable to create SSL context");
	ERR_print_errors_fp(stderr);
	exit(SSL_EXIT_FAILURE);
    }
    return ctx;
}
std::string HTTPS::https_read(SSL* ssl){

    char buf[1024]={0};
    int ret=SSL_read(ssl,buf,sizeof(buf));
    if(ret<=0){
        if(SSL_get_error(ssl,ret)==SSL_ERROR_WANT_READ){
            sleep(1);
            std::string msg;
            msg+=https_read(ssl);
            return msg;
        }
        else{
                std::string ssl_err_msg("SSL_read_failed\n");
                return ssl_err_msg;
        }
    }
    std::string msg(buf);
    return msg;
}
int HTTPS::https_write(SSL* ssl,char* str,int length){
    int ret=SSL_write(ssl,str,length);
    if(SSL_get_error(ssl,ret)==SSL_ERROR_WANT_WRITE){
        sleep(1);
        ret+=https_write(ssl,str,length);
        return ret;
    }
    else{
        return ret;
    }
}
std::string HTTPS::https_serve(int socketfd){
    SSL* ssl;
    https_init();
    ctx = https_create_context();
    https_configure_context(ctx,pub,pri);
    while(1){
        struct sockaddr_in addr;
        u_int32_t len = sizeof(addr);
        SSL *ssl;
        std::string reply;
        int client = accept(socketfd,(struct sockaddr*)&addr,&len);
        if(client < 0){
            std::cerr<<"Unable to accept\n";
            exit(EXIT_FAILURE);
        }
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl,client);
        if(SSL_accept(ssl)<=0){
            ERR_print_errors_fp(stderr);
        }
        else{
            char buf[BUFFERSIZE]={0};  
            g_lock.lock();              
            int read_from_client=SSL_read(ssl,buf,BUFFERSIZE);  
            g_lock.unlock();         
            RequestHandler myHandler;
            std::string msg(buf);
            int fd = open("/dev/null",O_WRONLY);
            myHandler.setupSSL(ssl);
            g_lock.lock();
            myHandler.HTTPRequest(msg,fd);
            g_lock.unlock();
        }
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(client);
    }
    close(socketfd);
    SSL_CTX_free(ctx);
    https_cleanup();
}

