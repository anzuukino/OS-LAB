#include <sys/signal.h>
#include <stdio.h>
#include <stdlib.h>


void sigint_handler(){
  puts("Exiting...");
  exit(0);
}


