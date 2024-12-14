#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "command.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

extern command cmd_list[20];
extern void print_command(command*);
extern int serializer(char*);

void execute(int cmd_count){
  for (int i = 0; i < cmd_count; i++) {
    command *cmd = &cmd_list[i];

    // Kiểm tra có sử dụng ký tự & không
    int background = 0;
    for (int j = 0; j < cmd->arg_count; j++) {
      if (strcmp(cmd->args[j], "&") == 0) {
        background = 1;
        cmd->args[j] = NULL; // Loại bỏ '&' từ args vì execvp không cần nó.
        break;
      }
    }

    // Gọi execvp để thực thi lệnh
    pid_t pid = fork();
    if (pid == 0) { // Process con
      execvp(cmd->args[0], cmd->args);
      perror("execvp failed");
      exit(EXIT_FAILURE);
    } else if (pid > 0) { // Process cha
      if (!background) {
        wait(NULL); // Chờ process con thoát nếu không chạy ngầm
      }
    } else {
      perror("fork failed");
      exit(EXIT_FAILURE);
    }
  }
}
#define EXECUTE_TEST_
#ifdef EXECUTE_TEST_
int main(int argc, char *argv[])
{
  char script[0x1000];
  fgets(script,0xfff, stdin);
  script[strcspn(script, "\n")] = 0;

  int cmd_count = serializer(script);

  for(int i=0;i<cmd_count;i++)
    print_command(&cmd_list[i]);
  execute(cmd_count);
  return EXIT_SUCCESS;
}

#endif /* ifdef EXECUTE_TEST_
 */
