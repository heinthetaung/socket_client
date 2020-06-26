SRCS=socket_client.c
LDFLAGS=
#CC=aarch64-linux-gnu-gcc
CC=gcc

all: socket_client

socket_client: $(SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o socket_client $(SRCS)
clean:
	-rm -f socket_client

