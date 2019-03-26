#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#define BUFFER_SIZE 256

// int outfile_function(char *name, char *readFile)
// {
//     FILE *fp;
//     FILE *fp2;
//     fp = fopen(name, "w");
//     fp2 = fopen(readFile, "r");

//     char buffer[BUFFER_SIZE];
//     ssize_t num_chars;
//     while ((num_chars = read(fp2, buffer, BUFFER_SIZE)) > 0)
//     {
//         write(fp, buffer, num_chars);
//     }
//     close(fp);
//     close(fp2);

//     return 0;
// }

int main(int argc, char **argv, char **env)
{
    //outfile_function("f2.txt", "f1.txt");

    int option = 0;
    bool rFlag = false;
    bool hFlag = false, mda5Flag = false, 1Flag = false, 256Flag = false;
    bool oFlag = false, outfileFlag = false;
    bool vFlag = false;
    bool filedirFlag = false;

    while ((option = getopt(argc, argv, "rh:m12:o:f:vd")) != -1)
    {
        switch (option)
        {
        case 'r': //rFlag = true;
            printf("rFlag ON!!\n");
            break;

        case 'h': //hFlag = true;
            printf("hFlag ON!!\n");
            break;

        case 'o':
            printf("oFlag ON\n");
            break;
        }
        printf("%d\n", argc);
    }
    return 0;
}