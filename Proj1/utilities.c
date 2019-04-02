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