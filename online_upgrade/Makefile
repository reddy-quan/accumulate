CC=arm-linux-gnueabi-gcc

PROG=ou

CFLAGS += -g -Wall

CFLAGS += -I../../build_curl-7.36.0/_install/include
LDFLAGS += -L../../build_curl-7.36.0/_install/lib

LDFLAGS += -lcurl

FILES = online_upgrade.c

OBJS := $(FILES:.c=.o)

$(PROG):$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(PROG)

$(OBJS):%o:%c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -rf $(PROG) *.o

