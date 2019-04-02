#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]){
    int number1, number2;
    int i=1;

    number1= atoi(argv[1]);
    number2= atoi(argv[2]);

    time_t t;
    srand((unsigned) time(&t));

       while(1){
           
        if (rand()%number1 == number2){
            printf ("It: %d ->  %d  \n",i,number2);
            break;
        }
        else {
             printf ("It: %d ->  %d  \n",i,rand()%number1);
        }
        i++;
       }
    return 0;
}