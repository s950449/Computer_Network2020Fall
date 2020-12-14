#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <netdb.h>
#define SSL_EXIT_FAILURE -2
void https_init(){
    SSL_library_init();
    SSL_load_error_strings();	
    OpenSSL_add_ssl_algorithms();
    return;
}
void https_cleanup(){
    EVP_cleanup();
    return;
}
void https_configure_context(SSL_CTX *ctx,std::string pub,std::string priv)
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
SSL_CTX* https_create_context()
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
std::string https_read(SSL* ssl){

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
int https_write(SSL* ssl,char* str,int length){
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
std::string https_serve(SSL* ssl){
    int read_from_client=0;
    char buf[1024]={0};
    int ret=SSL_accept(ssl);
    if(ret==-1){
        switch(SSL_get_error(ssl,ret)){
            case SSL_ERROR_WANT_READ:
                return https_serve(ssl);
            default:
                break;
        }
        std::string ssl_err_msg("SSL_accept_failed\n");
        std::cerr<<ssl_err_msg;
        return ssl_err_msg;
    }
    else{
        return https_read(ssl);
    }
}
