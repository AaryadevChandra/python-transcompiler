#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include "preprocessor_lexanalyzer.h"

void symbol_state_handler(char buffer[], int it, int op_count, FILE* tokens_filehandle);
void  identifier_char_state_handler(char buffer[], int it, FILE* tokens_filehandle);

bool is_identifier_char(char c){
  return (c >= 65 && c <= 90) || (c >= 97 && c <= 122)
   || (c >= 48 && c <= 57) || (c == 95) ? true : false;
}

bool is_operator_char(char c){
  return (c >= 33 && c <= 47) || (c >= 58 && c <= 64) ? true : false;
}

void identifier_char_state_handler(char buffer[], int it, FILE* tokens_filehandle)
{
  printf("\nchecking value %c", buffer[it]);
  if(buffer[it] != '\0'){
    if(is_identifier_char(buffer[it])){
      printf("\nidentifier case was ran");
      printf("\nprinting chars now\n");
      printf("%c", buffer[it]);

      fputc(buffer[it], tokens_filehandle);
      it++;
      identifier_char_state_handler(buffer, it, tokens_filehandle);
    }
    else if(is_operator_char(buffer[it])){
      printf("\noperator case was ran");
      fputs("\n", tokens_filehandle);
      int op_count = 0;
      // if not an identifier character
      symbol_state_handler(buffer, it, op_count, tokens_filehandle);
    }
    else{
      it++;
      identifier_char_state_handler(buffer, it, tokens_filehandle);
    }
  }

}

void symbol_state_handler(char buffer[], int it, int op_count, FILE* tokens_filehandle){


  printf("\nchecking value %c", buffer[it]);
  if(buffer[it] != '\0'){
    if(is_operator_char(buffer[it])){
      printf("\noperator case was ran");
      op_count++;
      if(op_count > 3){
        printf("\nError! Cannot have more than 3 operators in an expression");
        return ;
      }
      printf("\nprinting symbols now\n");
      printf("%c", buffer[it]);
      fputc(buffer[it], tokens_filehandle);
      it++;
      symbol_state_handler(buffer, it, op_count, tokens_filehandle);
    }
    else if(is_identifier_char(buffer[it])){
      printf("\nidentifier case was ran");
      fputs("\n", tokens_filehandle);
      identifier_char_state_handler(buffer, it, tokens_filehandle);
    }
    else{
      it++;
      symbol_state_handler(buffer, it, 0, tokens_filehandle);
    }
  }
}

int main(int argc, char** argv)
{

  if(argv[1] == NULL || argv[2] == NULL) {
    printf("Please supply the correct no. of args");
    exit(1);
  }

  // p1.py
  lexical_preprocessing(argv[1]);

  FILE* intermediate_filehandle = fopen("intermediate_file.py", "r+");

  int op_count = 0;
  int it=0;
  char buffer[100];

  FILE* tokens_filehandle = fopen(argv[2], "a+");

  while(fgets(buffer, 100, intermediate_filehandle)!=NULL){
    if(is_identifier_char(buffer[it])){
      identifier_char_state_handler(buffer, it, tokens_filehandle);
    }
    else if(is_operator_char(buffer[it])){
      symbol_state_handler(buffer, it, op_count, tokens_filehandle);
    }
    fputs("\n", tokens_filehandle);
  }
}
