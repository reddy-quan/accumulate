CC=arm-linux-gnueabi-gcc

PROG=ou

CFLAGS += -g -Wall

#curl
CFLAGS += -I../../build_curl-7.36.0/_install/include
LDFLAGS += -L../../build_curl-7.36.0/_install/lib
#openssl
CFLAGS += -I../../openssl-1.0.1e/_install/include
LDFLAGS += -L../../openssl-1.0.1e/_install/lib

LDFLAGS += -lcurl -lssl -lcrypto

FILES = online_upgrade.c

OBJS := $(FILES:.c=.o)

$(PROG):$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(PROG)

$(OBJS):%o:%c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -rf $(PROG) *.o

