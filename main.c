#include <stdio.h>
#include <termio.h>
#include "sqlite3.h"

sqlite3 *handle;
int flag_init = 0;

int getch(void)
{
    int ch;

    struct termios old;
    struct termios new;

    tcgetattr(0, &old);

    new = old;
    new.c_lflag &= ~(ICANON | ECHO);
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;

    tcsetattr(0, TCSAFLUSH, &new);
    ch = getchar();
    tcsetattr(0, TCSAFLUSH, &old);
}

// DB Functions

int check_DB()
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;

    // DB check
    ret = sqlite3_get_table(handle, "SELECT * from sqlite_master WHERE NAME = 'user';", &results, &rows, &columns, &errMsg);
    // printf("%d", atoi(results[1]));
    if (atoi(results[1]))
    {
        return 1;
    }
    ret = sqlite3_get_table(handle, "SELECT * from sqlite_master WHERE NAME = 'song';", &results, &rows, &columns, &errMsg);
    // printf("%d", atoi(results[1]));
    if (atoi(results[1]))
    {
        return 1;
    }
    ret = sqlite3_get_table(handle, "SELECT * from sqlite_master WHERE NAME = 'playlist';", &results, &rows, &columns, &errMsg);
    // printf("%d", atoi(results[1]));
    if (atoi(results[1]))
    {
        return 1;
    }
    ret = sqlite3_get_table(handle, "SELECT * from sqlite_master WHERE NAME = 'genre';", &results, &rows, &columns, &errMsg);
    // printf("%d", atoi(results[1]));
    if (atoi(results[1]))
    {
        return 1;
    }
    ret = sqlite3_get_table(handle, "SELECT * from sqlite_master WHERE NAME = 'artist';", &results, &rows, &columns, &errMsg);
    // printf("%d", atoi(results[1]));
    if (atoi(results[1]))
    {
        return 1;
    }
    ret = sqlite3_get_table(handle, "SELECT * from sqlite_master WHERE NAME = 'song_artist';", &results, &rows, &columns, &errMsg);
    // printf("%d\r\n", atoi(results[1]));
    if (atoi(results[1]))
    {
        return 1;
    }

    return 0;
}
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

    if (ret == SQLITE_OK || check_DB() == 0)
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
int add_user(char *username, char *pw)
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
            printf("Add user: %s\r\n", username);
            // printf("ret: %d, rows: %d, columns: %d\r\n", ret, rows, columns);

            if (rows == 0)
            {
                snprintf(buf, sizeof(buf), "INSERT into user (nickname, pw) values('%s', '%s');", username, pw);
                ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
                if (ret == SQLITE_OK)
                {
                    printf("Welcome! %s!\r\n", username);
                    return 0;
                }
                else
                {
                    printf("Add user failed!\r\n");
                    sqlite3_free_table(results);
                    return -1;
                }
            }
            else
            {
                printf("%s is already taken!\r\n", username);
                sqlite3_free_table(results);
                return -1;
            }
            sqlite3_free_table(results);
        }
        else
        {
            printf("Add user failed!\r\n");
            sqlite3_free_table(results);
            return -1;
        }
    }
}
int login(char *username, char *pw)
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[80];
    int userid = 0;

    if (flag_init == 1)
    {
        snprintf(buf, sizeof(buf), "SELECT * FROM user WHERE (nickname='%s') AND (pw='%s');", username, pw);
        ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
        if (ret == SQLITE_OK)
        {
            if (rows == 1)
            {
                userid = atoi(results[columns]);
                printf("Welcome! %s!\r\n", username);
                return userid;
            }
            else if (rows == 0)
            {
                printf("There is no such user!\r\n");
                sqlite3_free_table(results);
                return -1;
            }
            else
            {
                printf("Login Error!\r\n");
                sqlite3_free_table(results);
                return -1;
            }
            sqlite3_free_table(results);
        }
        else
        {
            printf("Login Error!\r\n");
            sqlite3_free_table(results);
            return -1;
        }
    }
}
void add_genre(char *genrename)
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[80];

    if (flag_init == 1)
    {
        snprintf(buf, sizeof(buf), "SELECT * FROM genre WHERE (genrename='%s');", genrename);
        ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
        if (ret == SQLITE_OK)
        {
            printf("Add genre: %s\r\n", genrename);
            // printf("ret: %d, rows: %d, columns: %d\r\n", ret, rows, columns);

            if (rows == 0)
            {
                snprintf(buf, sizeof(buf), "INSERT into genre (genrename) values('%s');", genrename);
                ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
                if (ret == SQLITE_OK)
                    printf("Genre %s successfully added!\r\n", genrename);
                else
                    printf("Add genre failed!\r\n");
            }
            else
            {
                printf("%s is already registered!\r\n", genrename);
            }
            sqlite3_free_table(results);
        }
        else
            printf("Add genre failed!\r\n");
    }
}
void add_artist(char *artistname)
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[80];

    if (flag_init == 1)
    {
        snprintf(buf, sizeof(buf), "SELECT * FROM artist WHERE (artistname='%s');", artistname);
        ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
        if (ret == SQLITE_OK)
        {
            printf("Add artist: %s\r\n", artistname);
            // printf("ret: %d, rows: %d, columns: %d\r\n", ret, rows, columns);

            if (rows == 0)
            {
                snprintf(buf, sizeof(buf), "INSERT into artist (artistname) values('%s');", artistname);
                ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
                if (ret == SQLITE_OK)
                    printf("Artist %s successfully added!\r\n", artistname);
                else
                    printf("Add artist failed!\r\n");
            }
            else
            {
                printf("%s is already registered!\r\n", artistname);
            }
            sqlite3_free_table(results);
        }
        else
            printf("Add genre failed!\r\n");
    }
}
int add_song(int songid, char *songname)
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[80];
    int i = 0;

    if (flag_init == 1)
    {
        snprintf(buf, sizeof(buf), "SELECT * FROM song WHERE (songid='%d');", songid);
        ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
        if (ret == SQLITE_OK)
        {
            if (rows == 0)
            {
                snprintf(buf, sizeof(buf), "INSERT into song (songid, songname) values('%d', '%s');", songid, songname);
                ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
                if (ret == SQLITE_OK)
                {
                    printf("%d: %s successfully added!\r\n", songid, songname);
                    sqlite3_free_table(results);
                    return 0;
                }
                else
                {
                    printf("Add song failed!\r\n");
                    sqlite3_free_table(results);
                    return 1;
                }
            }
            else
            {
                printf("%s is already registered!\r\n", songname);
                sqlite3_free_table(results);
                return 1;
            }
        }
        else
        {
            printf("Add song failed!\r\n");
            sqlite3_free_table(results);
            return 1;
        }
    }
}
void list_table(char *tablename)
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[80];

    snprintf(buf, sizeof(buf), "SELECT * FROM %s;", tablename);
    ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
    if (ret == SQLITE_OK)
    {
        if (rows == 0)
            printf("Data Not Found\r\n");
        else
        {
            for (int i = 0; i <= rows; i++)
            {
                for (int j = 0; j < columns; j++)
                {
                    printf("%s\t", results[i * columns + j]);
                }
                if (i == 0)
                    printf("\r\n==============================================\r\n");
                else
                    printf("\r\n");
            }
        }
        sqlite3_free_table(results);
    }
}

