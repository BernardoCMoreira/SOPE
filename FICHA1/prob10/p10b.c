#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

int main(int argc, char *argv[]){

    char string[MAX]; 
    char aux[6]= {'-', '/', '*', '>', '<', '.'};
    char *result;
    char *pass=NULL;

    fgets(string,MAX,stdin);

    result = strtok(string,aux);

    while (result != NULL && strcmp(result,"\n")!=0){
       // printf("\n%s\n",result);
        pass=result;
        result=strtok(NULL,aux);
    }
    printf("\n%s\n",pass);
    return 0;
}