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

#define BUFFER_SIZE 256

int main(int argc, char **argv, char **envp)
{
    //Read input from one file, and copy to another!
    int option = 0;
    bool rFlag = false;
    DIR *d;
    struct dirent *dir;
    bool hFlag = false;
    bool dFlag = false;
    //bool mda5Flag = false, m1Flag = false, m256Flag = false;
    bool oFlag = false;
    bool vFlag = false;
    char *inputFile = argv[argc - 1];
    char *outputFile = NULL;

    printf("\n--------------Environment Variables-------------------\n");
    char **env;
    for (env = envp; *env != 0; env++)
    {
        char *thisEnv = *env;
        printf(" %s\n", thisEnv);
    }

    printf("\n----------------Options--------------------\n");
    while ((option = getopt(argc, argv, "rh:m,1,2o:f:vd")) != -1)
    {
        switch (option)
        {
        case 'r':
            rFlag = true;
            break;
        case 'h':
            hFlag = true;
            break;
        case 'o':
            oFlag = true;
            outputFile = optarg;
            break;
        case 'v':
            vFlag = true;
            break;
        case 'd':
            dFlag = true;
        case '?':
            fprintf(stderr, "Unrecognized option: -%c\n", optopt);
            break;
        }
    }

    if (rFlag == true)
    {
        printf("\n......-r INSTRUCTION......\n");
        d = opendir(inputFile);
        if (d)
        {
            while ((dir = readdir(d)) != NULL)
            {
                printf("%s\n", dir->d_name);
            }
            closedir(d);
        }
    }
    if (hFlag == true)
    {
        printf("\n......-h INSTRUCTION......\n");
        printf("H\n");
    }
    if (oFlag == true)
    {
        printf("\n......-o INSTRUCTION......\n");
        outfile_function(outputFile, inputFile);
        printf("Copying file %s to:-----> %s \n", inputFile, outputFile);
    }
    if (vFlag == true)
    {
        printf("\n......-v INSTRUCTION......\n");
    }
    if (dFlag == true)
    {
        printf("\n......-d INSTRUCTION......\n");
        printf("Output file : %s\n", outputFile);
    }
    printf("\n ............Input Size:%d..............\n", argc);

    //if (S_ISDIR()) // check if is directory

    struct stat fileStat;
    if (stat(argv[argc - 1], &fileStat) < 0)
        return 1;
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
    printf("File created date: \t\t %s", ctime(&fileStat.st_birthtime));
    printf("File modification date: \t %s ", ctime(&fileStat.st_mtime));
    printf("\n");

    return 0;
}