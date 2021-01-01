#include <iostream>
#include <unistd.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <fcntl.h>
#include "status.hpp"
#include <magic.h>
#include <openssl/sha.h>
#include <curl/curl.h>
#include <sys/time.h>
#include <thread>
#include <openssl/ssl.h>
#ifdef Regex
#include <regex>
#endif
extern char* rootdir;
#define ROOTDIR "./var"
#define BUFFERSIZE 16384
#define MAX_FILENAME 4096

class RequestHandler{
    private:
        std::string random_string();
        bool https_mode;
        SSL* ssl;
        std::string workingdir;
        int msgfd;
        int fd;
        const char* conlen="Content-Length: ";
        const char* contype="Content-Type: ";
        char *files;
        std::string shorturlGen(std::string longurl);
        bool getShorturl(std::string shorturl,std::string& longurl);
        std::string html_msg(std::string str);
        std::vector<std::string> getSubtoken(std::string str,char dil);
        HTTPSpec::Method http_method;
        HTTPSpec::Version http_version;
        bool checkPath(std::string filepath);
        bool checkLogin(std::string str);
        enum class FILE_TYPE{
            IMG_JPEG,TEXT_HTML,IMG_PNG,TEXT_PLAIN,TEXT_CSS,TEXT_JS,
        };
        void get_http_method(std::string str);
        void get_http_version(std::string str);
        int FILE_O(std::string filepath);
        int FILE_IO(int socketfd);
        void FILE_C();
        void not_found();
        void ok();
        std::string GetFileType(std::string filepath);
        std::stringstream responseS;
        std::string response;
        std::ifstream myfile;
        int binToSocket(char* buf,int socketfd,int bufL);
        int strToSocket(std::string str,int socketfd);
        std::string curlDecoding(std::string str);
        std::string curlEncoding(const char* str);
        int LoginHandler(std::string str);
        int RegisterHandler(std::string str);
        int msgHandler(std::string str);
        int showMsgHandler(std::string str);
        std::string SetCookie(std::string username);
    public:
        bool setupSSL(SSL* inssl);
        RequestHandler(){
            https_mode = false;
            char working_dir_buf[MAX_FILENAME]={0};
            getcwd(working_dir_buf,MAX_FILENAME);
            workingdir+=working_dir_buf;
            workingdir+='/';
            workingdir+=ROOTDIR;
        };
        void clearSSL(){
            https_mode = false;
        }
        std::thread multi_HTTPRequest(std::string incoming,int socketfd){
            return std::thread(&RequestHandler::HTTPRequest,this,incoming,socketfd);
        };
        int HTTPRequest(std::string incoming,int socketfd);
        std::string output;

};
