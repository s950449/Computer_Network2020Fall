#include <sqlite3.h>
#include <iostream>
#include <string>
class LoginSystem{
    private:

    public:
        LoginSystem(){

        };
        ~LoginSystem(){

        };
        int Login(std::string username,std::string pass);
        int Register(std::string username,std::string pass);
};