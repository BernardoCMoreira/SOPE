#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "utilities.h"
#include <time.h>
#include <dirent.h>
#include <errno.h>
int outfile_function(char *readFile, char *writeFile)
{
    FILE *fp1;
    FILE *fp2;
    char c;

    fp1 = fopen(readFile, "r");
    if (fp1 == NULL)
    {
        printf("Cannot open file :%s\n", readFile);
        exit(0);
    }
    fp2 = fopen(writeFile, "w");

    if (fp2 == NULL)
    {
        printf("Cannot open file :%s\n", writeFile);
        exit(0);
    }

    c = fgetc(fp1);

    while (c != EOF)
    {
        fputc(c, fp2);
        c = fgetc(fp1);
    }

    fclose(fp1);
    fclose(fp2);

    return 0;
}

void get_Type(char *Name, char *cmnd)
{
    char str[50];
    strcpy(str, cmnd);
    strcat(str, Name);
    FILE *using = popen(str, "r");
    char buffer[100];
    fgets(buffer, 100, using);
    char *x;
    char s[2] = ":";
    x = strtok(buffer, s);
    x = strtok(NULL, s);
    printf("File Type: \t\t\t%s", x);
}

void file_Info(char **argv, int argc)
{
    struct stat fileStat;
    if (stat(argv[argc - 1], &fileStat) < 0)
        return;
    printf("Information for %s\n", argv[argc - 1]);
    printf("---------------------------\n");
    printf("File Name: \t\t\t %s \n", argv[argc - 1]);
    get_Type(argv[argc - 1], "file ");
    printf("File Size: \t\t\t %lld bytes\n", fileStat.st_size);
    printf("File Permissions: \t\t ");
    printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf("\n");
    char createdTime[40];
    strftime(createdTime, 40, "%FT%T", localtime(&fileStat.st_birthtime));
    printf("File created date: \t\t %s \n", createdTime);
    char modificationTime[40];
    strftime(modificationTime, 40, "%FT%T", localtime(&fileStat.st_mtime));
    printf("File modification date: \t %s ", modificationTime);
    printf("\n");
}

int isFile(const char *name)
{
    DIR *dir = opendir(name);

    if (dir != NULL)
    {
        closedir(dir);
        return 0;
    }

    if (errno == ENOTDIR)
    {
        return 1;
    }

    return -1;
}

int checkDirectorys(char *inputFile)
{

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(inputFile)) != NULL)
    {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL)
        {
            printf("%s\n", ent->d_name);

            struct stat s;
            fstat((int)ent->d_name, &s);

            // if ((s.st_mode & S_IFMT) == S_IFDIR)
            // { //if directory
            //     //if ()
            //     //{
            //     if (fork() == 0)
            //         checkDirectorys();
            //     exit(0);
            //     //}
            // }

            // if ()
            // { //normal file
            //     //print results
            // }
        }
        closedir(dir);
    }
    else
    {
        /* could not open directory */
        perror("");
        return EXIT_FAILURE;
    }
    return 0;
}

int get_MD5(char *Name, char *cmnd)
{
    char str[50];
    strcpy(str, cmnd);
    strcat(str, Name);
    FILE *using = popen(str, "r");
    char buffer[100];
    fgets(buffer, 100, using);
    char *x;
    char s[2] = "=";
    x = strtok(buffer, s);
    x = strtok(NULL, s);
    printf("File MD5 Value: \t\t\t%s", x);

    return 0;
}

int get_SHA1(char *Name, char *cmnd)
{
    char str[50];
    strcpy(str, cmnd);
    strcat(str, Name);
    FILE *using = popen(str, "r");
    char buffer[100];
    fgets(buffer, 100, using);
    char *x;
    char s[2] = " ";
    x = strtok(buffer, s);
    printf("File SHA1 Value: \t\t\t %s \n", x);

    return 0;
}
int get_SHA256(char *Name, char *cmnd)
{
    char str[50];
    strcpy(str, cmnd);
    strcat(str, Name);
    FILE *using = popen(str, "r");
    char buffer[100];
    fgets(buffer, 100, using);
    char *x;
    char s[2] = " ";
    x = strtok(buffer, s);
    printf("File SHA256 Value: \t\t\t %s \n", x);

    return 0;
}