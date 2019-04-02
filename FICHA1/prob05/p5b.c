#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv, char **envp)
{
printf("Hello ");
  for (char **env = envp; *env != 0; env++)
  {
    if (strncmp(*env,"USER=",5)==0){
    char *thisEnv = *env;
    for (int i=5; i<strlen(thisEnv); i++){
    printf("%c", thisEnv[i]);   
    } 
    printf("! \n");
    }
  }
  return 0;
}