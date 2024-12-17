build:
	gcc -I. components/serializer.c components/execute.c components/signal.c components/piping.c components/main.c  -g -o osh

