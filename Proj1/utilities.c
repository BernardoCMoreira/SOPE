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

void file_Info(char *inputFile)
{
    struct stat fileStat;
    if (stat(inputFile, &fileStat) < 0)
        return;
    printf("---------------------------\n");
    printf("Information for %s\n", inputFile);
    printf("File Name: \t\t\t %s \n", inputFile);
    get_Type(inputFile, "file ");
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
    printf("\n\n");
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

void checkDirectorys(char *inputFile)
{

    DIR *dir = opendir(inputFile);
    struct dirent *ent;
    char path[1000];
    if (!dir)
        return;
    while ((ent = readdir(dir)) != NULL)
    {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0 || strcmp(ent->d_name, ".DS_Store") == 0)
        {
            continue;
        }

        printf("%s\n", ent->d_name);
        strcpy(path, inputFile);
        strcat(path, "/");
        strcat(path, ent->d_name);
        file_Info(path);
        checkDirectorys(path);
    }
    closedir(dir);
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
    printf("File MD5 Value: \t\t\t%s \n\n", x);

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
    printf("File SHA1 Value: \t\t\t %s \n\n", x);

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
    printf("File SHA256 Value: \t\t\t %s \n\n", x);

    return 0;
}
