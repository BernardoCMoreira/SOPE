#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv, char **envp)
{
 printf("Hello ");
  printf("%s ! \n", getenv("USER=")); 
  return 0;
}