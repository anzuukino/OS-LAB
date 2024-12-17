#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "command.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>
#define HISTORY_FILE "shell_history"
#define BUFF_SIZE 0x1000
#define HISTORY_SIZE 0x1000

char *history_command_list[100];
char *history_list[HISTORY_SIZE];
int CURRENT_HISTORY = 0;
char *NOTFOUND = "Not found";

extern command cmd_list[20];
extern void print_command(command*);
extern int serializer(char*);
extern void execute(int);

void trim_newline(char * s) {
  char * p = s;
  int l = strlen(p);

  while (l > 0 && (p[l - 1] == '\n')) {
        p[--l] = 0;
  }

  memmove(s, p, l + 1);
} 

void read_history(){
  FILE* file = fopen(HISTORY_FILE, "r");
  if (file == NULL) {
    perror("Error opening history file");
    exit(EXIT_FAILURE);
  }
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  int index = 0;
  while ((read = getline(&line, &len, file)) != -1) {
    history_list[index] = strdup(line);
    trim_newline(history_list[index]);
    index++;
  }
  CURRENT_HISTORY = index;
  fclose(file);
  if (line)
    free(line);
}


void history_add(char* cmd){
  FILE* file = fopen(HISTORY_FILE, "a");
  if (file == NULL) {
    perror("Error opening history file");
    exit(EXIT_FAILURE);
  }
  fprintf(file, "%s\n", cmd);
  fclose(file);
}

int history_serializer(char* history_command){
  if(history_command == NULL)
    return 0;
  char *start = history_command;
  size_t length = strlen(history_command);
  char *end = history_command + length;
  int history_count = 0, index = 0;
  int flag = 0;
  char *temp = (char*)malloc(BUFF_SIZE);
  while(*start){
    // printf("Start: %c,%d\n", *start, index);
    if (*start == '!' && flag == 0){
      if (index != 0){
        char *tmp = strcpy(strndup(temp, index), temp);
        history_command_list[history_count] = tmp;
        history_count++;
        bzero(temp, BUFF_SIZE);
        index = 0;
      }
      temp[index++] = *start;
      flag = 1;
      start++;
      continue;
    }
    if (flag == 1){
      if (*start == '!'){
        char *tmp = strndup(start - 1, 2);
        history_command_list[history_count] = tmp;
        history_count++;
        start++;
        flag = 0;
        bzero(temp, BUFF_SIZE);
        index = 0;
        continue;
      }else if (*start == '-' || isdigit(*start)){
        char *start_index = start;
        while(isdigit(*start)){
          start++;
        }
        char *tmp = strndup(start_index - 1, start - start_index + 1);
        history_command_list[history_count] = tmp;
        history_count++;
        flag = 0;
        bzero(temp, BUFF_SIZE);
        index = 0;
        continue;
      }else{
        char *start_index = start;
        while(*start != '!' && *start){
          start++;
        }
        char *tmp = strndup(start_index - 1, start - start_index + 1);
        history_command_list[history_count] = tmp;
        history_count++;
        flag = 0;
        bzero(temp, BUFF_SIZE);
        index = 0;
        continue;
      }
    }

    temp[index] = *start;
    index++;
    start++;
  }
  if (index != 0){
    history_command_list[history_count] = temp;
    history_count++;
  }
  return history_count;
}

char *convert_history(char* history_command){
  if (history_command == NULL)
    return NULL;
  char *start = history_command;
  size_t length = strlen(history_command);
  char *end = history_command + length;
  if (*start == '!'){
    if (*(start + 1) == '!'){
      return strdup(history_list[CURRENT_HISTORY - 1]);
    }else if (*(start + 1) == '-'){
      start++;
      int index = atoi(start);
      if (CURRENT_HISTORY + index < 0){
        return NOTFOUND;
      }
      return strdup(history_list[CURRENT_HISTORY + index]);
    }else if (isdigit(*(start + 1))){
      start++;
      int index = atoi(start);
      if (index >= CURRENT_HISTORY){
        return NOTFOUND;
      }
      return strdup(history_list[index]);
    }else{
      return strdup(history_command);
    }
  }else{
    return strdup(history_command);
  }
  
}

void history_command(command* history_cmd){
  char* cmd = history_cmd->raw_string;
  // puts("Analyzing history command");
  int history_count = history_serializer(cmd);
  printf("History count: %d\n", history_count);
  // for (int i = 0; i < history_count; i++){
  //   printf("History command %d: %s\n", i, history_command_list[i]);
  // }
  read_history();
  for (int i = 0; i < history_count; i++){
    char *history_cmd = convert_history(history_command_list[i]);
    printf("History command %d: %s\n", i, history_cmd);
  }
  
}
#define HISTORY_TEST_
#ifdef HISTORY_TEST_
int main(int argc, char *argv[])
{
  
    char script[0x1000];
    fgets(script,0xfff, stdin);
    script[strcspn(script, "\n")] = 0;
    int cmd_count = serializer(script);
    for(int i=0;i<cmd_count;i++){
      // print_command(&cmd_list[i]);
      history_command(&cmd_list[i]);
      
    }
    history_add(script);

  return EXIT_SUCCESS;
}

#endif /* ifdef EXECUTE_TEST_
 */
