#include <iostream>
#include <unistd.h>
class RequestHandler{
    private:
            
    public:
        RequestHandler(){};
        std::string HTTPRequest(std::string incoming);
};