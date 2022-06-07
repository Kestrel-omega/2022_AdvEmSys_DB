#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termio.h>
#include "sqlite3.h"
#include <openssl/md5.h>

sqlite3 *handle;
int flag_init = 0;

// MD5 Hash Function

char *str2md5(const char *str, int length)
{
    int n;
    MD5_CTX c;
    unsigned char digest[16];
    char *out = (char *)malloc(33);

    MD5_Init(&c);

    while (length > 0)
    {
        if (length > 512)
        {
            MD5_Update(&c, str, 512);
        }
        else
        {
            MD5_Update(&c, str, length);
        }
        length -= 512;
        str += 512;
    }

    MD5_Final(digest, &c);

    for (n = 0; n < 16; ++n)
    {
        snprintf(&(out[n * 2]), 16 * 2, "%02x", (unsigned int)digest[n]);
    }

    return out;
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
    // ret = sqlite3_get_table(handle, "SELECT * from sqlite_master WHERE NAME = 'song_artist';", &results, &rows, &columns, &errMsg);
    // // printf("%d\r\n", atoi(results[1]));
    // if (atoi(results[1]))
    // {
    //     return 1;
    // }

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
                            "CREATE TABLE user (userid INTEGER PRIMARY KEY ASC, pw TEXT, nickname TEXT);"                                            // create user table
                            "CREATE TABLE song (songid INTEGER PRIMARY KEY NOT NULL, songname TEXT, artistid INTEGER, genreid INTEGER);"             // create song table
                            "CREATE TABLE playlist (playlist INTEGER PRIMARY KEY ASC, userid INTEGER NOT NULL, songid INTEGER, difficulty INTEGER);" // create playlist table
                            "CREATE TABLE genre (genreid INTEGER PRIMARY KEY ASC, genrename TEXT);"                                                  // create genre table
                            "CREATE TABLE artist (artistid INTEGER PRIMARY KEY ASC, artistname TEXT);"                                               // create artist table
                            // "CREATE TABLE song_artist (said INTEGER PRIMARY KEY ASC, songid INTEGER, artistid INTEGER);"                    // create song-artist link table
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

int login(char *username, char *pw)
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[512];
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

int add_user(char *username, char *pw)
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[512];

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

int add_genre(char *genrename)
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[512];

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
                {
                    printf("Genre %s successfully added!\r\n", genrename);
                    sqlite3_free_table(results);
                    return 0;
                }
                else
                {
                    printf("Add genre failed!\r\n");
                    sqlite3_free_table(results);
                    return 1;
                }
            }
            else
            {
                printf("%s is already registered!\r\n", genrename);
                sqlite3_free_table(results);
                return 0;
            }
        }
        else
        {
            printf("Add genre failed!\r\n");
            sqlite3_free_table(results);
            return 1;
        }
    }
}

int add_artist(char *artistname)
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[512];

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
                {
                    printf("Artist %s successfully added!\r\n", artistname);
                    sqlite3_free_table(results);
                    return 0;
                }
                else
                {
                    printf("Add artist failed!\r\n");
                    sqlite3_free_table(results);
                    return 1;
                }
            }
            else
            {
                printf("%s is already registered!\r\n", artistname);
                sqlite3_free_table(results);
                return 0;
            }
        }
        else
        {
            printf("Add artist failed!\r\n");
            sqlite3_free_table(results);
            return 1;
        }
    }
}

