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
#include "mysha.hpp"
#include "login.hpp"
#include <sys/time.h>
#ifdef Regex
#include <regex>
#endif
extern char* rootdir;
#define ROOTDIR "./var"
#define BUFFERSIZE 16384
#define MAX_FILENAME 4096

class RequestHandler{
    private:
        std::string workingdir;
        int msgfd;
        int fd;
        const char* conlen="Content-Length: ";
        const char* contype="Content-Type: ";
        char *files;
        SSL* ssl;
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
        std::string curlDecoding(std::string str);
        std::string curlEncoding(const char* str);
        int LoginHandler(std::string str);
        int RegisterHandler(std::string str);
        int msgHandler(std::string str);
        int showMsgHandler(std::string str);
        std::string SetCookie(std::string username);
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
std::string RequestHandler::SetCookie(std::string username){
    std::string ret("Set-Cookie: ");
    ret+="username="+username;
    return ret;
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
    //chdir(workingdir.c_str());
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
std::string RequestHandler::curlEncoding(const char* str){
    CURL *curl = curl_easy_init();
    if(curl){
        char *tmp = curl_easy_unescape(curl,str,strlen(str),NULL);
        if(tmp){
            std::string ret(tmp);
            curl_free(tmp);
            return ret;
        }
    }
    std::string error_msg="ERROR\n";
    return error_msg;    
}
std::string RequestHandler::curlDecoding(std::string str){
    CURL *curl=curl_easy_init();
    if(curl){
        char *tmp = curl_easy_escape(curl,str.c_str(),str.length());
        if(tmp){
            std::string ret(tmp);
            curl_free(tmp);
            return ret;
        }
    }
    std::string error_msg="ERROR\n";
    return error_msg;
}
int RequestHandler::showMsgHandler(std::string str){
    std::stringstream response;
    response<<"HTTP/1.1 200 OK\r\n";
    response<<conlen<<str.length();
    response<<contype<<"text/html\r\n";
    response<<"\r\n";
    response<<str;
    strToSocket(response.str(),msgfd);
    return 0;
}
int RequestHandler::msgHandler(std::string str){
    struct timeval tv;
    std::vector<std::string>Lines;
    Lines=getSubtoken(str,'=');
    gettimeofday(&tv,NULL);
    std::cerr<<tv.tv_sec<<"."<<tv.tv_usec<<'\n';
    std::stringstream myStream;
    myStream<<tv.tv_sec<<tv.tv_usec;
    sqlite3 *db;
    char buf[256]={0};
    std::string pass_db;
    int status=sqlite3_open("msgboard.db",&db);
    char *err_msg=NULL;
    if(!status){
        std::cerr<<"Open Database Successfully\n";
    }
    else{
        std::cerr<<"Failed to open database\n";
        std::cerr<<sqlite3_errmsg(db)<<'\n';
        return 1;
    }    
    if(strcmp(Lines[0].c_str(),"msg")==0){
        const char* query = "INSERT INTO msgboard (time,msg) VALUES(?,?);";

        sqlite3_stmt *stmt;
        if(sqlite3_prepare_v2(db,query,-1,&stmt,NULL)!=SQLITE_OK){
            std::cerr<<"Database Prepare Error\n";
            std::cerr<<sqlite3_errmsg(db)<<'\n';
            return 1;
        }
        std::string mytime = myStream.str();
        sqlite3_bind_text(stmt,1,mytime.c_str(),mytime.length(),NULL);
        sqlite3_bind_text(stmt,2,Lines[1].c_str(),Lines[1].length(),NULL);
        if(sqlite3_step(stmt) == SQLITE_DONE){
            sqlite3_finalize(stmt);
            std::cerr<<"Message success\n";
            std::stringstream retmsg;
            retmsg<<"<!DOCTYPE HTML>";
            retmsg<<"<html lang=\"en\">";
            retmsg<<"<head>";
            retmsg<<"<title>留言成功</title>";
            retmsg<<"<meta charset=\"UTF-8\">";
            retmsg<<"</head>";
            retmsg<<"<body>";
            retmsg<<"<h1>Success</h1>";
            retmsg<<"</body>";
            retmsg<<"</html>";
            showMsgHandler(retmsg.str());            
            return 0;
        }
        else{
            std::cerr<<"Message Failed\n";
            std::cerr<<sqlite3_errmsg(db)<<'\n';
            sqlite3_finalize(stmt);
            return 1;
        }     
    }
    else if(strcmp(Lines[0].c_str(),"showmsg")==0){
        const char* query = "SELECT msg FROM msgboard;";
        std::stringstream retmsg;
        sqlite3_stmt *stmt;
        if(sqlite3_prepare_v2(db,query,-1,&stmt,NULL)!=SQLITE_OK){
            std::cerr<<"Database Prepare Error\n";
            std::cerr<<sqlite3_errmsg(db)<<'\n';
            return 1;
        }
        if(sqlite3_step(stmt) == SQLITE_DONE){
            std::cerr<<"Message doesn't exist\n";
            return 1;
        }
        else{
            int j=1;
            retmsg<<"<!DOCTYPE HTML>";
            retmsg<<"<html lang=\"en\">";
            retmsg<<"<head>";
            retmsg<<"<title>顯示留言板</title>";
            retmsg<<"<meta charset=\"UTF-8\">";
            retmsg<<"</head>";
            retmsg<<"<body>";
            while(sqlite3_step(stmt)==SQLITE_ROW){
                retmsg<<"<p>"<<j<<" "<<curlEncoding(reinterpret_cast<const char*>(sqlite3_column_text(stmt,0)))<<"</p>";
                j++;
            };
            retmsg<<"</body>";
            retmsg<<"</html>";
            showMsgHandler(retmsg.str());
        }        
    }
    return 0;
}

int RequestHandler::LoginHandler(std::string str){
    std::vector<std::string> Lines;
    std::cerr<<"In Login Handler\n";
    Lines = getSubtoken(str,'&');
#ifdef Debug
    for(int i = 0;i < Lines.size();i++){
        std::cerr<<Lines[i]<<'\n';
    }
#endif
    std::vector<std::string> tmp;
    tmp=getSubtoken(Lines[0],'=');
    std::string username = tmp[1];
    tmp=getSubtoken(Lines[1],'=');
    std::string pw = tmp[1];
    LoginSystem Test;
    int status=Test.Login(username,pw);
    if(status == LOGIN_FAILURE){
        return 1;
    }
    else if(status == 0){
        return 0;
    }
    return 1;
}
int RequestHandler::RegisterHandler(std::string str){
    std::vector<std::string> Lines;
    std::cerr<<"In Register Handler\n";
    Lines = getSubtoken(str,'&');
    std::vector<std::string> tmp;
    tmp=getSubtoken(Lines[0],'=');
    std::string username = tmp[1];
    tmp=getSubtoken(Lines[1],'=');
    std::string pw = tmp[1];
    tmp=getSubtoken(Lines[2],'=');
    std::string pwr = tmp[1];
    unsigned char pw_hash[256],pwr_hash[256];
    unsigned char tmp_hash[256]={};
    std::copy(pw.begin(),pw.end(),tmp_hash);
    tmp_hash[pw.length()]=0;
    SHA256(tmp_hash,pw.length(),pw_hash);
    memset(tmp_hash,0,sizeof(tmp_hash));
    std::copy(pwr.begin(),pwr.end(),tmp_hash);
    tmp_hash[pwr.length()]=0;    
    SHA256(tmp_hash,pwr.length(),pwr_hash);

    std::cerr<<pw_hash<<" "<<pwr_hash<<'\n';
    if(pw != pwr){
        return 3;
    }
    LoginSystem Test;
    int status = Test.Register(username,pw);
    if(status == ACC_EXIST){
        return 3;
    }
    else if(status == 0){
        return 0;
    }
    else{
        return 1;
    }
#ifdef Debug
    for(int i = 0;i < Lines.size();i++){
        std::cerr<<Lines[i]<<'\n';
    }
#endif
    return 0;    
}

int RequestHandler::HTTPRequest(std::string incoming,int socketfd){
    if(incoming.empty()){
        return -1;
    }
    response.clear();
    fd=socketfd;
    std::string HTTPMethod;
    std::string TargetFile;
    std::string HTTPVersion;
    std::vector <std::string> Header;
    Header = getSubtoken(incoming,'\n'); // strtok with newline
    std::cerr<<"Get Header\n";
    if(Header.empty()){
        http_method = HTTPSpec::Method::BAD_REQUEST;
        http_version = HTTPSpec::Version::HTTP1_1;
        return -1;
    }
    std::vector<std::string> Lines;
    Lines = getSubtoken(Header[0],' '); //strtok with space
    std::cerr<<"Get Lines\n";
    if(Lines.empty()){
        http_method = HTTPSpec::Method::BAD_REQUEST;
        http_version = HTTPSpec::Version::HTTP1_1;
        return -1;
    }
    if(Lines.size()<3){        
        std::cerr<<"Incoming Size Not Valid\n";
        http_method = HTTPSpec::Method::BAD_REQUEST;
        http_version = HTTPSpec::Version::HTTP1_1;
        return -1;
    }
    HTTPMethod = Lines[0];
    TargetFile = Lines[1];
    HTTPVersion = Lines[2];
    get_http_method(HTTPMethod);
    std::cerr<<"Get HTTP Method\n";
    get_http_version(HTTPVersion);
    std::cerr<<"Get HTTP Version\n";
    std::string target_dir; 
    std::string rootdir_test(ROOTDIR);
    rootdir_test+="/"; 
    int file_length = -1; 
    int status=-1;
#ifdef VERBOSE
    std::cerr<<incoming<<'\n';
#endif
    switch(http_method){
        case HTTPSpec::Method::GET:
            std::cerr<<"GET "<<Lines[1]<<'\n';
            target_dir = ROOTDIR + Lines[1];
            std::cerr<<target_dir<<'\n';
            if(target_dir.compare(rootdir_test)==0){
                target_dir = ROOTDIR + Lines[1] + "index.html";
            }
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
            std::cerr<<HTTPMethod<<' '<<TargetFile<<' '<<HTTPVersion<<'\n';
            std::cerr<<curlDecoding(Header[Header.size()-1])<<'\n';

            if(TargetFile=="/login.html"){
                status=LoginHandler(Header[Header.size()-1]);
                if(status == 0){
                    std::vector<std::string>tmp_user,tmp_str;
                    tmp_str=getSubtoken(Header[Header.size()-1],'&');
                    tmp_user=getSubtoken(tmp_str[0],'=');
                    response +="HTTP/1.1 ";
                    response +="302 Found\r\n";
                    response+="Location: /index.html\r\n";
                    response=response+SetCookie(tmp_user[1])+"\r\n";
                    strToSocket(response,socketfd);
                    FILE_IO(socketfd);
                    FILE_C();                    
                }
                else{
                    response+="HTTP/1.1 ";
                    response+="403 Forbidden\r\n";
                    strToSocket(response,socketfd);                  
                }
            }
            else if(TargetFile == "/register.html"){
                status=RegisterHandler(Header[Header.size()-1]);
                if(status == 3 || status == 1){
                    //Password Repeat Error
                    response+="HTTP/1.1 ";
                    response+="403 Forbidden\r\n";
                    strToSocket(response,socketfd);
                }
                else if(status == 0){
                    std::vector<std::string>tmp_user,tmp_str;
                    tmp_str=getSubtoken(Header[Header.size()-1],'&');
                    tmp_user=getSubtoken(tmp_str[0],'=');
                    response +="HTTP/1.1 ";
                    response +="302 Found\r\n";
                    response+="Location: /register_success.html\r\n";
                    strToSocket(response,socketfd);
                    FILE_IO(socketfd);
                    FILE_C();                       
                }
            }
            else if(TargetFile == "/msg_board.html"){
                msgfd = socketfd;
                status = msgHandler(Header[Header.size()-1]);
            }

            break;
        case HTTPSpec::Method::BAD_REQUEST:
            response = response + "400 Bad Request\r\n";
            strToSocket(response,socketfd);
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