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
    if (!ctx) {
	perror("Unable to create SSL context");
	ERR_print_errors_fp(stderr);
	exit(SSL_EXIT_FAILURE);
    }
    return ctx;
}
std::string https_serve(SSL* ssl){
    int read_from_client=0;
    char buf[1024]={0};
    if(SSL_accept(ssl)==-1){
        std::string ssl_err_msg("SSL_accept_failed\n");
        std::cerr<<ssl_err_msg;
        return ssl_err_msg;
    }
    else{
        read_from_client = SSL_read(ssl,buf,sizeof(buf));
        if(read_from_client<=0){
            std::string ssl_err_msg("SSL_read_failed\n");
            return ssl_err_msg;
        }
        std::string msg(buf);
        return msg;
    }
}