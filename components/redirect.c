#include <stdio.h>
#include <string.h>
#include "command.h"
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

extern command cmd_list[20];
extern void print_command(command*);
extern int serializer(char*);
extern void redirect(const char*, int);
extern void execute(command*);

void execute_redirect(command *cmd){
    if(cmd->file_in){
        redirect(cmd->file_in, STDIN_FILENO);
    }
    if(cmd->file_out){
        redirect(cmd->file_out, STDOUT_FILENO);
    }
}

#ifdef REDIRECT_TEST_
int main(int argc, char *argv[])
{
  char script[0x1000];
  fgets(script,0xfff, stdin);
  script[strcspn(script, "\n")] = 0;

  int cmd_count = serializer(script);

  for(int i=0;i<cmd_count;i++)
  {
    print_command(&cmd_list[i]);
    execute_redirect(&cmd_list[i]);
  }
  return EXIT_SUCCESS;
}

#endif