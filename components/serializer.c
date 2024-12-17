// gcc serializer.c -o main
#include <stdio.h>
#include "command.h"

#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
command cmd_list[20];

void trim(char * s) {
  char * p = s;
  int l = strlen(p);

  while(isspace(p[l - 1])) p[--l] = 0;
  while(* p && isspace(* p)) ++p, --l;

  memmove(s, p, l + 1);
} 
// Serialize multiple commands into command struct. e.g: 
// "ls | grep -r" --> {"ls", "grep -r"}
// "ls | grep -r "OOP" > oop.txt " --> {"ls", "grep -r "OOP" > oop.txt"}
// return number of commands found or -1 if number of command > 20. redirection does not count as command
// Set pipe_to, pipe_from of each command.
int serialize_command(char* input){
  if(input == NULL)
    return -1;
  char *delim_pos = NULL;
  char *start = input;
  int cmd_count = 0;
  while(*start){
    delim_pos = strpbrk(start, "|;");
    if(delim_pos){
      int length = delim_pos - start;
      cmd_list[cmd_count].raw_string = strndup(start, length);
      if(*delim_pos == '|'){
        cmd_list[cmd_count].pipe_from = true;
        cmd_list[cmd_count].pipe_to = true;
      }
      start = delim_pos + 1;
      cmd_count++;
    }
    else {
      cmd_list[cmd_count].raw_string = strdup(start);
      if(*(start-1) == '|'){
        cmd_list[cmd_count].pipe_to = false;
        cmd_list[cmd_count].pipe_from = true;
      }
      cmd_count++;
      break;
    }
  }
  cmd_list[0].pipe_from = false;
  for(int i=0;i<cmd_count;i++)
    trim(cmd_list[i].raw_string);
  return cmd_count;
}

// Serialize a single commmand into args. e.g: 
// "grep -r -i "abc"" -> {"grep", "-r", "-i", ""abc""}
int serialize_args(command *cmd){
  if(cmd == NULL)
    return -1;
  // Seperate redirect file from command
  char* token = NULL;
  char *cmd_only = NULL;
  char *file = NULL;
  char *first = NULL;
  char *second = NULL;

  if(strchr(cmd->raw_string, '>') && strchr(cmd->raw_string, '>'))
    cmd->redirect = FILE_IO;
  else if(strchr(cmd->raw_string, '<' ))
    cmd->redirect = FILE_INP;
  else if(strchr(cmd->raw_string, '>'))
    cmd->redirect = FILE_OUT;
  else 
    cmd->redirect = NONE;

  if(cmd->redirect != NONE){
    first = strpbrk(cmd->raw_string, "><");

    second = strpbrk(first+1, "><");
    if(*second == '>')
      cmd->file_out = strdup(second+1);
    else if(*second == '<')
      cmd->file_in = strdup(second+1);

    *second = 0;
    if(*first == '>')
      cmd->file_out = strdup(first+1);
    else
      cmd->file_in = strdup(first+1);
    *first = 0;

  }

  // Split args.
  cmd_only = strdup(cmd->raw_string);
  int argc = 0;
  token = strtok(cmd_only, " ");
  while(token != NULL){
    cmd->args[argc++] = strdup(token);
    if(argc > 20) {
      free(cmd_only);
      return -1;
    }
    token = strtok(NULL, " ");
  }
  free(cmd_only);
  cmd->arg_count = argc;
  return 0;
}

void safe_free(void**ptr){
  if(*ptr){
    free(*ptr);
    *ptr = NULL;
  }
}

void clean_cmd(){
  for(int i=0;i<20;i++){
    safe_free((void**) &cmd_list[i].raw_string);
    safe_free((void**) &cmd_list[i].file_in);
    safe_free((void**) &cmd_list[i].file_out);
    for(int j =0;j<20;j++)
      safe_free((void**) &cmd_list[i].args[j]);
    cmd_list[i].arg_count = 0;
    cmd_list[i].pipe_to = false;
    cmd_list[i].pipe_from = false;
    cmd_list[i].redirect = NONE;
  }
}

int serializer(char *userinput){
  int cmd_count = serialize_command(userinput);
  if(cmd_count == -1){
    printf("Number of commands exceeds 20\n");
    return -1;
  }
  for(int i=0;i<cmd_count;i++){
    int res = serialize_args(&cmd_list[i]);
    if(res == -1){
      printf("Number of arguments exceeds 20\n");
      return -1;
    }
  }
  return cmd_count;
}
#define DEBUG
#ifdef DEBUG
void print_command(command *cmd){
  printf("Raw string: %s\n", cmd->raw_string);
  printf("Argc: %d\n", cmd->arg_count);
  for(int i=0;i<cmd->arg_count;i++){
    printf("argv[%d]: %s\n", i, cmd->args[i]);
  }
  printf("Redirect: %d\n", cmd->redirect);
  printf("File in: %s\n", cmd->file_in);
  printf("File_out: %s\n", cmd->file_out);
  printf("Pipe from: %s\n", cmd->pipe_from ? "True" : "False");
  printf("Pipe to: %s\n", cmd->pipe_to ? "True" : "False");
}
#endif /* ifdef DEBUG */ 
//#define SERIALIZE_TEST
#ifdef SERIALIZE_TEST 
int main(int argc, char *argv[])
{
  char script[0x1000];
  fgets(script,0xfff, stdin);
  script[strcspn(script, "\n")] = 0;
  int cmd_count = serializer(script);
  for(int i=0;i<cmd_count;i++)
    print_command(&cmd_list[i]);
  execvp(cmd_list[0].args[0], cmd_list[0].args);
  return EXIT_SUCCESS;
}
#endif
