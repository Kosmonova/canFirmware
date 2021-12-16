// source code on java:
// https://www.ewertenergy.com/products.php?item=candapter&page=samplecode

// manual:
// https://www.ewertenergy.com/products.php?item=candapter&page=utilities

#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h> 
#include <stdio.h>
#include <string.h>
#include <stdarg.h> 
#include <poll.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#include "Battery.h"

#define SIZE_BUFFER 100
#define error_message printf

#define CAN_10Kb  0
#define CAN_20Kb  1
#define CAN_50Kb  2
#define CAN_100Kb 3
#define CAN_125Kb 4
#define CAN_250Kb 5
#define CAN_500Kb 6
#define CAN_800Kb 7
#define CAN_1Mb   8

struct ThreadData
{
	int fdSerial;
// 	ConverterBase **ppConverter;
};


int
set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        if (tcgetattr (fd, &tty) != 0)
        {
                error_message ("error %d from tcgetattr\n", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                error_message ("error %d from tcsetattr\n", errno);
                return -1;
        }
        return 0;
}

void
set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                error_message ("error %d from tggetattr\n", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                error_message ("error %d setting term attributes\n", errno);
}

int openComPort(char *portname, int *fd)
{
	*fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);

	if (*fd < 0)
	{
		error_message ("error %d opening %s: %s\n", errno, portname, strerror (errno));
		return -1;
	}

	set_interface_attribs (*fd, B921600, 0);  // set speed to 115,200 bps, 8n1 (no parity)
	set_blocking (*fd, 0); 

	return 0;
}

int openCanPort(int fdSerial, uint8_t baudRate)
{
	uint8_t data[4] = {0x15, 'S', baudRate, 0x15};
	write(fdSerial, data, 4);
}

int readCom(int SerialHandle, uint8_t * pBuff, uint32_t BytesToRead)
{
	struct pollfd fd = { .fd = SerialHandle, .events = POLLIN };
    size_t      bytesread = 0;

    while (poll (&fd, 1, 25) == 1)
    {
        int chunksize = read (SerialHandle, pBuff + bytesread, BytesToRead);
        if (chunksize == -1)
            break;

        bytesread += chunksize;
        BytesToRead -= chunksize;

        if (BytesToRead == 0)
           break;
    }

	return bytesread;
}

void *readThread(void* arg)
{
	int fd = ((struct ThreadData*)arg)->fdSerial;
	uint8_t buff[SIZE_BUFFER];
	int idx = 0;

	while(1)
	{
		int numRcvBytes = readCom(fd, buff, SIZE_BUFFER);

		if(numRcvBytes)
		{
// 			printf("\n");

// 		for(idx = 0; idx < numRcvBytes; idx++)
// 		{
// 			printf("%c", buff[idx]);
// 		}
			if(numRcvBytes == 12)
				/*(*ppConverter)->parse(*((uint32_t*)buff), buff + 4)*/;
			else
				printf("wrong data received\n");
		}
	}
}

int main(int argc, char *argv[])
{
	char portname[20];
	uint8_t canBitRate = CAN_125Kb;

	if(argc >= 2)
		strcpy(portname, argv[1]);
	else
		strcpy(portname, "/dev/ttyUSB0");

	if(argc >= 3)
		sscanf(argv[2], "%u", &canBitRate);

	if(canBitRate > 8)
		canBitRate = 8;

	printf("trying open %s\n", portname);
	printf("trying can bus set on: %d\n", canBitRate);
	int fd;

	if(openComPort(portname, &fd) < 0)
		return -1;

	openCanPort(fd, canBitRate);

	return 0;
}


