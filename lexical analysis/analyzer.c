#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include "preprocessor_lexanalyzer.h"

#define CURRENT_TOKEN_SIZE 100

void symbol_state_handler(char buffer[], int it, int op_count, FILE* tokens_filehandle, char curr_token[]);
void  identifier_char_state_handler(char buffer[], int it, FILE* tokens_filehandle, char curr_token[]);

bool is_identifier_char(char c){
  return (c >= 65 && c <= 90) || (c >= 97 && c <= 122)
   || (c >= 48 && c <= 57) || (c == 95) ? true : false;
}

bool is_operator_char(char c){
  return (c >= 33 && c <= 47) || (c >= 58 && c <= 64) ? true : false;
}

int is_keyword(char curr_token[])
{
  char keywords[36][20] = {
    "False",
    "None",
    "True",
    "__peg_parser__",
    "and",
    "as",
    "assert",
    "async",
    "await",
    "break",
    "class",
    "continue",
    "def",
    "del",
    "elif",
    "else",
    "except",
    "finally",
    "for",
    "from",
    "global",
    "if",
    "import",
    "in",
    "is",
    "lambda",
    "nonlocal",
    "not",
    "or",
    "pass",
    "raise",
    "return",
    "try",
    "while",
    "with",
    "yield"};

  for(int i=0;i<36;i++){
    if(!strcmp(curr_token, keywords[i])){
      return true;
    }
  }
  return false;
}

void string_state_handler(char buffer[], int it, FILE* tokens_filehandle, char curr_token[])
{
  if(buffer[it] == '"'){
    fputc(buffer[it], tokens_filehandle);
    it++;
    while(buffer[it] != '"'){
      fputc(buffer[it], tokens_filehandle);
      it++;
    }
    fputc(buffer[it], tokens_filehandle);
  }

  it++;
  if(is_identifier_char(buffer[it])) {
    identifier_char_state_handler(buffer, it, tokens_filehandle, curr_token);
  }
  else if(is_operator_char(buffer[it])){
    symbol_state_handler(buffer, it, 0, tokens_filehandle, curr_token);
  }
  else{
    identifier_char_state_handler(buffer, it, tokens_filehandle, curr_token);
  }
}



void identifier_char_state_handler(char buffer[], int it, FILE* tokens_filehandle, char curr_token[])
{

  if(buffer[it] != '\0'){
    if(is_identifier_char(buffer[it])){
      printf("\n%s", curr_token);

      curr_token[it] = buffer[it];
      if(is_keyword(curr_token)){
        printf("\n %s is a keyword", curr_token);
        memset(curr_token, 0, CURRENT_TOKEN_SIZE);
        fputc(buffer[it], tokens_filehandle);
        fputs("\n", tokens_filehandle);
        it++;
        identifier_char_state_handler(buffer, it, tokens_filehandle, curr_token);
      }
      else{
        fputc(buffer[it], tokens_filehandle);
        it++;
        identifier_char_state_handler(buffer, it, tokens_filehandle, curr_token);
      }
    }
    else if(is_operator_char(buffer[it])){
      fputs("\n", tokens_filehandle);
      int op_count = 0;
      // if not an identifier character
      symbol_state_handler(buffer, it, op_count, tokens_filehandle, curr_token);
    }
    // else if(buffer[it] == ' ' && is_identifier_char(buffer[it + 1])){
    //   printf("\nError identified due to faulty identifier char expressions");
    //   exit(1);
    // }
    else{
      it++;
      identifier_char_state_handler(buffer, it, tokens_filehandle, curr_token);
    }
  }
}

void symbol_state_handler(char buffer[], int it, int op_count, FILE* tokens_filehandle, char curr_token[]){

  // printf("\n\t\t%s", curr_token);
  if(buffer[it] != '\0'){
    if(is_operator_char(buffer[it]) && buffer[it] != '"'){
      op_count++;
      if(op_count > 3){
        printf("\nError! Cannot have more than 3 operators in an expression");
        return ;
      }
      fputc(buffer[it], tokens_filehandle);
      it++;
      symbol_state_handler(buffer, it, op_count, tokens_filehandle, curr_token);
    }
    else if(buffer[it] == '"'){
      fputs("\n", tokens_filehandle);
      string_state_handler(buffer, it, tokens_filehandle, curr_token);
    }
    else if(is_identifier_char(buffer[it])){
      fputs("\n", tokens_filehandle);
      identifier_char_state_handler(buffer, it, tokens_filehandle, curr_token);
    }
    else{
      it++;
      symbol_state_handler(buffer, it, 0, tokens_filehandle, curr_token);
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

  char curr_token[CURRENT_TOKEN_SIZE];

  memset(curr_token, 0, sizeof curr_token);

  while(fgets(buffer, 100, intermediate_filehandle)!=NULL){
    if(is_identifier_char(buffer[it])){
      identifier_char_state_handler(buffer, it, tokens_filehandle, curr_token);
    }
    else if(is_operator_char(buffer[it])){
      symbol_state_handler(buffer, it, op_count, tokens_filehandle, curr_token);
    }
    fputs("\n", tokens_filehandle);
  }
}
