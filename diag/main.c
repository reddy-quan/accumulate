#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>

#include "common.h"

int diagfd;

//diag command list
const short off_line_id = 1;
const short MODE_RESET_F = 2;			  //!<' Reset. Only exit from offline

 // EFS CMD_CODE
 #define EFS_CMD_CODE  75
 // EFS SUBSYS_ID
 #define EFS_SUBSYS_ID  19
 // EFS SUBSYS_CMD_CODE
 #define EFS2_DIAG_HELLO  0
 #define EFS2_DIAG_OPEN  2
 #define EFS2_DIAG_CLOSE  3
 #define EFS2_DIAG_READ  4
 #define EFS2_DIAG_WRITE  5
 #define EFS2_DIAG_OPENDIR  11
 #define EFS2_DIAG_READDIR  12
 #define EFS2_DIAG_CLOSEDIR  13
 #define EFS2_DIAG_STAT  15
 #define EFS2_DIAG_PUT  38
 #define EFS2_DIAG_GET  39

 // File Mode
 #define EFS2_S_IFREG_RWX  33279  // 八进制100777
 #define EFS2_S_IFREG_RW	33206  // 八进制100666

 /*print key value*/
#define PR_HEX(buf, len) \
    {\
        int i;\
        for(i=0; i<len; i++)\
            printf("%02X ", buf[i]);\
        printf("\n");\
    }

static void inline get_bytes(char * in, const char* ptr, int count)
{
	if (!in || !ptr || count <1) {
		LOG("get_bytes: invalid input\n");
		exit(-1);
	}
	
	int i;
	for (i=0; i<count; i++)
		in[i] = ptr[i];
	return;
}

// CRC table for 16 bit CRC, with generator polynomial 0x8408, calculated 8 bits at a time, LSB first.
const byte ASYNC_HDLC_FLAG = 0x7E;              // Opening/closing flag  
const byte ASYNC_HDLC_ESC = 0x7D;               // Data Escape
const byte ASYNC_HDLC_ESC_MASK = 0x20;     // XOR mask used with Data Escape 
const byte ASYNC_HDLC_CRC = 2;	                // CRC bytes = 2 
const ushort CRC_SEED = 0xFFFF;
const ushort CRC_END = 0xF0B8;