// Interraction Functions

void add_music_func(int userid)
{
    int songid;
    char songname[50];
    char artistname[16];
    char genrename[16];
    int genreid;
    int count = 0;
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[80];

    printf("Music ID (TJ) : ");
    scanf("%d", &songid);
    printf("Music Name : ");
    scanf("%s", songname);
    if (!add_song(songid, songname))
    {
        printf("Artist Name (enter - to stop): ");
        do
        {
            scanf("%s", artistname);
            add_artist(artistname);
            count++;
        } while (artistname[0] != '-' && count < 5);

        printf("Genre Name : ");
        scanf("%s", genrename);
        add_genre(genrename);
        snprintf(buf, sizeof(buf), "SELECT * FROM genre WHERE (genrename='%s');", genrename);
        ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
        if (ret == SQLITE_OK)
        {
            genreid = atoi(results[columns]);
            sqlite3_free_table(results);
        }
    }

    snprintf(buf, sizeof(buf), "INSERT into playlist (userid, songid, ) values('%d', '%d');", userid, songid);
    ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);

}
/*void search_music_func(int userid)
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[80];
    int i = 0;

    if (flag_init == 1)
    {
        snprintf(buf, sizeof(buf), "SELECT * FROM song WHERE (songid='%d');", songid);
        ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
        if (ret == SQLITE_OK)
        {
            if (rows == 0)
            {
                snprintf(buf, sizeof(buf), "INSERT into song (songid, songname) values('%d', '%s');", songid, songname);
                ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
                if (ret == SQLITE_OK)
                {
                    printf("%d: %s successfully added!\r\n", songid, songname);
                    sqlite3_free_table(results);
                    return 0;
                }
                else
                {
                    printf("Add song failed!\r\n");
                    sqlite3_free_table(results);
                    return 1;
                }
            }
            else
            {
                printf("%s is already registered!\r\n", songname);
                sqlite3_free_table(results);
                return 1;
            }
        }
        else
        {
            printf("Add song failed!\r\n");
            sqlite3_free_table(results);
            return 1;
        }
    }
}*/
void rate_difficulty_func(int userid)
{
}
void recommend_music_func(int userid)
{
}

