#include <stdio.h>
#include <stdlib.h>

void handler1(void){

    puts("Exit handler 1.");
}

void handler2(void){

    puts("Exit handler 2.");
}

int main ()
{

  atexit(handler1);
  atexit(handler2);

  //abort();
  puts ("Main function.");
  
  return 0;
}