static ushort m_CrcTable[] = {
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

static ushort crc_compute(ushort x_crc, byte x_byte)
{
	return (ushort)(( (x_crc) >> 8) ^ m_CrcTable[((x_crc) ^ (x_byte)) & 0x00ff]);
}

static byte* calculate(byte* _Raw, int in_len)
{
	//byte[] Ret_ = new byte[ _Raw.Length + 2 ];
	byte *Ret_ = malloc((in_len+2)*sizeof(byte));
	if (!Ret_) {
		LOG("%s malloc failed\n", __func__);
		exit(-1);
	}
	
	ushort CRC_ = CRC_SEED;
	int i;
	for (i = 0; i < in_len; i++)
	{
	    CRC_ = crc_compute(CRC_, _Raw[i]);
	    Ret_[i] = _Raw[i];
	}
	CRC_ ^= 0xFFFF;
	Ret_[in_len] = (byte)CRC_;
	Ret_[in_len+1] = (byte)(CRC_>>8);
	return Ret_;
}


/*
	调用本函数会返回指针，需要调用者手动 free()  !!!
*/

int crc_cmd_len=0; /*用来保存CRC后的命令串的长度*/

static byte* ConstructFrame( byte* _Raw, int _Len)
{
	byte* RawCRC_ = calculate(_Raw, _Len);

	byte *Tmp_ = malloc(_Len * 2*sizeof(byte));
	if (!Tmp_) {
		LOG("%s malloc failed\n", __func__);
		exit(-1);
	}
	
	int i, TmpLength_ = 0;
	for (i = 0; i < _Len; i++) // 特殊字替换
	{
		if ((RawCRC_[i] == ASYNC_HDLC_FLAG) || (RawCRC_[i] == ASYNC_HDLC_ESC))
		{
			Tmp_[TmpLength_] = ASYNC_HDLC_ESC;
			TmpLength_ += 1;
			Tmp_[TmpLength_] = (byte)(RawCRC_[i] ^ ASYNC_HDLC_ESC_MASK);
			TmpLength_ += 1;
		}
		else
		{
			Tmp_[TmpLength_] = RawCRC_[i];
			TmpLength_ += 1;
		}
	}
	free(RawCRC_);

	byte* Ret_ = malloc((TmpLength_+1)*sizeof(byte));
	
	for (i = 0; i < TmpLength_; i++ )
	{
		Ret_[i] = Tmp_[i];
	}
	free(Tmp_);
	Ret_[TmpLength_] = 0x7E;
	crc_cmd_len = TmpLength_+1;

	return Ret_;
}

static void __reset()
{
       // CMD_CODE   MODE  
       //       1 byte      2 bytes
       int ret;
	char cmd[3] = {0};
	char cmd_result[128] = {0};
	cmd[0] = 0x29;
	get_bytes(&cmd[1], (char*)&MODE_RESET_F, sizeof(MODE_RESET_F));
	LOG("%s cmd string is: %s\n", __func__, cmd);
	PR_HEX(cmd, sizeof(cmd));
	
	//Need construct CRC
	byte *crc_cmd = ConstructFrame(cmd, sizeof(cmd));
	PR_HEX(crc_cmd, crc_cmd_len);
	
	ret = write(diagfd, crc_cmd, crc_cmd_len);
	if ( crc_cmd_len!= ret) {
		LOG("%s failed,  errno %d\n", __func__, ret);
		exit(-1);
	}
	ret = read(diagfd, cmd_result, sizeof(cmd_result));
	LOG("%s read result is: %s\n", __func__, cmd_result);

	free(crc_cmd);
}

/*重启一般都执行的比较慢，要多等会儿*/

//重启相当于: send_data 0x29 0x02 0x00 0x7e
static void reset()
{
	int retry=3;
	__reset();
	int old_diagfd = diagfd;
	close(diagfd);
	/*if it DO rebooted, we will get an error no 2 when we attempt to open it
	use this to judge whether it rebooted or not!
	*/
	do {
		if (!check_reboot_ok(30, 1)) {
			LOG("%s check reboot not ok!\n", __func__);
		}
	} while (--retry>0);
	
	LOG("%s reboot ok, go to next step...\n", __func__);	
	diagfd = get_serial_fd_retry(10, 5);
	if (diagfd<3) {
		LOG("%s failed, diagfd is %d\n", __func__, diagfd);
		exit (-1);
	}
	
	LOG("%s OK, diagfd is %d, old diagfd is %d\n", __func__, diagfd, old_diagfd);
}

static void off_line()
{
       // CMD_CODE   MODE  
       //       1 byte      2 bytes
       int ret;
	char cmd[3] = {0};
	char cmd_result[128] = {0};
	cmd[0] = 0x29;
	get_bytes(&cmd[1], (char*)&off_line_id, sizeof(off_line_id));
	LOG("%s cmd string is: %s\n", __func__, cmd);
	PR_HEX(cmd, sizeof(cmd));
	
	ret = write(diagfd, cmd, sizeof(cmd));
	if ( sizeof(cmd)!= ret) {
		LOG("switch to off line failed, abort!\n");
		exit(-1);
	}
	ret = read(diagfd, cmd_result, sizeof(cmd_result));
	LOG("read is: %s\n", cmd_result);
	
}

static void switch_to_download()
{
            // CMD_CODE  
            //       1 byte
       int ret;
	char cmd[1] = {0x3A};
	char cmd_result[128] = {0};
	
	ret = write(diagfd, cmd, sizeof(cmd));
	if ( sizeof(cmd)!= ret) {
		LOG("switch to off line failed, abort!\n");
		exit(-1);
	}
	LOG("%s send command ok, wait for response\n", __func__);
	ret = read(diagfd, cmd_result, sizeof(cmd_result));
	LOG("%s read is: %s\n", __func__, cmd_result);
	
	//Now the Port is /dev/ttyUsB0
}

static void backup_efs()
{
}

static void backup_nv()
{
}

int main(ing argc, char **argv)
{
	int ret;
	char buf[1024] = {0};
	diagfd = get_serial_fd();
	//ret = write(diagfd, "Hello", 6);
	//LOG("Write result is: %d\n", ret);

	//off_line();
	switch_to_download();
	//reset();

	//ret = read(diagfd, buf, sizeof(buf));
	//printf("Read length is %d result is: %s\n", ret, buf);
	close(diagfd);
	return 0;
}