int main()
{
    printf("Start\r\n");

    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    int userid = 0;

    int select_login = 0;
    int select_menu = 0;

    struct termios old;
    struct termios new;

    tcgetattr(0, &old);

    new = old;
    new.c_lflag &= ~(ICANON | ECHO);
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;

    sqlite3_open("test.db", &handle);
    if (handle)
        printf("DB Opened!\r\n");
    else
    {
        printf("DB Open Error\r\n");
        return -1;
    }
    init_table();

    printf("\r\nWelcome to the SRS (Song Recommendation System)\r\n\r\n");

LOGIN_SELECT:

    printf("Type 0(Join) or 1(Login) : ");
    scanf("%d", &select_login);
    int i = 0;
    if (select_login == 0) // user join
    {
        char username_new[16], pw_new[32];
        printf("Username : ");
        scanf("%s", username_new);
        printf("Password : ");
        scanf("%s", pw_new);
        add_user(username_new, pw_new);
        goto LOGIN_SELECT;
    }
    else if (select_login == 1)
    {
        char username_new[16], pw_new[32];
        printf("Username : ");
        scanf("%s", username_new);
        printf("Password : ");
        tcsetattr(0, TCSAFLUSH, &new);
        scanf("%s", pw_new);
        tcsetattr(0, TCSAFLUSH, &old);
        userid = login(username_new, pw_new);
        if (userid == -1)
            goto LOGIN_SELECT;
    }
    else
    {
        printf("Wrong menu selected!, Try again\r\n");
        goto LOGIN_SELECT;
    }

    while (1)
    {
        select_menu = 0;
        if (userid > 0)
        {
            system("clear");
            printf("1) ADD Music\r\n2) SEARCH Music\r\n3) RATE Difficulty of Music\r\n4) Music Recommendation\r\n0) Exit Program\r\nChoose Menu : ");
            scanf("%d", &select_menu);
            switch (select_menu)
            {
            case 1:
                add_music_func(userid);
                break;
            case 2:
                search_music_func(userid);
                break;
            case 3:
                rate_difficulty_func(userid);
                break;
            case 4:
                recommend_music_func(userid);
                break;
            case 0:
                return 0;
            default:
                printf("Wrong menu selected!, Try again\r\n");
                break;
            }
        }
    }
}