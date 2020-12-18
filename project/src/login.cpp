#include "login.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
static int LoginCallback(void *data,int argc,char **argv,char **colname){
    std::cerr<<(const char*)data<<'\n';
    for(int i=0;i<argc;i++){
        std::cerr<<colname[i]<<" "<<(argv[i]?argv[i]:"NULL");
    }
    std::cerr<<'\n';
    return 0;
}
int LoginSystem::Login(std::string username,std::string pass){
    sqlite3 *db;
    char buf[256]={0};
    std::cerr<<getcwd(buf,sizeof(buf));
    int status=sqlite3_open("userauth.db",&db);
    const char* query="SELECT pass FROM account WHERE user = ? ;";
    char *err_msg=NULL;
    const char* data="Callback function";
    if(!status){
        std::cerr<<"Open Database Successfully\n";
    }
    else{
        std::cerr<<"Failed to open database\n";
        std::cerr<<sqlite3_errmsg(db)<<'\n';
        return 1;
    }
    sqlite3_stmt *stmt;
    if(sqlite3_prepare_v2(db,query,-1,&stmt,NULL)!=SQLITE_OK){
        std::cerr<<"Database Prepare Error\n";
        std::cerr<<sqlite3_errmsg(db)<<'\n';
        return 1;
    }
    sqlite3_bind_text(stmt,1,username.c_str(),username.length(),NULL);
    if(sqlite3_step(stmt) == SQLITE_DONE){
        std::cerr<<"Account doesn't exist\n";
        return 1;
    }
    else{
        std::string password = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt,0)));
        std::cerr<<"Password from db "<<password<<'\n';
    }
#ifdef oldsql
    status = sqlite3_exec(db,query,LoginCallback,(void*)data,&err_msg);
    if(status != SQLITE_OK){
        std::cerr<<err_msg<<'\n';
        sqlite3_free(err_msg);
    }
    else{
        std::cerr<<"Database select OK\n";
    }
#endif
    sqlite3_close(db);
    return 0;
}