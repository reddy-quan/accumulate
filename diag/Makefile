#2013.10.10
# CC=arm-linux-gcc
PROG=diag

#CFLAGS += -Ilib
CFLAGS += -g -Wall
LDFLAGS += -lpthread 

USE_SSL = 0
#use ssl?
ifeq (1, ${USE_SSL})
CFLAGS += -DUSE_SSL
CFLAGS += -I/home/reddy/work/openssl-1.0.1e/_install/include
LDFLAGS += /home/reddy/work/openssl-1.0.1e/_install/lib/libssl.a /home/reddy/work/openssl-1.0.1e/_install/lib/libcrypto.a
LDFLAGS += -ldl
endif

# CFLAGS += -I../inc

#libxml2
# LIBXML2 = ../libxml2-2.9.0/_install
# CFLAGS += -I$(LIBXML2)/include/libxml2
# LDFLAGS += -L$(LIBXML2)/lib
# LDFLAGS += -lxml2

FILES := main.c csapp.c serial_port.c

OBJS := $(FILES:.c=.o)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(PROG)

$(OBJS):%.o:%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -rf $(PROG) *.o ../inc/*.o

