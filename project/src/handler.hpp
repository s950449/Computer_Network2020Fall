#include <iostream>
#include <unistd.h>
#include <string>
#include <sstream>
#include <vector>
#include "status.hpp"
extern char* rootdir;

class RequestHandler{
    private:
        std::vector<std::string> getSubtoken(std::string str,char dil);
        HTTPSpec::Method http_method;

    public:
        RequestHandler(){};
        int HTTPRequest(std::string incoming);
};
 std::vector<std::string> RequestHandler::getSubtoken(std::string str,char dil){
    std::string imm;
    std::stringstream subline(str);
    std::vector<std::string> token;
    while(getline(subline,imm,dil)){
        token.push_back(imm);
    }
    return token;
}
int RequestHandler::HTTPRequest(std::string incoming){
#ifdef DEBUG
    std::cerr<<incoming<<'\n';
#endif
    std::string HTTPMethod;
    std::string TargetFile;
    std::string HTTPVersion;
    std::vector <std::string> Header;
    Header = getSubtoken(incoming,'\n'); // strtok with newline
    std::vector<std::string> Lines;
    Lines = getSubtoken(Header[0],' '); //strtok with space
    HTTPMethod = Lines[0];
    if(HTTPMethod == "GET"){
        http_method = HTTPSpec::Method::GET;
    }
    else if(HTTPMethod == "POST"){
        http_method = HTTPSpec::Method::POST;
    }
    TargetFile = Lines[1];
    HTTPVersion = Lines[2];
    
#ifdef DEBUG
    for(int i = 0;i<Header.size();i++){
        std::cerr<<Header[i]<<'\n';
    }
#endif
    return 0;
}