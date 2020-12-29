#include <sqlite3.h>
#include <iostream>
#include <string>
#define LOGIN_FAILURE 3
#define ACC_EXIST 4
#define WRONG_PASSWD 5
class LoginSystem{
    private:

    public:
        LoginSystem(){

        };
        ~LoginSystem(){

        };
        int Login(std::string username,std::string pass);
        int Register(std::string username,std::string pass);
        bool isLogin(std::string username,std::string sessionid);
        int UpdateSession(std::string username,std::string sessionid);
};