int add_song(int songid, char *songname, int genreid, int artistid)
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[200];
    int i = 0;

    if (flag_init == 1)
    {
        snprintf(buf, sizeof(buf), "SELECT * FROM song WHERE (songid='%d');", songid);
        ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
        if (ret == SQLITE_OK)
        {
            printf("rows : %d\r\n", rows);
            if (rows == 0)
            {
                snprintf(buf, sizeof(buf), "INSERT into song (songid, songname, genreid, artistid) values('%d', '%s', '%d', '%d');", songid, songname, genreid, artistid);
                // printf(buf);
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

int add_playlist_id(int userid, int songid)
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[200];

    snprintf(buf, sizeof(buf), "SELECT * FROM playlist WHERE (userid='%d') AND (songid='%d')", userid, songid);
    ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
    if (ret == SQLITE_OK)
    {
        if (rows == 0)
        {
            snprintf(buf, sizeof(buf), "INSERT into playlist (userid, songid) values('%d', '%d');", userid, songid);
            ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
            if (ret != SQLITE_OK)
                printf("Add Playlist Error\r\n");
            else
                printf("Added!\r\n");
        }
        else
            printf("Already Added\r\n");
    }
    else
        printf("Search Error\r\n");
    sqlite3_free_table(results);

    return 0;
}

int add_playlist(int userid)
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[200];

    int songid;

    printf("Enter Music ID : ");
    scanf("%d", &songid);

    snprintf(buf, sizeof(buf), "SELECT * FROM playlist WHERE (userid='%d') AND (songid='%d')", userid, songid);
    ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
    if (ret == SQLITE_OK)
    {
        if (rows == 0)
        {
            snprintf(buf, sizeof(buf), "INSERT into playlist (userid, songid) values('%d', '%d');", userid, songid);
            ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
            if (ret != SQLITE_OK)
                printf("Add Playlist Error\r\n");
            else
                printf("Added!\r\n");
        }
        else
            printf("Already Added\r\n");
    }
    else
        printf("Search Error\r\n");
    sqlite3_free_table(results);

    return 0;
}

void list_table(char *tablename)
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[512];

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

int add_playlist_func(int userid)
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[512];
    int songid;
    printf("Music ID : ");
    scanf("%d", &songid);
    snprintf(buf, sizeof(buf), "SELECT * FROM song WHERE (songid='%d');", songid);
    ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
    if (ret == SQLITE_OK)
    {
        if (rows == 0)
        {
            printf("No Such Music in DB\r\n");
        }
        else if (rows == 1)
        {
            snprintf(buf, sizeof(buf), "SELECT * FROM playlist WHERE (songid='%d') AND (userid='%d');", songid, userid);
            ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
            if (ret == SQLITE_OK)
            {
                if (rows == 0)
                {
                    snprintf(buf, sizeof(buf), "INSERT into playlist (userid, songid) values('%d', '%d');", userid, songid);
                    ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
                    if (ret != SQLITE_OK)
                        printf("Add Playlist Error\r\n");
                    else
                        printf("Music Added!\r\n");
                }
                else
                    printf("Already Added\r\n");
            }
            else
                printf("Error\r\n");
        }
        else
            printf("Error\r\n");
    }
    else
        printf("Search Error\r\n");
    sqlite3_free_table(results);
}

