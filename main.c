#include <stdio.h>
#include "sqlite3.h"

sqlite3 *handle;
int flag_init = 0;

void init_table()
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;

    printf("Starting Program...\r\n");

    ret = sqlite3_get_table(handle,
                            "CREATE TABLE user (userid INTEGER PRIMARY KEY ASC, pw TEXT, nickname TEXT);"                                   // create user table
                            "CREATE TABLE song (songid INTEGER PRIMARY KEY NOT NULL, songname TEXT, artistid INTEGER, genreid INTEGER);"    // create song table
                            "CREATE TABLE playlist (playlist INTEGER PRIMARY KEY ASC, userid INTEGER, songid INTEGER, difficulty INTEGER);" // create playlist table
                            "CREATE TABLE genre (genreid INTEGER PRIMARY KEY ASC, genrename TEXT);"                                         // create genre table
                            "CREATE TABLE artist (artistid INTEGER PRIMARY KEY ASC, artistname TEXT);"                                      // create artist table
                            "CREATE TABLE song_artist (said INTEGER PRIMARY KEY ASC, songid INTEGER, artistid INTEGER);"                    // create song-artist link table
                            ,
                            &results, &rows, &columns, &errMsg);

    if (ret == SQLITE_OK)
    {
        flag_init = 1;
        sqlite3_free_table(results);
        printf("Initialize Done!\r\n");
    }
    else
    {
        printf("Initialize Error!\r\n");
    }
}

void add_user(char *username, char *pw)
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[80];

    if (flag_init == 1)
    {
        snprintf(buf, sizeof(buf), "SELECT * FROM user WHERE (nickname='%s');", username);
        ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
        if (ret == SQLITE_OK)
        {
            printf("Add User: %s\r\n", username);
            // printf("ret: %d, rows: %d, columns: %d\r\n", ret, rows, columns);

            if (rows == 0)
            {
                snprintf(buf, sizeof(buf), "INSERT into user (nickname, pw) values('%s', '%s');", username, pw);
                ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
                if (ret == SQLITE_OK)
                    printf("Welcome! %s!\r\n", username);
                else
                    printf("Add User Failed!\r\n");
            }
            else
            {
                printf("%s is already taken!\r\n", username);
            }
            sqlite3_free_table(results);
        }
        else
            printf("Add User Failed!\r\n");
    }
}

void add_song(int songid, char *songname, char *artistname, char *genrename)
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[80];

    if (flag_init == 1)
    {
        snprintf(buf, sizeof(buf), "SELECT * FROM user WHERE (nickname='%s');", username);
        ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
        if (ret == SQLITE_OK)
        {
            printf("Add User: %s\r\n", username);
            // printf("ret: %d, rows: %d, columns: %d\r\n", ret, rows, columns);

            if (rows == 0)
            {
                snprintf(buf, sizeof(buf), "INSERT into user (nickname, pw) values('%s', '%s');", username, pw);
                ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
                if (ret == SQLITE_OK)
                    printf("Welcome! %s!\r\n", username);
                else
                    printf("Add User Failed!\r\n");
            }
            else
            {
                printf("%s is already taken!\r\n", username);
            }
            sqlite3_free_table(results);
        }
        else
            printf("Add User Failed!\r\n");
    }
}

int main()
{
    printf("Start\r\n");

    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;

    sqlite3_open("test.db", &handle);
    init_table();

    add_user("kestrel", "test");

    return 0;
}