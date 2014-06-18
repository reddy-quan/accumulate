#include <time.h>
//#include "system_log.h"
#include "common.h"
#include <pthread.h>
#include <signal.h>

#include <sys/select.h>
#include <sys/time.h>


#include     <stdio.h>      /*标准输入输出定义*/
#include     <stdlib.h>     /*标准函数库定义*/
#include     <unistd.h>     /*Unix 标准函数定义*/
#include     <sys/types.h>
#include     <sys/stat.h>
#include     <fcntl.h>      /*文件控制定义*/
#include     <termios.h>    /*PPSIX 终端控制定义*/
#include     <errno.h>      /*错误号定义*/

/**
*@brief  设置串口通信速率
*@param  fd     类型 int  打开串口的文件句柄
*@param  speed  类型 int  串口速度
*@return  void
*/
int speed_arr[] = { B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300};
int name_arr[] =  { 115200,      57600,    38400,   19200,    9600,   4800,    2400,    1200,   300};
void set_serial_speed(int fd, int speed)
{
    int   i;
    int   status;
    struct termios   Opt;
    tcgetattr(fd, &Opt);
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
    {
        if  (speed == name_arr[i])
        {
            tcflush(fd, TCIOFLUSH);
            if(0!=cfsetispeed(&Opt, speed_arr[i])){
                LOG("cfsetispeed error\n");
            }
            if(0!=cfsetospeed(&Opt, speed_arr[i])){
                LOG("cfsetospeed error\n");
            }
            status = tcsetattr(fd, TCSANOW, &Opt);
            if  (status != 0)
            {
                LOG("tcsetattr error, errno:%d", status);
                return;
            }
            tcflush(fd,TCIOFLUSH);
        }
    }
}


/**
*@brief   设置串口数据位，停止位和效验位
*@param  fd     类型  int  打开的串口文件句柄
*@param  databits 类型  int 数据位   取值 为 7 或者8
*@param  stopbits 类型  int 停止位   取值为 1 或者2
*@param  parity  类型  int  效验类型 取值为N,E,O,S
*/
int set_serial_options(int fd,int databits,int stopbits,int parity)
{
    struct termios options;
    if  ( tcgetattr( fd,&options)  !=  0)
    {
        LOG("SetupSerial 1\n");
        return(FALSE);
    }
    options.c_cflag &= ~CSIZE;
    switch (databits) /*设置数据位数*/
    {
    case 7:
        options.c_cflag |= CS7;
        break;
    case 8:
        options.c_cflag |= CS8;
        break;
    default:
        LOG("Unsupported data size\n");
        return (FALSE);
    }
    switch (parity)
    {
    case 'n':
    case 'N':
        options.c_cflag &= ~PARENB;   /* Clear parity enable */
        options.c_iflag &= ~INPCK;     /* Enable parity checking */
        break;
    case 'o':
    case 'O':
        options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/
        options.c_iflag |= INPCK;             /* Disnable parity checking */
        break;
    case 'e':
    case 'E':
        options.c_cflag |= PARENB;     /* Enable parity */
        options.c_cflag &= ~PARODD;   /* 转换为偶效验*/
        options.c_iflag |= INPCK;       /* Disnable parity checking */
        break;
    case 'S':
    case 's':  /*as no parity*/
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        break;
    default:
        LOG("Unsupported parity\n");
        return (FALSE);
    }
    /* 设置停止位*/
    switch (stopbits)
    {
    case 1:
        options.c_cflag &= ~CSTOPB;
        break;
    case 2:
        options.c_cflag |= CSTOPB;
        break;
    default:
        LOG("Unsupported stop bits\n");
        return (FALSE);
    }
    /* Set input parity option */
    if (parity != 'n')
        options.c_iflag |= INPCK;
    tcflush(fd,TCIFLUSH);
    //options.c_cc[VTIME] = 0;
    /*如果串口超时没有收到消息，则可
    认为现场服务器与门锁控制器串口通信中断，
    需要立即发送报警
    */
    //qj options.c_cc[VTIME] = 150; /* 设置超时15 seconds*/
    //qj options.c_cc[VMIN] = 0; /* Update the options and do it NOW */

    /*
    如果不是开发终端之类的，只是串口传输数据，
    而不需要串口来处理，
    那么使用原始模式(Raw Mode)方式来通讯
    */
    options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
    options.c_oflag  &= ~OPOST;   /*Output*/

    
    if (tcsetattr(fd,TCSANOW,&options) != 0)
    {
        LOG("SetupSerial 3");
        return (FALSE);
    }
    return (TRUE);
}

int get_serial_fd()
{
    int fd;

    fd=open(DIAG_PORT_DEV, O_RDWR);//O_NONBLOCK
    if (fd<0)
    {
        LOG("open diag port %s failed, error %d\n", DIAG_PORT_DEV, errno);
        exit(-1);
    }
    set_serial_speed(fd, 57600);

    if (set_serial_options(fd, 8, 1, 'N') == FALSE)
    {
        LOG("Set Parity Error\n");
        exit (-1);
    }
    return fd;
}

int get_serial_fd_retry(int retry_time, int interval)
{
	int fd;

	interval = interval <5? 5: interval;
	
	do {
		fd=open(DIAG_PORT_DEV, O_RDWR);//O_NONBLOCK
		if (fd<0)
		{
		    LOG("%s failed, error is %d, retry left %d\n", __func__, errno, retry_time);
		    sleep(interval);
		    retry_time--;
		    continue;
		}
		set_serial_speed(fd, 57600);
		if (set_serial_options(fd, 8, 1, 'N') == FALSE)
		{
		    LOG("Set Parity Error\n");
		    exit (-1);
		}
		return fd;
	} while(retry_time>0);
	return -1;
}

int check_reboot_ok(int retry_time, int interval)
{
	int fd;

	interval = interval <0? 1: interval;
	
	do {
		fd=open(DIAG_PORT_DEV, O_RDWR);//O_NONBLOCK
		if (fd<0)
		{
		    LOG("%s failed, error is %d, retry left %d\n", __func__, errno, retry_time);
		    if (errno ==2)
		    	return 1;

		    sleep(interval);
		}
		close(fd);
		sleep(interval);
		LOG("%s waiting for reboot, retry left %d\n", __func__, --retry_time);
	} while(retry_time>0);
	return 0;
}