int search_playlist_func(int userid)
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[512];
    int menu_search_playlist = 0;
    int songid;
    char songname[100];
    char artistname[32];
    char genrename[16];

    while (1)
    {
        printf("1) SEARCH by Number\r\n2) SEARCH by Title\r\n3) SEARCH by Artist\r\n4) SEARCH by Genre\r\n0) RETURN to Main Menu\r\nSelect Search Rule : ");
        scanf("%d", &menu_search_playlist);
        switch (menu_search_playlist)
        {
        case 1:
            printf("Music ID : ");
            scanf("%d", &songid);
            snprintf(buf, sizeof(buf), "SELECT user.nickname, song.songid, song.songname, artist.artistname, genre.genrename, playlist.difficulty FROM playlist LEFT OUTER JOIN user ON playlist.userid=user.userid LEFT OUTER JOIN song ON playlist.songid=song.songid LEFT OUTER JOIN genre ON song.genreid=genre.genreid LEFT OUTER JOIN artist ON song.artistid=artist.artistid WHERE (user.userid='%d') AND (song.songid='%d') ;", userid, songid);
            ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
            if (ret == SQLITE_OK)
            {
                if (rows == 0)
                {
                    printf("No Such Music\r\n");
                }
                else
                {
                    printf("No.%s : %s - %s, %s, Difficulty : %s\r\n", results[columns + 1], results[columns + 2], results[columns + 3], results[columns + 4], results[columns + 5]);
                    printf("\r\n");
                }
            }
            else
                printf("Error\r\n");

            sqlite3_free_table(results);
            break;

        case 2:
            printf("Music Title : ");
            scanf("%s", songname);
            snprintf(buf, sizeof(buf), "SELECT user.nickname, song.songid, song.songname, artist.artistname, genre.genrename, playlist.difficulty FROM playlist LEFT OUTER JOIN user ON playlist.userid=user.userid LEFT OUTER JOIN song ON playlist.songid=song.songid LEFT OUTER JOIN genre ON song.genreid=genre.genreid LEFT OUTER JOIN artist ON song.artistid=artist.artistid WHERE (user.userid='%d') AND (song.songname='%s') ;", userid, songname);
            ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
            if (ret == SQLITE_OK)
            {
                if (rows == 0)
                    printf("No Such Music\r\n");
                else
                {
                    for (int i = 1; i <= rows; i++)
                    {
                        printf("No.%s : %s - %s, %s, Difficulty : %s\r\n", results[i * columns + 1], results[i * columns + 2], results[i * columns + 3], results[i * columns + 4], results[i * columns + 5]);
                        printf("\r\n");
                    }
                }
            }

            sqlite3_free_table(results);
            break;

        case 3:
            printf("Artist : ");
            scanf("%s", artistname);
            snprintf(buf, sizeof(buf), "SELECT user.nickname, song.songid, song.songname, artist.artistname, genre.genrename, playlist.difficulty FROM playlist LEFT OUTER JOIN user ON playlist.userid=user.userid LEFT OUTER JOIN song ON playlist.songid=song.songid LEFT OUTER JOIN genre ON song.genreid=genre.genreid LEFT OUTER JOIN artist ON song.artistid=artist.artistid WHERE (user.userid='%d') AND (artist.artistname='%s') ;", userid, artistname);
            ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
            if (ret == SQLITE_OK)
            {
                if (rows == 0)
                    printf("No Such Music\r\n");
                else
                {
                    for (int i = 1; i <= rows; i++)
                    {
                        printf("No.%s : %s - %s, %s, Difficulty : %s\r\n", results[i * columns + 1], results[i * columns + 2], results[i * columns + 3], results[i * columns + 4], results[i * columns + 5]);
                        printf("\r\n");
                    }
                }
            }

            sqlite3_free_table(results);
            break;

        case 4:
            printf("Genre : ");
            scanf("%s", genrename);
            snprintf(buf, sizeof(buf), "SELECT user.nickname, song.songid, song.songname, artist.artistname, genre.genrename, playlist.difficulty FROM playlist LEFT OUTER JOIN user ON playlist.userid=user.userid LEFT OUTER JOIN song ON playlist.songid=song.songid LEFT OUTER JOIN genre ON song.genreid=genre.genreid LEFT OUTER JOIN artist ON song.artistid=artist.artistid WHERE (user.userid='%d') AND (genre.genrename='%s') ;", userid, genrename);
            ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
            if (ret == SQLITE_OK)
            {
                if (rows == 0)
                    printf("No Such Music\r\n");
                else
                {
                    for (int i = 1; i <= rows; i++)
                    {
                        printf("No.%s : %s - %s, %s, Difficulty : %s\r\n", results[i * columns + 1], results[i * columns + 2], results[i * columns + 3], results[i * columns + 4], results[i * columns + 5]);
                        printf("\r\n");
                    }
                }
            }

            sqlite3_free_table(results);
            break;

        case 0:
            return 0;
            break;

        default:
            printf("Wrong input\r\n");
            break;
        }
    }
}

void rate_difficulty(int userid)
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[512];
    int songid;
    int difficulty;

    printf("Music ID : ");
    scanf("%d", &songid);
    printf("Difficulty (1~10) : ");
    scanf("%d", &difficulty);
    if (difficulty > 10)
        difficulty = 10;
    else if (difficulty < 1)
        difficulty = 1;
    snprintf(buf, sizeof(buf), "UPDATE playlist SET difficulty='%d' WHERE (userid='%d') AND (songid='%d');", difficulty, userid, songid);
    ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
    if (ret == SQLITE_OK)
        printf("Successfully Rated\r\n");
    else
        printf("Error!\r\n");

    sqlite3_free_table(results);
}

void delete_song_playlist(int userid)
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[512];
    int songid;
    int difficulty;

    printf("Music ID : ");
    scanf("%d", &songid);
    snprintf(buf, sizeof(buf), "DELETE FROM playlist WHERE (userid='%d') AND (songid='%d');", userid, songid);
    ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
    if (ret == SQLITE_OK)
        printf("Successfully Deleted\r\n");
    else
        printf("Error!\r\n");

    sqlite3_free_table(results);
}

void see_all_playlist(int userid)
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[512];

    snprintf(buf, sizeof(buf), "SELECT user.nickname, song.songid, song.songname, artist.artistname, genre.genrename, playlist.difficulty FROM playlist LEFT OUTER JOIN user ON playlist.userid=user.userid LEFT OUTER JOIN song ON playlist.songid=song.songid LEFT OUTER JOIN genre ON song.genreid=genre.genreid LEFT OUTER JOIN artist ON song.artistid=artist.artistid WHERE (user.userid='%d');", userid);
    ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
    if (ret == SQLITE_OK)
    {
        if (rows == 0)
            printf("No Such Music\r\n");
        else
        {
            for (int i = 1; i <= rows; i++)
            {
                printf("No.%s : %s - %s, %s, Difficulty : %s\r\n", results[i * columns + 1], results[i * columns + 2], results[i * columns + 3], results[i * columns + 4], results[i * columns + 5]);
                printf("\r\n");
            }
        }
    }

    sqlite3_free_table(results);
}

