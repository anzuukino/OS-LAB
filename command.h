#include <stdbool.h>
#ifndef COMMAND_H
#define COMMAND_H

typedef struct{
  char *raw_string;
  char *args[20];
  int arg_count;
  enum {NONE, FILE_INP, FILE_OUT} redirect ;
  char *filename;
  bool pipe_to;
  bool pipe_from;
} command;

#endif // !COMMAND_H
