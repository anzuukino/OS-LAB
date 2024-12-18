#include <signal.h>
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
#include <linux/limits.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"


extern command cmd_list[20];
extern void print_command(command*);
extern int serializer(char*);
extern void execute_commands(int );
extern void execute_pipe(int);
extern void execute(command*);
extern void *shared_mem; //remember to initialize this 
extern flag_ * flag;
extern void clean_cmd();
extern void sigint_handler();
extern void sigquit_handler();
extern void print_command(command*);
extern void execute_redirect(command*);

void* create_shm(){
  int shared_fd = shm_open("OS", O_RDWR | O_CREAT, 0666);
  ftruncate(shared_fd, 0x1000);
  void * ret = mmap(0, 0x1000, PROT_READ |PROT_WRITE, MAP_SHARED, shared_fd, 0);
  if(ret == (void*)-1){
    perror("mmap error");
    exit(-1);
  }
  return ret;
}

void execute_commands(int cmd_count){
  if (cmd_list[0].pipe_to == true){
    execute_pipe(cmd_count);
  }
  else {
    for(int i=0;i<cmd_count;i++){
      execute(&cmd_list[i]);
    }
  }
}

int main(int argc, char *argv[]){
  char *input_cmd = malloc(0x1000);
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd) );
  shared_mem = create_shm();
  flag = shared_mem;
  int cmd_count = 0;
  
  signal(SIGINT, sigint_handler);

  while(1){
    clean_cmd();
    memset(input_cmd,0,0x1000);
    printf(KRED "osh: %s$ " KNRM,cwd);
    fgets(input_cmd,0xfff, stdin);
    input_cmd[strcspn(input_cmd, "\n")] = 0;

    // Serialize the input command and execute it
    cmd_count = serializer(input_cmd);
    execute_commands(cmd_count);
    usleep(100000);
  }
  return EXIT_SUCCESS;
}

