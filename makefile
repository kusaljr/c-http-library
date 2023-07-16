CC = gcc
CFLAGS = -g -I/usr/include/postgresql
LIBS = -lpq

myprogram: main.c server.c http_parser.c db/pg_connector.c auth_handler.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)