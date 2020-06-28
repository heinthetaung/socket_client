SRCS=socket_client.c cJSON.c
LDFLAGS=-pthread
#CC=aarch64-linux-gnu-gcc
CC=gcc
#CC=x86_64-w64-mingw32-gcc

all: socket_client

socket_client: $(SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o socket_client $(SRCS)
clean:
	-rm -f socket_client

