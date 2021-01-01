#include "handler.hpp"
#include "login.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <random>
#include <string>

std::string RequestHandler::random_string()
{
     std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

     std::random_device rd;
     std::mt19937 generator(rd());

     std::shuffle(str.begin(), str.end(), generator);
    /* Ref: https://stackoverflow.com/a/47978023 */
     return str.substr(0, 7);    // assumes 32 < number of characters in str         
}
std::string RequestHandler::shorturlGen(std::string longurl){
    std::string shorten_url = random_string();
    std::string tmp_for_pass;
    while(getShorturl(shorten_url,tmp_for_pass)){
        shorten_url = random_string();
    }
    sqlite3 *db;
    char buf[256]={0};
    int status=sqlite3_open("shorturl.db",&db);
    char *err_msg=NULL;
    if(!status){
        std::cerr<<"Open Database Successfully\n";
    }
    else{
        std::cerr<<"Failed to open database\n";
        std::cerr<<sqlite3_errmsg(db)<<'\n';
        std::string tmp("Failed to open database\n");
        return tmp;
    }
    sqlite3_stmt *stmt;
    std::string query("INSERT INTO shorturl (shortenurl,longurl) VALUES(?,?);");    
    sqlite3_busy_timeout(db,10000);
    if(sqlite3_prepare_v2(db,query.c_str(),-1,&stmt,NULL)!=SQLITE_OK){
        std::cerr<<"Database Prepare Error\n";
        std::cerr<<sqlite3_errmsg(db)<<'\n';
        sqlite3_close(db);
        std::string tmp("Database Prepare Error\n");
        return tmp;
    }
    int i=1;
    sqlite3_bind_text(stmt,1,shorten_url.c_str(),shorten_url.length(),NULL);       
    sqlite3_bind_text(stmt,2,longurl.c_str(),longurl.length(),NULL);

    if(sqlite3_step(stmt) == SQLITE_DONE){
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        std::cerr<<"Shorten success\n";
        return shorten_url;
    }
    else{
        std::cerr<<"Shorten failed\n";
        std::cerr<<sqlite3_errmsg(db)<<'\n';
        sqlite3_finalize(stmt);
        sqlite3_close(db);        
        std::string tmp("Shorten failed\n");
        return tmp;
    } 

}
bool RequestHandler::getShorturl(std::string shorturl,std::string& longurl){
    bool ret_flags = false;
    std::stringstream ret;
    std::cerr<<shorturl<<'\n';
    sqlite3* db;
    char buf[256]={0};
    int status=sqlite3_open("shorturl.db",&db);
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
    std::string longurl_db;
    std::string query("select longurl from shorturl where shortenurl = ? ;");    
    sqlite3_busy_timeout(db,10000);
    if(sqlite3_prepare_v2(db,query.c_str(),-1,&stmt,NULL)!=SQLITE_OK){
        std::cerr<<"Database Prepare Error\n";
        std::cerr<<sqlite3_errmsg(db)<<'\n';
        sqlite3_close(db);
        return false;
    }    
    sqlite3_bind_text(stmt,1,shorturl.c_str(),shorturl.length(),NULL);
    if(sqlite3_step(stmt) == SQLITE_DONE){
        std::cerr<<"This shorturl doesn't exist\n";
        sqlite3_close(db);
        return false;
    }
    else{
        longurl = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt,0)));
    }       
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return true;
}