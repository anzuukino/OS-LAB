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
extern void safe_free(void **);
extern void execute_redirect(command*);
//void execute(int cmd_count){
//
//
//  for (int i = 0; i < cmd_count; i++) {
//    command *cmd = &cmd_list[i];
//    signal(SIGCHLD, SIG_IGN); // prevent child process from zombie after terminated
//    // Kiểm tra có sử dụng ký tự & không
//    int background = 0;
//    for (int j = 0; j < cmd->arg_count; j++) {
//      if (strncmp(cmd->args[j], "&", 1) == 0) {
//        background = 1;
//        safe_free((void**) &cmd->args[j]);
//        cmd->args[j] = NULL; // Loại bỏ '&' từ args vì execvp không cần nó.
//        break;
//      }
//    }
//
//    // Gọi execvp để thực thi lệnh
//    pid_t pid = fork();
//    if (pid == 0) { // Process con
//      execvp(cmd->args[0], cmd->args);
//      perror("execvp failed");
//      printf("Cannot execute %s\n",cmd->raw_string);
//      exit(EXIT_FAILURE);
//    } else if (pid > 0) { // Process cha
//        if (!background) {
//          wait(NULL); // Chờ process con thoát nếu không chạy ngầm
//        }
//    } else {
//      perror("fork failed");
//      exit(EXIT_FAILURE);
//    }
//  }
//
//}
//
void execute(command* cmd){
 signal(SIGCHLD, SIG_IGN); // prevent child process from zombie after terminated
    // Kiểm tra có sử dụng ký tự & không
    int background = 0;
    for (int j = 0; j < cmd->arg_count; j++) {
      if (strncmp(cmd->args[j], "&", 1) == 0) {
        background = 1;
        safe_free((void**) &cmd->args[j]);
        cmd->args[j] = NULL; // Loại bỏ '&' từ args vì execvp không cần nó.
        break;
      }
    }

    // Gọi execvp để thực thi lệnh
    if(cmd->args[0] == NULL)
      return ;
    if (strcmp(cmd->args[0],"cd") == 0){
        chdir(cmd->args[1]);
        return;
    }
    // printf("cmd->history_error: %d\n",cmd->history_error);
    if (cmd->history_error){
      printf("%s\n",cmd->raw_string);
      return;
    }
    pid_t pid = fork();
    int original_stdin = dup(STDIN_FILENO);
    int original_stdout = dup(STDOUT_FILENO);
    
    if (pid == 0) { // Process con
    // Merging the redirecting file function of the shell here
      execute_redirect(cmd); 
      execvp(cmd->args[0], cmd->args);
      perror("execvp failed");
      printf("Cannot execute %s\n",cmd->raw_string);
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


#ifdef EXECUTE_TEST_
int main(int argc, char *argv[])
{
  char script[0x1000];
  fgets(script,0xfff, stdin);
  script[strcspn(script, "\n")] = 0;

  int cmd_count = serializer(script);
  for(int i=0;i<cmd_count;i++){
    print_command(&cmd_list[i]);
    execute(&cmd_list[i]);
  }
  return EXIT_SUCCESS;
}

#endif /* ifdef EXECUTE_TEST_
 */
