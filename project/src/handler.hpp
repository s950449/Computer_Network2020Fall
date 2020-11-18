#include <iostream>
#include <unistd.h>
class RequestHandler{
    private:

    public:
        RequestHandler(){};
        int HTTPRequest(std::string incoming);
};
int RequestHandler::HTTPRequest(std::string incoming){
    std::cerr<<incoming<<'\n';
    return 0;
}