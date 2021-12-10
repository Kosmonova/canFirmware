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

#include "REG1K0100G.h"
#include "BEG75050.h"
#include "CEG1K0100G.h"
#include "UXR100030.h"


#define SIZE_BUFFER 100
#define error_message printf

struct ThreadData
{
	int fdSerial;
	ConverterAbstract *pConverter;
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

char portname[20];

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

int openComPort(int *fd)
{
	*fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);

	if (*fd < 0)
	{
		error_message ("error %d opening %s: %s\n", errno, portname, strerror (errno));
		return -1;
	}

	set_interface_attribs (*fd, B9600, 0);  // set speed to 115,200 bps, 8n1 (no parity)
	set_blocking (*fd, 0); 

	return 0;
}

void *readThread(void* arg)
{
	int fd = ((struct ThreadData*)arg)->fdSerial;
	ConverterAbstract *pConverter = ((struct ThreadData*)arg)->pConverter;

	uint8_t buff[SIZE_BUFFER];
	int idx = 0;

	while(1)
	{
		int numRcvBytes = readCom(fd, buff, SIZE_BUFFER);
		for(idx = 0; idx < numRcvBytes; idx++)
		{
			printf("%x, ", buff[idx]);
		}

		if(numRcvBytes)
		{
			printf("\n");

			if(numRcvBytes == 12)
				pConverter->parse(*((int*)buff), buff + 4);
			else
				printf("wrong data received\n");
		}
	}
}

void printHelp()
{
	printf("usage:\n");
	printf("\thelp print this message\n");
	printf("\texit exit from this program\n");
	printf("\tset <on|off>, turn on or off converter\n");
	printf("\tset_voltage <voltage> ,where voltage is in Volt unit integer\n");
	printf("\tchange_type <converter>,\n");
	printf("\t\t<where converter is BEG75050 or CEG1K0100G or UXR100030 or REG1K0100G\n");
	printf("\tcurrent_type show current type of converter\n");
}

int nextCommand(char **ppCommandStart)
{
	char *pCommandStart = *ppCommandStart;
	char *findChar = strchr(pCommandStart, ' ');

	if(!findChar)
	{
		findChar = strchr(pCommandStart, '\n');
		return -1;
	}

	int lenString = findChar - pCommandStart;

	*ppCommandStart += lenString + 1;
	return 0;
}

int matchCommand(char **ppCommandStart, const char *command)
{
	char *pCommandStart = *ppCommandStart;
	char *findChar = strchr(pCommandStart, ' ');

	if(!findChar)
		findChar = strchr(pCommandStart, '\n');

	int lenString = findChar - pCommandStart;

	if(strlen(command) != lenString)
		return 0;

	if(strncmp(pCommandStart, command, lenString - 1) != 0)
		return 0;

	*ppCommandStart += lenString + 1;
	return 1;
}

int main(int argc, char *argv[])
{
	printf("hello world!!\n");
	if(argc == 2)
		strcpy(portname, argv[1]);
	else
		strcpy(portname, "/dev/ttyUSB0");

	printf("trying open %s\n", portname);
	int fd;

	if(openComPort(&fd) < 0)
		return -1;

	ConverterAbstract *pConverter = new BEG75050(fd);
	
	pthread_t threadID;
	struct ThreadData threadData = {.fdSerial = fd, .pConverter = pConverter};
	
	pthread_create(&threadID, NULL, readThread, (void *)&threadData);
	char buff[SIZE_BUFFER];
	char *pCommandStart = buff;
	int rdBytes = 0;

	while(1)
	{
		if(rdBytes == 0 || pCommandStart - buff > rdBytes)
		{
			printf("wait for new command...\n");
			fgets(buff, SIZE_BUFFER, stdin);
			rdBytes = strlen(buff);
			pCommandStart = buff;
		}

		if(matchCommand(&pCommandStart, "exit"))
			break;
		else if(matchCommand(&pCommandStart, "help"))
			printHelp();
		else if(matchCommand(&pCommandStart, "set"))
		{
			if(matchCommand(&pCommandStart, "on"))
				pConverter->on();
			else if(matchCommand(&pCommandStart, "off"))
				pConverter->off();
		}
		else if(matchCommand(&pCommandStart, "set_voltage"))
		{
			int value;
			sscanf(pCommandStart, "%d", &value);
			pConverter->setVoltage(value);
		}
		else if(matchCommand(&pCommandStart, "change_type"))
		{
			if(matchCommand(&pCommandStart, "REG1K0100G"))
			{
				delete pConverter;
				pConverter = new REG1K0100G(fd);
			}
			else if(matchCommand(&pCommandStart, "BEG75050"))
			{
				delete pConverter;
				pConverter = new BEG75050(fd);
			}
			else if(matchCommand(&pCommandStart, "CEG1K0100G"))
			{
				delete pConverter;
				pConverter = new CEG1K0100G(fd);
			}
			else if(matchCommand(&pCommandStart, "UXR100030"))
			{
				delete pConverter;
				pConverter = new UXR100030(fd);
			}
		}
		else if(matchCommand(&pCommandStart, "current_type"))
		{
			printf("current type: ");
			pConverter->showType();
			printf("\n");
		}
		else if(nextCommand(&pCommandStart) < 0)
			rdBytes = 0;
	}

	close(fd);

	return 0;
}

