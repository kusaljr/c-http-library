CC = gcc
CFLAGS = -g -I/usr/include/postgresql
LIBS = -lpq -lcrypto

SRCS = main.c server/server.c http/http_parser.c db/pg_connector.c handlers/auth_handler.c logger/logger.c jwt/jwt_helper.c handlers/env_parser.c
TARGET = myprogram

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f $(TARGET)