// Main Menu Functions

void add_music_func() // number 1
{
    int songid;
    char songname[100];
    char artistname[16];
    char genrename[16];
    int genreid, artistid;
    int count = 0;
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[512];

    printf("Music ID (TJ) : ");
    scanf("%d", &songid);
    printf("Music Name : ");
    scanf("%s", songname);
    printf("Representative Artist Name: ");
    scanf("%s", artistname);
    if (!add_artist(artistname))
    {
        snprintf(buf, sizeof(buf), "SELECT * FROM artist WHERE (artistname='%s');", artistname);
        ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
        if (ret == SQLITE_OK)
        {
            artistid = atoi(results[columns]);
            sqlite3_free_table(results);
        }
        else
        {
            printf("Artist Load Failed\r\n");
            sqlite3_free_table(results);
        }
    }
    printf("Genre Name : ");
    scanf("%s", genrename);
    if (!add_genre(genrename))
    {
        snprintf(buf, sizeof(buf), "SELECT * FROM genre WHERE (genrename='%s');", genrename);
        ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
        if (ret == SQLITE_OK)
        {
            genreid = atoi(results[columns]);
            sqlite3_free_table(results);
        }
        else
        {
            printf("Genre Load Failed\r\n");
            sqlite3_free_table(results);
        }
    }
    add_song(songid, songname, genreid, artistid);
}

void remove_music_func() // number 2
{
    int songid;
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[512];

    printf("Enter the ID of the music you want to delete : ");
    scanf("%d", &songid);

    snprintf(buf, sizeof(buf), "SELECT * FROM song WHERE (songid='%d');", songid);
    ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
    if (ret == SQLITE_OK)
    {
        if (rows == 1)
        {
            snprintf(buf, sizeof(buf), "DELETE FROM song WHERE songid='%d';", songid);
            ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
            if (ret != SQLITE_OK)
                printf("Delete Music Error!\r\n");
            else
                printf("Song %d Succefully Removed from DB!\r\n\r\n", songid);
        }
        else
            printf("There is no such music\r\n");
    }
    else
        printf("Delete Music Error!\r\n");
    sqlite3_free_table(results);
}

