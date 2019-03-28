#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#define BUFFER_SIZE 256

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

int main(int argc, char **argv, char **env)
{
    //Read input from one file, and copy to another!
    outfile_function("f1.txt", "f2.txt");

    int option = 0;
    // bool rFlag = false;
    DIR *d;
    struct dirent *dir;
    // bool hFlag = false, mda5Flag = false, 1Flag = false, 256Flag = false;
    // bool oFlag = false, outfileFlag = false;
    // bool vFlag = false;
    // bool filedirFlag = false;

    while ((option = getopt(argc, argv, "rh:m12:o:f:vd")) != -1)
    {
        switch (option)
        {
        case 'r': //rFlag = true;

            printf("rFlag ON!!\n");
            d = opendir(".");
            if (d)
            {
                while ((dir = readdir(d)) != NULL)
                {
                    printf("%s\n", dir->d_name);
                }
                closedir(d);
            }
            break;

            // case 'h': //hFlag = true;
            //     printf("hFlag ON!!\n");
            //     break;

            // case 'o':
            //     printf("oFlag ON\n");
            //     break;
        }
        printf("Input Size:%d\n", argc);
    }
    return 0;
}