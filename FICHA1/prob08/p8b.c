#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h> 
#include <unistd.h> 

int main(int argc, char *argv[]){
    int number1, number2;
    int i=1;

    clock_t start, end;
    struct tms ts;
    long ticks; 

    number1= atoi(argv[1]);
    number2= atoi(argv[2]);

    start = times(&ts); /* início da medição de tempo */
    ticks = sysconf(_SC_CLK_TCK); 

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
        end = times(&ts); /* fim da medição de tempo */
        printf("Clock: %4.2f s\n", (double)(end-start)/ticks);
        printf("User time: %4.2f s\n", (double)ts.tms_utime/ticks);
        printf("System time: %4.2f s\n", (double)ts.tms_stime/ticks);
        printf("Children user time: %4.2f s\n", (double)ts.tms_cutime/ticks);
        printf("Children system time: %4.2f s\n", (double)ts.tms_cstime/ticks); 
    return 0;
}