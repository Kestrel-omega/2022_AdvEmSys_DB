#include <sqlite3.h>
#include <stdio.h>
#include <md5.h>

int main(void)
{
    sqlite3 *db;
    char *err_msg = 0;
    char buffer[] = NULL;
    
    int rc = sqlite3_open("test.db", &db);
    
    if (rc != SQLITE_OK)
    {
        // fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        printf("Cannot open database\r\n");
        sqlite3_close(db);
        
        return 1;
    }
    
    // insert db data to test.db
    char *sql = "CREATE TABLE User (userid INTEGER PRIMARY KEY AUTOINCREMENT, nickname, pw);"
                ""
                "";

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    snprintf("")
    
    if (rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        
        return 1;
    } 
    
    sqlite3_close(db);
    
    return 0;
}