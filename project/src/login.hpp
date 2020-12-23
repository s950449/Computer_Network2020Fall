#include <sqlite3.h>
#include <iostream>
#include <string>
#define LOGIN_FAILURE 3
#define ACC_EXIST 4
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