int search_music_func(int userid) // number 3
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[512];

    int menu_search = 0;
    char flag_agree = '0';
    int songid;
    char title[100];
    char artist[32];
    char genre[16];

    while (1)
    {
        printf("1) SEARCH by Number\r\n2) SEARCH by Title\r\n3) SEARCH by Artist\r\n4) SEARCH by Genre\r\n0) Return to Main Menu\r\nSelect Search Rule : ");
        scanf("%d", &menu_search);
        switch (menu_search)
        {
        case 1:
            printf("Music ID : ");
            scanf("%d", &songid);
            snprintf(buf, sizeof(buf), "SELECT song.songid, song.songname, artist.artistname, genre.genrename FROM song LEFT OUTER JOIN artist ON song.artistid=artist.artistid LEFT OUTER JOIN genre ON song.genreid=genre.genreid WHERE song.songid IN ('%d');", songid);
            ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
            if (ret == SQLITE_OK)
            {
                if (rows == 0)
                {
                    printf("Music Not Found!\r\n");
                }
                else
                {
                    printf("No.%s : %s - %s, %s\r\n", results[columns], results[columns + 1], results[columns + 2], results[columns + 3]);
                    printf("\r\n");
                }
            }
            else
                printf("Error!\r\n");
            sqlite3_free_table(results);
            break;

        case 2:
            printf("Music Title : ");
            scanf("%s", title);
            snprintf(buf, sizeof(buf), "SELECT song.songid, song.songname, artist.artistname, genre.genrename FROM song LEFT OUTER JOIN artist ON song.artistid=artist.artistid LEFT OUTER JOIN genre ON song.genreid=genre.genreid WHERE song.songname IN ('%s');", title);
            ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
            if (ret == SQLITE_OK)
            {
                if (rows == 0)
                {
                    printf("Music Not Found!\r\n");
                }
                else
                {
                    for (int i = 1; i <= rows; i++)
                    {
                        printf("No.%s : %s - %s, %s\r\n", results[i * columns], results[i * columns + 1], results[i * columns + 2], results[i * columns + 3]);
                        printf("\r\n");
                    }
                }
            }
            else
                printf("Error!\r\n");
            sqlite3_free_table(results);
            break;

        case 3:
            printf("Artist : ");
            scanf("%s", artist);
            snprintf(buf, sizeof(buf), "SELECT song.songid, song.songname, artist.artistname, genre.genrename FROM song LEFT OUTER JOIN artist ON song.artistid=artist.artistid LEFT OUTER JOIN genre ON song.genreid=genre.genreid WHERE artist.artistname IN ('%s');", artist);
            ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
            if (ret == SQLITE_OK)
            {
                if (rows == 0)
                {
                    printf("Music Not Found!\r\n");
                }
                else
                {
                    for (int i = 1; i <= rows; i++)
                    {
                        printf("No.%s : %s - %s, %s\r\n", results[i * columns], results[i * columns + 1], results[i * columns + 2], results[i * columns + 3]);
                        printf("\r\n");
                    }
                }
            }
            else
                printf("Error!\r\n");
            sqlite3_free_table(results);
            break;

        case 4:
            printf("Genre : ");
            scanf("%s", genre);
            snprintf(buf, sizeof(buf), "SELECT song.songid, song.songname, artist.artistname, genre.genrename FROM song LEFT OUTER JOIN artist ON song.artistid=artist.artistid LEFT OUTER JOIN genre ON song.genreid=genre.genreid WHERE genre.genrename IN ('%s');", genre);
            ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
            if (ret == SQLITE_OK)
            {
                if (rows == 0)
                {
                    printf("Music Not Found!\r\n");
                }
                else
                {
                    for (int i = 1; i <= rows; i++)
                    {
                        printf("No.%s : %s - %s, %s\r\n", results[i * columns], results[i * columns + 1], results[i * columns + 2], results[i * columns + 3]);
                        printf("\r\n");
                    }
                }
            }
            else
                printf("Error!\r\n");
            sqlite3_free_table(results);
            break;

        case 0:
            return 0;
            break;
        default:
            printf("Wrong input\r\n");
            break;
        }
    }
}

int playlist_func(int userid) // number 4
{
    int menu_playlist = 0;
    while (1)
    {
        printf("1) Add Music to Playlist\r\n2) Search Music from Playlist\r\n3) Rate Difficulty of Music\r\n4) Delete Music from Playlist\r\n5) See All of Musics in Playlist\r\n0) Return to Main Menu\r\nSelect Option : ");
        scanf("%d", &menu_playlist);
        switch (menu_playlist)
        {
        case 1:
            add_playlist_func(userid);
            break;
        case 2:
            search_playlist_func(userid);
            break;
        case 3:
            rate_difficulty(userid);
            break;
        case 4:
            delete_song_playlist(userid);
            break;
        case 5:
            see_all_playlist(userid);
            break;
        case 0:
            return 0;
            break;
        default:
            printf("Wrong input\r\n");
            break;
        }
    }
}

