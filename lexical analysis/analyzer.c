#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include "preprocessor_lexanalyzer.h"

int symbol_state_handler(char buffer[], int it, int op_count, FILE* tokens_filehandle, int return_value);
int identifier_char_state_handler(char buffer[], int it, FILE* tokens_filehandle, int return_value);


int file_rollback_error_handler(char* filename_to_delete)
{
  printf("\nError detected!");
  printf("\nDeleting created files...");

  // defining variable for string formatting ; stores file name of token file
  char token_file_name[10];
  sprintf(token_file_name, "%s", filename_to_delete);
  if(remove("intermediate_file.py") || remove(token_file_name)){
    printf("\nError deleting created files...");
    return -1;
  }
  else{
    printf("\nFiles deleted sucessfully...rollback successful");
  }

  return 0;
}





bool is_identifier_char(char c){
  return (c >= 65 && c <= 90) || (c >= 97 && c <= 122)
   || (c >= 48 && c <= 57) || (c == 95) ? true : false;
}

bool is_operator_char(char c){
  return (c >= 33 && c <= 47) || (c >= 58 && c <= 64) ? true : false;
}

int identifier_char_state_handler(char buffer[], int it, FILE* tokens_filehandle, int return_value)
{

  if(return_value == -1){
    return -1;
  }

  if(buffer[it] != '\0'){
    if(is_identifier_char(buffer[it])){
      fputc(buffer[it], tokens_filehandle);
      it++;
      identifier_char_state_handler(buffer, it, tokens_filehandle, return_value);
    }
    else if(is_operator_char(buffer[it])){
      fputs("\n", tokens_filehandle);
      int op_count = 0;
      // if not an identifier character
      symbol_state_handler(buffer, it, op_count, tokens_filehandle, return_value);
    }

    // 2 characters cannot exist with only a space bw them
    else if(buffer[it] == ' ' && is_identifier_char(buffer[it + 1])){

      printf("\nreturning -1 to main function");
      return_value = -1;
    }
    else{
      it++;
      identifier_char_state_handler(buffer, it, tokens_filehandle, return_value);
    }
  }
  return 0;

}

int symbol_state_handler(char buffer[], int it, int op_count, FILE* tokens_filehandle, int return_value){

  if(buffer[it] != '\0'){
    if(is_operator_char(buffer[it])){
      op_count++;
      if(op_count > 3){
        printf("\nError! Cannot have more than 3 operators in an expression");
        return -1;
      }
      fputc(buffer[it], tokens_filehandle);
      it++;
      symbol_state_handler(buffer, it, op_count, tokens_filehandle);
    }
    else if(is_identifier_char(buffer[it])){
      fputs("\n", tokens_filehandle);
      identifier_char_state_handler(buffer, it, tokens_filehandle, return_value);
    }
    else{
      it++;
      symbol_state_handler(buffer, it, 0, tokens_filehandle);
    }
  }

  return 0;
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

  int return_value = 0;

  while(fgets(buffer, 100, intermediate_filehandle)!=NULL){
    if(is_identifier_char(buffer[it])){
      int char_state_handler_result = identifier_char_state_handler(buffer, it, tokens_filehandle, return_value);
      if(char_state_handler_result == -1 ){
        printf("\nrolling back file creation");
        file_rollback_error_handler(argv[2]);
      }
    }
    else if(is_operator_char(buffer[it])){
      int symbol_state_handler_result = symbol_state_handler(buffer, it, op_count, tokens_filehandle);
      if(symbol_state_handler_result == -1){
        file_rollback_error_handler(argv[2]);
      }
    }
    fputs("\n", tokens_filehandle);
  }
}
