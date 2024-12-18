#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "command.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/shm.h>
extern command cmd_list[20];
extern void print_command(command*);
extern int serializer(char*);

void *shared_mem;
flag_ * flag;

void redirect(const char * filename, int newfd){
  int fd;
  if(newfd == STDIN_FILENO)
    fd = open(filename, O_RDONLY);
  if(newfd == STDOUT_FILENO)
    fd = open(filename, O_WRONLY |  O_CREAT, 0755);
  if(fd == -1){
    perror("open() failed. Exiting...");
    exit(-1);
  }
  if(dup2(fd, newfd) < 0){
    perror("dup2() failed. Exiting...");
    exit(-1);
  }
  close(fd);
}

void execute_pipe(int cmd_count){
  int pid1 = fork();
  if(pid1 == 0){
    int old_pipe[2];
    int new_pipe[2];
    flag->stop_piping = 0;
    for(int i=0;i<cmd_count;i++){
      if(flag->stop_piping)
        break;
      if(cmd_list[i].pipe_to)
        pipe(new_pipe);

      pid_t pid = fork();

      if(pid < 0){
        perror("fork() failed. Exiting...");
        exit(-1);
      }

      if(pid == 0){
        if(cmd_list[i].pipe_from){
          dup2(old_pipe[0], STDIN_FILENO);
          close(old_pipe[0]);
          close(old_pipe[1]);
        }
        if(i==0 && cmd_list[i].file_in)
          redirect(cmd_list[i].file_in, STDIN_FILENO);

        if(i==cmd_count-1 && cmd_list[i].file_out)
          redirect(cmd_list[i].file_out, STDOUT_FILENO);


        if(cmd_list[i].pipe_to){
          close(new_pipe[0]);
          dup2(new_pipe[1], STDOUT_FILENO);
          close(new_pipe[1]);
        }
        execvp(cmd_list[i].args[0], cmd_list[i].args);
        perror("execvp error");
        flag->stop_piping = 1;
        exit(-1);
      }
      else{
        if(cmd_list[i].pipe_from){
          close(old_pipe[0]);
          close(old_pipe[1]);
        }
        if(cmd_list[i].pipe_to){
          old_pipe[0] = new_pipe[0];
          old_pipe[1] = new_pipe[1];
        }
      }
    }
    close(old_pipe[0]);
    close(old_pipe[1]);
    close(new_pipe[0]);
    close(new_pipe[1]);
    usleep(10000);
    exit(0);
  }
  if(pid1 > 0){
    wait(NULL);
    return;
  }
}

#ifdef PIPING_TEST_

int main(int argc, char *argv[])
{
  char script[0x1000];
  fgets(script,0xfff, stdin);
  script[strcspn(script, "\n")] = 0;

  int cmd_count = serializer(script);

  for(int i=0;i<cmd_count;i++)
    print_command(&cmd_list[i]);
  execute_pipe(cmd_count);
  return EXIT_SUCCESS;
}


#endif /* ifdef PIPING_TEST_
#define PIPING_TEST_ */

