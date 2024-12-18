build:
	gcc -I. components/redirect.c components/serializer.c components/history.c components/execute.c components/signal.c components/piping.c main.c  -g -o osh