int recommend_music_func(int userid) // number 5
{
    char *errMsg = NULL;
    char **results;
    int rows;
    int columns;
    int ret;
    char buf[512];
    int menu_recommend = 0;
    int difficulty = 0;
    int songid;
    char artistname[32];

    while (1)
    {
        printf("1) From All\r\n2) By Difficulty\r\n3) By Artist\r\n0) Return to Main Menu\r\nSelect SRS Rule : ");
        scanf("%d", &menu_recommend);
        switch (menu_recommend)
        {
        case 1:
            snprintf(buf, sizeof(buf), "SELECT songid FROM playlist WHERE (userid='%d') ORDER BY RANDOM() LIMIT 1;", userid);
            ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
            if (ret == SQLITE_OK)
            {
                songid = atoi(results[columns]);
                snprintf(buf, sizeof(buf), "SELECT user.nickname, song.songid, song.songname, artist.artistname, genre.genrename, playlist.difficulty FROM playlist LEFT OUTER JOIN user ON playlist.userid=user.userid LEFT OUTER JOIN song ON playlist.songid=song.songid LEFT OUTER JOIN genre ON song.genreid=genre.genreid LEFT OUTER JOIN artist ON song.artistid=artist.artistid WHERE (user.userid='%d') AND (song.songid='%d') ;", userid, songid);
                ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
                if (ret == SQLITE_OK)
                {
                    printf("No.%s : %s - %s, %s, Difficulty : %s\r\n", results[columns + 1], results[columns + 2], results[columns + 3], results[columns + 4], results[columns + 5]);
                    printf("\r\n");
                }
                else
                    printf("Error\r\n");

                sqlite3_free_table(results);
                break;
            }
            else
                printf("Error!\r\n");
            break;
        case 2:
            printf("Enter Difficulty : ");
            scanf("%d", &difficulty);
            snprintf(buf, sizeof(buf), "SELECT songid FROM playlist WHERE (difficulty='%d') AND (userid='%d') ORDER BY RANDOM() LIMIT 1;", difficulty, userid);
            ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
            if (ret == SQLITE_OK)
            {
                songid = atoi(results[columns]);
                snprintf(buf, sizeof(buf), "SELECT user.nickname, song.songid, song.songname, artist.artistname, genre.genrename, playlist.difficulty FROM playlist LEFT OUTER JOIN user ON playlist.userid=user.userid LEFT OUTER JOIN song ON playlist.songid=song.songid LEFT OUTER JOIN genre ON song.genreid=genre.genreid LEFT OUTER JOIN artist ON song.artistid=artist.artistid WHERE (user.userid='%d') AND (song.songid='%d') ;", userid, songid);
                ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
                if (ret == SQLITE_OK)
                {
                    printf("No.%s : %s - %s, %s, Difficulty : %s\r\n", results[columns + 1], results[columns + 2], results[columns + 3], results[columns + 4], results[columns + 5]);
                    printf("\r\n");
                }
                else
                    printf("Error\r\n");

                sqlite3_free_table(results);
                break;
            }
            else
                printf("Error!\r\n");
            break;
        case 3:
            printf("Enter Artist : ");
            scanf("%s", artistname);
            snprintf(buf, sizeof(buf), "SELECT playlist.songid FROM (playlist LEFT OUTER JOIN song ON playlist.songid=song.songid LEFT OUTER JOIN artist ON song.artistid=artist.artist.id) WHERE (artistname='%s') AND (userid='%d') ORDER BY RANDOM() LIMIT 1;", artistname, userid);
            ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
            if (ret == SQLITE_OK)
            {
                songid = atoi(results[columns]);
                snprintf(buf, sizeof(buf), "SELECT user.nickname, song.songid, song.songname, artist.artistname, genre.genrename, playlist.difficulty FROM playlist LEFT OUTER JOIN user ON playlist.userid=user.userid LEFT OUTER JOIN song ON playlist.songid=song.songid LEFT OUTER JOIN genre ON song.genreid=genre.genreid LEFT OUTER JOIN artist ON song.artistid=artist.artistid WHERE (user.userid='%d') AND (song.songid='%d') ;", userid, songid);
                ret = sqlite3_get_table(handle, buf, &results, &rows, &columns, &errMsg);
                if (ret == SQLITE_OK)
                {
                    printf("No.%s : %s - %s, %s, Difficulty : %s\r\n", results[columns + 1], results[columns + 2], results[columns + 3], results[columns + 4], results[columns + 5]);
                    printf("\r\n");
                }
                else
                    printf("Error\r\n");

                sqlite3_free_table(results);
                break;
            }
            else
                printf("Error!\r\n");
            break;
        case 0:
            return 0;
            break;
        default:
            printf("Wrong menu selected!, Try again\r\n");
            break;
        }
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

    printf("\r\nWelcome to the MRS (Music Recommendation System)\r\n\r\n");

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
        char *pw_md5 = str2md5(pw_new, strlen(pw_new));
        add_user(username_new, pw_md5);
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
        char *pw_md5 = str2md5(pw_new, strlen(pw_new));
        userid = login(username_new, pw_md5);
        if (userid == -1)
            goto LOGIN_SELECT;
    }
    else
    {
        printf("Wrong menu selected!, Try again\r\n");
        goto LOGIN_SELECT;
    }

    list_table("user");

    while (1)
    {
        select_menu = 0;
        if (userid > 0)
        {
            // system("clear");
            printf("\r\n");
            
            printf("1) ADD Music in DB\r\n2) REMOVE Music from DB\r\n3) SEARCH Music and Register Music to Playlist\r\n4) SEARCH Music from Playlist\r\n5) Music Recommendation\r\n0) Exit Program\r\nChoose Menu : ");
            scanf("%d", &select_menu);
            switch (select_menu)
            {
            case 1:
                add_music_func();
                break;
            case 2:
                remove_music_func();
                break;
            case 3:
                search_music_func(userid);
                break;
            case 4:
                playlist_func(userid);
                break;
            case 5:
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