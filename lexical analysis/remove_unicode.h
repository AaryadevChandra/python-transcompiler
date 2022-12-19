#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<string.h>

void remove_unicode(char* intermediate_file_name)
{
  FILE* file = fopen(intermediate_file_name, "r+");
  FILE* runicodefile = fopen("intermediate_file2.py", "a+");
  char buffer[100];
  while(fgets(buffer, 100, file)!=NULL){
    int it = 0;
    while(buffer[it] >= 21 && buffer[it] <= 125){
      fputc(buffer[it], runicodefile);
      it++;
    }
    fputc('\n', runicodefile);
  }

  fclose(file);
  fclose(runicodefile);
}
