#ifndef __common_h
#define __common_h

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define LOG(...)  printf(__VA_ARGS__)

#define FALSE  -1
#define TRUE     0

#define SUCCESS TRUE
#define FAIL        FALSE

#define MAX_NAME_LEN 128

#define DIAG_PORT_DEV "/dev/ttyUSB0"

typedef unsigned short ushort;
typedef char byte;

typedef struct _diag_cmd {
	byte * cmd;
	int len;
} diag_cmd;

#endif

