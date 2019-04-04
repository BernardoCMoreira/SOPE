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
    // DIR *d;
    // struct dirent *dir;
    bool hFlag = false;
    bool dFlag = false;
    bool md5Flag = false, sha1Flag = false, sha256Flag = false;
    bool oFlag = false;
    bool vFlag = false;
    char *inputFile = argv[argc - 1];
    char *outputFile = NULL;
    char *hString = NULL;

    // printf("\n--------------Environment Variables-------------------\n");
    char **env;
    for (env = envp; *env != 0; env++)
    {
        //char *thisEnv = *env;
        //printf(" %s\n", thisEnv);
    }

    //printf("\n----------------Options--------------------\n");
    while ((option = getopt(argc, argv, "rh:m,1,2o:f:vd")) != -1)
    {
        switch (option)
        {
        case 'r':
            rFlag = true;
            break;
        case 'h':
            hFlag = true;
            hString = optarg;
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
        checkDirectorys(inputFile);
    }
    if (hFlag == true)
    {
        printf("\n......-h INSTRUCTION......\n");

        if (strcmp(hString, " md5") == 0)
            md5Flag = true;
        else if (strcmp(hString, " sha1") == 0)
            sha1Flag = true;
        else if (strcmp(hString, " sha256") == 0)
            sha256Flag = true;

        char *x;
        char s[2] = ",";
        x = strtok(hString, s);
        if (strcmp(x, "md5") == 0)
        {
            md5Flag = true;
        }
        else if (strcmp(x, "sha1") == 0)
        {
            sha1Flag = true;
        }
        else if (strcmp(x, "sha256") == 0)
        {
            sha256Flag = true;
        }

        while ((x = strtok(NULL, s)))
        {
            if (strcmp(x, "md5") == 0)
                md5Flag = true;
            else if (strcmp(x, "sha1") == 0)
                sha1Flag = true;
            else if (strcmp(x, "sha256") == 0)
                sha256Flag = true;
        }
    }
    if (oFlag == true)
    {
        printf("\nData saved on file %s \n", outputFile);
        printf("Execution records saved on file ...\n");
        freopen(outputFile, "w", stdout);
        file_Info(*argv);
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

    if (argc == 2)
    {
        file_Info(inputFile);
    }

    if (md5Flag)
    {
        printf("\n.......MD5FLAG.......\n");
        get_MD5(argv[argc - 1], "md5 ");
    }

    if (sha1Flag)
    {
        printf("\n.......SHA1FLAG.......\n");
        get_SHA1(argv[argc - 1], "shasum -a 1 ");
    }

    if (sha256Flag)
    {
        printf("\n.......SHA256FLAG.......\n");
        get_SHA256(argv[argc - 1], "shasum -a 256 ");
    }

    return 0;
}
