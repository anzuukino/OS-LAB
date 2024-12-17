#include <sys/signal.h>
#include <stdio.h>
#include <stdlib.h>


void sigint_handler(){
  puts("\nExiting...");
  exit(0);
}


