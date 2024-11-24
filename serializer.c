// gcc serializer.c -o main
#include <stdio.h>
#include "command.h"
#include "global.c"
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
// Serialize multiple commands into command struct. e.g: 
// "ls | grep -r" --> {"ls", "grep -r"}
// "ls | grep -r "OOP" > oop.txt " --> {"ls", "grep -r "OOP" > oop.txt"}
// return number of commands found or -1 if number of command > 20. redirection does not count as command
// Set pipe_to, pipe_from of each command.
int serialize_command(char* input){
  if(input == NULL)
    return -1;
  char *inp_dup = strdup(input);
  int cmd_count = 0;
  char *token = NULL;
  token = strtok(inp_dup, "|");
  while(token != NULL){
    cmd_list[cmd_count].raw_string = strdup(token);
    cmd_list[cmd_count].pipe_to = true;
    cmd_list[cmd_count++].pipe_from = true;
    if(cmd_count > 20)
      return -1;
    token = strtok(NULL, "|");
  }
  cmd_list[0].pipe_from = false;
  cmd_list[cmd_count-1].pipe_to = false;
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
  if(strchr(cmd->raw_string, '>'))
    cmd->redirect = FILE_OUT;
  else if(strchr(cmd->raw_string, '<' ))
    cmd->redirect = FILE_INP;
  else 
    cmd->redirect = NONE;
  if(cmd->redirect != NONE){
    token = strtok(cmd->raw_string, "><");
    cmd_only = strdup(token);
    token = strtok(NULL, " \n");
    file = strdup(token);
    cmd->filename = file;
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

static inline void safe_free(void*ptr){
  if(ptr){
    free(ptr);
    ptr = NULL;
  }
}

void clean_cmd(){
  for(int i=0;i<20;i++){
    safe_free(cmd_list[i].raw_string);
    safe_free(cmd_list[i].filename);
    for(int j =0;j<20;j++)
      safe_free(cmd_list[i].args[j]);
    cmd_list[i].arg_count = 0;
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
  printf("Filename: %s\n", cmd->filename);
}
#endif /* ifdef DEBUG */ 

int main(int argc, char *argv[])
{
  char script[0x1000];
  fgets(script,0xfff, stdin);
  int cmd_count = serializer(script);
  for(int i=0;i<cmd_count;i++)
    print_command(&cmd_list[i]);
  return EXIT_SUCCESS;
}
