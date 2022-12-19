#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include "preprocessor_lexanalyzer.h"
#include "remove_unicode.h"

#define CURRENT_TOKEN_SIZE 100
#define NUM_KEYWORDS 36
#define MAX_KEYWORD_LEN 20

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
  char keywords[NUM_KEYWORDS][MAX_KEYWORD_LEN] = {
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

  for(int i=0;i<NUM_KEYWORDS;i++){
    if(!strcmp(curr_token, keywords[i])) return true;
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
  fputc('\n', tokens_filehandle);
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
      curr_token[it] = buffer[it];
      if(is_keyword(curr_token)){
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
      memset(curr_token, 0, CURRENT_TOKEN_SIZE);
      int op_count = 0;
      // if not an identifier character
      symbol_state_handler(buffer, it, op_count, tokens_filehandle, curr_token);
    }
    else{
      memset(curr_token, 0, CURRENT_TOKEN_SIZE);
      it++;
      identifier_char_state_handler(buffer, it, tokens_filehandle, curr_token);
    }
  }
}

void symbol_state_handler(char buffer[], int it, int op_count, FILE* tokens_filehandle, char curr_token[])
{

  if(buffer[it] != '\0'){
    if(is_operator_char(buffer[it]) && buffer[it] != '"'){
      op_count++;
      if(op_count > 3){
        printf("\nError! Exiting...");
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

  remove_unicode("intermediate_file.py");

  FILE* intermediate_filehandle = fopen("intermediate_file2.py", "r+");

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
    else{
      identifier_char_state_handler(buffer, it, tokens_filehandle, curr_token);
    }
    fputs("\n", tokens_filehandle);
  }
}
