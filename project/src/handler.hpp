#include <iostream>
#include <unistd.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <fcntl.h>
#include "status.hpp"
#include <magic.h>
#ifdef Regex
#include <regex>
#endif
extern char* rootdir;
#define ROOTDIR "./var"
#define BUFFERSIZE BUFSIZ
#define MAX_FILENAME 4096

class RequestHandler{
    private:
        std::string workingdir;
        int fd;
        const char* conlen="Content-Length: ";
        const char* contype="Content-Type: ";
        char *files;
        std::vector<std::string> getSubtoken(std::string str,char dil);
        HTTPSpec::Method http_method;
        HTTPSpec::Version http_version;
        bool checkPath(std::string filepath);
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
    public:
        RequestHandler(){
            char working_dir_buf[MAX_FILENAME]={0};
            getcwd(working_dir_buf,MAX_FILENAME);
            workingdir+=working_dir_buf;
            workingdir+='/';
            workingdir+=ROOTDIR;
        };
        int HTTPRequest(std::string incoming,int socketfd);
};
void RequestHandler::FILE_C(){
    myfile.close();
    return;
}
bool RequestHandler::checkPath(std::string filepath){
#ifndef Regex
    std::cerr<<workingdir<<'\n';
    std::cerr<<filepath<<'\n';
    std::string tmp;
    tmp= workingdir+filepath;
    int ret=chdir(tmp.c_str());
    if(ret==-1){
        chdir(workingdir.c_str());
        return false;
    }
    char buf[MAX_FILENAME]={0};
    getcwd(buf,MAX_FILENAME);
    std::cerr<<"Buf "<<buf<<'\n';
    if(strncmp(buf,workingdir.c_str(),workingdir.length())!=0){
        chdir(workingdir.c_str());    
        return false;
    }
    else{
        chdir(workingdir.c_str());
        return true;
    }
#else
    std::regex e("\\.{2}");
    std::cout<<filepath<<'\n';
    if(std::regex_search(filepath,e)){
        std::cerr<<"Do not contain .. in target_dir\n";
        return false;
    }
    else{
        return true;
    }
#endif
}
int RequestHandler::FILE_IO(int socketfd){
    int ret=0;
    
    if(myfile.is_open()){
        myfile.seekg(0,myfile.end);
        int length = myfile.tellg();
        myfile.seekg(0,myfile.beg);
        std::cerr<<"FILE_IO\n";
        char buf[BUFFERSIZE]={0};
        std::cerr<<length<<'\n';
//        myfile.read(buf,length);
        ret = length;
        binToSocket(buf,socketfd,length);
        std::cerr<<"Bye\n";
    }
    return ret;
}
std::string RequestHandler::GetFileType(std::string filepath){
    magic_t magic;
    const char* mime;
    std::cerr<<"Get File Type\n";
    std::cerr<<filepath<<'\n';
    magic = magic_open(MAGIC_MIME_TYPE);
    magic_load(magic,NULL);
    mime = magic_file(magic,filepath.c_str());
    std::stringstream tmp;
    tmp<<mime;
    std::string ret= tmp.str();
    std::cerr<<ret<<'\n';
    return ret;
}
void RequestHandler::not_found(){
    response +="HTTP/1.1 ";
    response +="404 NOT FOUND\r\n";
    response+=conlen;
    std::string html404;
    html404 = html404+ROOTDIR+"/404.html";
    int ret=FILE_O(html404);
    //responseS<<"HTTP/1.1 404 NOT FOUND\r\n"<<contype<<"\r\n"<<contype<<"text/html\r\n"<<conlen<<ret<<"\r\n";
    response=response+std::to_string(ret)+"\r\n";
    response+=contype;
    response=response+"text/html"+"\r\n";
    response=response+"\r\n";
    strToSocket(response,fd);
    FILE_IO(fd);
    //response+=fromfile;
    FILE_C();
    return;
}
int RequestHandler::FILE_O(std::string filepath){
    int ret = 0;
    myfile.open(filepath,std::ios::binary);
#ifdef DEBUG
    std::cerr<<"Hello\n";
#endif    
    if(!myfile.is_open()){
        return -1;
    }
    else{
        myfile.seekg(0,myfile.end);
        int length = myfile.tellg();
        myfile.seekg(0,myfile.beg);
        ret = length;
    }
    return ret;
}
void RequestHandler::get_http_method(std::string str){
    if(str == "GET"){
        http_method = HTTPSpec::Method::GET;
    }
    else if(str == "POST"){
        http_method = HTTPSpec::Method::POST;
    }
    else{
        http_method = HTTPSpec::Method::INVALID;
    }
    return;
}
void RequestHandler::get_http_version(std::string str){
    if(str == "HTTP/1.1"){
        http_version = HTTPSpec::Version::HTTP1_1;
    }
    return;
}
std::vector<std::string> RequestHandler::getSubtoken(std::string str,char dil){
    std::string imm;
    std::stringstream subline(str);
    std::vector<std::string> token;
    while(getline(subline,imm,dil)){
        token.push_back(imm);
    }
    return token;
}
int RequestHandler::binToSocket(char* buf,int socketfd,int bufL){
    int cur = 0;
    int cnt=0;
#ifndef Test
    while(cur < bufL){
        memset(buf,0,BUFFERSIZE);
        if(bufL-cur < BUFFERSIZE){
            myfile.read(buf,bufL-cur);
            char tmp[bufL-cur];
            memcpy(tmp,buf,bufL-cur);
            //cnt+=write(socketfd,buf+cur,bufL-cur);
            cnt+=write(socketfd,tmp,bufL-cur);
        }
        else{
            myfile.read(buf,BUFFERSIZE);
            //cnt+=write(socketfd,buf+cur,BUFFERSIZE);
            cnt+=write(socketfd,buf,BUFFERSIZE);
        }
        cur+=BUFFERSIZE;
    }
    std::cerr<<"cnt "<<cnt<<'\n';
#else
    std::cerr<<"BUFL "<<bufL<<'\n';
    int ret=write(socketfd,buf,bufL);
    std::cerr<<"RET"<<ret<<"\n";
#endif
    return 0;
}
int RequestHandler::strToSocket(std::string str,int socketfd){
    int strL = str.length();
    char writestr[strL+1];
    memset(writestr,0,sizeof(writestr));
    strcpy(writestr,str.c_str());
    int cur = 0;
    while(cur<strL){
        if(strL-cur < BUFFERSIZE){
            write(socketfd,writestr+cur,strL-cur);
        }
        else{
            write(socketfd,writestr+cur,BUFFERSIZE);
        }
        cur+=BUFFERSIZE;
    }
    return 0;
}
int RequestHandler::HTTPRequest(std::string incoming,int socketfd){
#ifdef DEBUG
    std::cerr<<incoming<<'\n';
#endif
    if(incoming.empty()){
        return 0;
    }
    response.clear();
    fd=socketfd;
    std::string HTTPMethod;
    std::string TargetFile;
    std::string HTTPVersion;
    std::vector <std::string> Header;
    Header = getSubtoken(incoming,'\n'); // strtok with newline
    std::vector<std::string> Lines;
    Lines = getSubtoken(Header[0],' '); //strtok with space
    HTTPMethod = Lines[0];
    TargetFile = Lines[1];
    HTTPVersion = Lines[2];
    get_http_method(HTTPMethod);
    get_http_version(HTTPVersion);
    std::string target_dir;  
    int file_length = -1; 
    switch(http_method){
        case HTTPSpec::Method::GET:
            std::cerr<<"GET "<<Lines[1]<<'\n';
            target_dir = ROOTDIR + Lines[1];
            std::cerr<<target_dir<<'\n';

            if(!checkPath(target_dir)){
                std::cerr<<"Suspicious GET Request\n";
                not_found();
                break;
            }           
            file_length = FILE_O(target_dir);
            if(file_length == -1){
                std::cerr<<"File not found\n";
                not_found();
                std::cerr<<response<<'\n';
                break;
            }
            else{
                response +="HTTP/1.1 ";
                response +="200 OK\r\n";
                response+=conlen;
                std::cerr<<"File Length"<<std::to_string(file_length)<<'\n';
                response=response+std::to_string(file_length)+"\r\n";
                response+=contype;
                response=response+GetFileType(target_dir)+"\r\n";
                response+="\r\n";
                strToSocket(response,socketfd);
                FILE_IO(socketfd);
                FILE_C();
                

            }
            break;
        case HTTPSpec::Method::POST:
            break;
        case HTTPSpec::Method::INVALID:
        default:
            std::cerr<<"Invalid Method\n";
            response = response + "405 Method Not Allowed\n";
            strToSocket(response,socketfd);
    }

#ifdef DEBUG
    for(int i = 0;i<Header.size();i++){
        std::cerr<<Header[i]<<'\n';
    }
#endif
    return 0;
}