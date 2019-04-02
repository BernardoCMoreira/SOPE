// PROGRAMA p6a.c
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #define BUF_LENGTH 256
    #define MAX 256

    int main(int argc, char *argv[])
    {
      FILE *src, *dst;
      char buf[BUF_LENGTH];
      
      if (argv[1] == NULL || argv[2] == NULL){
          printf("usage: nome_do_executável file1 file2");
          exit(1);
      }
      if ( ( src = fopen( argv[1], "r" ) ) == NULL )
      {
        printf("Error1: %s !\n",strerror(1));
        exit(1);
      }

      if ( ( dst = fopen( argv[2], "w" ) ) == NULL )
      {
        printf("Error1: %s !\n",strerror(2));
        exit(2);
      }

      while( ( fgets( buf, MAX, src ) ) != NULL )
      {
        fputs( buf, dst );
      }

      fclose( src );
      fclose( dst );
      exit(0);  // zero é geralmente indicativo de "sucesso"
}