// PROGRAMA p6a.c
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #define BUF_LENGTH 256
    #define MAX 256

    int main(void)
    {
      FILE *src, *dst;
      char buf[BUF_LENGTH];

      if ( ( src = fopen( "infile.txt", "r" ) ) == NULL )
      {
        perror("Exit1: ");
        printf("Error: %s\n", strerror(1));
        exit(1);
      }

      if ( ( dst = fopen( "outfile.txt", "w" ) ) == NULL )
      {
        perror("Exit2: ");
        printf("Error: %s\n", strerror(2)); 
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