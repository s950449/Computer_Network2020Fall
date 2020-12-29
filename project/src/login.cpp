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
    std::string pass_db;
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
        pass_db = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt,0)));
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
    sqlite3_finalize(stmt);
    if(pass.compare(pass_db) == 0){
        std::cerr<<"User "<<username<<" Login Successful\n";
        sqlite3_close(db);
        return 0;
    }
    else{
        std::cerr<<"User "<<username<<" Login Failed\n";
        sqlite3_close(db);        
        return LOGIN_FAILURE;
    }
}
int LoginSystem::Register(std::string username,std::string pass){
    bool accountExist = true;
    sqlite3 *db;
    char buf[256]={0};
    std::string pass_db;
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
        sqlite3_close(db);            
        return 1;
    }
    sqlite3_bind_text(stmt,1,username.c_str(),username.length(),NULL);
    if(sqlite3_step(stmt) == SQLITE_DONE){
        accountExist = false;
    }
    std::string reg_query("INSERT INTO account (user,pass) VALUES(?,?);");    
    if(accountExist){
        std::cerr<<"Account Exist\n";
        sqlite3_close(db);        
        return ACC_EXIST;
    }
    sqlite3_busy_timeout(db,10000);
    sqlite3_stmt *reg_stmt;
    if(sqlite3_prepare_v2(db,reg_query.c_str(),-1,&reg_stmt,NULL)!=SQLITE_OK){
        std::cerr<<"Database Prepare Error\n";
        std::cerr<<sqlite3_errmsg(db)<<'\n';
        sqlite3_close(db);
        return 1;
    }
    int i=1;
    sqlite3_bind_text(reg_stmt,1,username.c_str(),username.length(),NULL);       
    sqlite3_bind_text(reg_stmt,2,pass.c_str(),pass.length(),NULL);

    if(sqlite3_step(reg_stmt) == SQLITE_DONE){
        sqlite3_finalize(reg_stmt);
        sqlite3_close(db);
        std::cerr<<"Register success\n";
        return 0;
    }
    else{
        std::cerr<<"Register Failed\n";
        std::cerr<<sqlite3_errmsg(db)<<'\n';
        sqlite3_finalize(reg_stmt);
        sqlite3_close(db);        
        return 1;
    } 

}
bool LoginSystem::isLogin(std::string username,std::string sessionid){
    bool ret = false;
    std::cerr<<"User:"<<username<<" SessionID:"<<sessionid<<'\n';
    sqlite3 *db;
    char buf[256]={0};
    std::string sessionid_db;
    int status=sqlite3_open("userauth.db",&db);
    const char* query="SELECT sessionid FROM account WHERE user = ? ;";
    char *err_msg=NULL;
    if(!status){
        std::cerr<<"Open Database Successfully\n";
    }
    else{
        std::cerr<<"Failed to open database\n";
        std::cerr<<sqlite3_errmsg(db)<<'\n';
        return false;
    }
    sqlite3_stmt *stmt;
    if(sqlite3_prepare_v2(db,query,-1,&stmt,NULL)!=SQLITE_OK){
        std::cerr<<"Database Prepare Error\n";
        std::cerr<<sqlite3_errmsg(db)<<'\n';
        return false;
    }
    sqlite3_bind_text(stmt,1,username.c_str(),username.length(),NULL);
    if(sqlite3_step(stmt) == SQLITE_DONE){
        std::cerr<<"Account doesn't exist\n";
        return false;
    }
    else{
        sessionid_db = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt,0)));
    }
    std::cerr<<"Hello World\n";
    std::cerr<<" From DB:"<<sessionid_db<<'\n';
    std::cerr<<"SessionID:"<<sessionid;    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    if(sessionid.compare(sessionid_db) == 0){
        std::cerr<<"User "<<username<<"Already Login\n";
        return true;
    }
    else{
        std::cerr<<username<<" Is Not Login\n";
        return false;
    }    
}
int LoginSystem::UpdateSession(std::string username,std::string sessionid){
    std::cerr<<"In UpdateSession\n";
    sqlite3 *db;
    char buf[256]={0};
    std::string pass_db;
    int status=sqlite3_open("userauth.db",&db);
    const char* query="UPDATE account SET sessionid = ? WHERE user = ? ;";
    char *err_msg=NULL;
    if(!status){
        std::cerr<<"Open Database Successfully\n";
    }
    else{
        std::cerr<<"Failed to open database\n";
        std::cerr<<sqlite3_errmsg(db)<<'\n';
        return 1;
    }
    sqlite3_stmt *stmt;
    sqlite3_busy_timeout(db,10000);
    if(sqlite3_prepare_v2(db,query,-1,&stmt,NULL)!=SQLITE_OK){
        std::cerr<<"Database Prepare Error\n";
        std::cerr<<sqlite3_errmsg(db)<<'\n';
        return 1;
    }
    sqlite3_bind_text(stmt,1,sessionid.c_str(),sessionid.length(),NULL);    
    sqlite3_bind_text(stmt,2,username.c_str(),username.length(),NULL);
    sqlite3_busy_timeout(db,100);    
    if(sqlite3_step(stmt) != SQLITE_DONE){
        std::cerr<<"Account doesn't exist\n";
        sqlite3_finalize(stmt);
        std::cerr<<sqlite3_errmsg(db)<<'\n';
        return 1;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;

}