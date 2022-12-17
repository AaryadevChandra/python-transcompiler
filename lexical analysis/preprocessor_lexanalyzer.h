#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>

FILE* lexical_preprocessing(char* source_filename)
{

    // preprocessing beginning

  FILE* source_filehandle = fopen(source_filename, "r+");

  if(source_filehandle == NULL){
    printf("Filename supplied not found");
    exit(1);
  }

  FILE* intermediate_filehandle = fopen("intermediate_file.py", "a+");

  char source_buffer[100];
  char intermediate_buffer[100];

  int line_number= 1 ;

  while(fgets(source_buffer, 100, source_filehandle)!=NULL){

    int source_buffer_it= 0;
    int intermediate_buffer_it = 0;

    printf("\nline number %d", line_number);
    printf("\n%c ----> %d", source_buffer[0], source_buffer[0]);

    int stop_anayzing_flag = 0;

    while(source_buffer[source_buffer_it] != '\0'){
      if(source_buffer[source_buffer_it] == '#') break;
      if(source_buffer[source_buffer_it] == '\n') break;
      if(source_buffer[source_buffer_it] == '"' || source_buffer[source_buffer_it] == '\'') {
        stop_anayzing_flag = !stop_anayzing_flag;
      }
      if(source_buffer[source_buffer_it] != ' '){
        intermediate_buffer[intermediate_buffer_it] = source_buffer[source_buffer_it];
        intermediate_buffer_it++;
      }
      else if(source_buffer[source_buffer_it] == ' ' && stop_anayzing_flag == 0){
        intermediate_buffer[intermediate_buffer_it] = source_buffer[source_buffer_it];
        intermediate_buffer_it++;
      }
      else if(source_buffer[source_buffer_it] == ' ' && stop_anayzing_flag == 1 ){
        intermediate_buffer[intermediate_buffer_it] = source_buffer[source_buffer_it];
        intermediate_buffer_it++;
      }
      source_buffer_it++;
    }

    if(intermediate_buffer_it > 0){
      fputs(intermediate_buffer, intermediate_filehandle);
      fputs("\n", intermediate_filehandle);
      memset(intermediate_buffer, 0, sizeof intermediate_buffer);
    }

    line_number++;
  }
  fclose(intermediate_filehandle);
  fclose(source_filehandle);
}
