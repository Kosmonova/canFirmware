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

#define SIZE_BUFFER 100
#define error_message printf

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
	int fd = (int)arg;
	uint8_t buff[SIZE_BUFFER];
	int idx = 0;

	while(1)
	{
		int numRcvBytes = readCom(fd, buff, SIZE_BUFFER);
		for(idx = 0; idx < numRcvBytes; idx++)
		{
			printf("%c", buff[idx]);
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
}

int matchCommand(char *pCommandStart, char *command, int lenString)
{
	if(strlen(command) != lenString)
		return 0;

	return strncmp(pCommandStart, command, lenString - 1) == 0;
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

	pthread_t* threadID;
	pthread_create(&threadID, NULL, readThread, (void *)fd);
	uint8_t buff[SIZE_BUFFER];
	uint8_t *pCommandStart = 0;
	int idxBuff = 0;
	int rdBytes = 0;

	while(1)
	{
		if(rdBytes == 0 || idxBuff > rdBytes)
		{
			printf("wait for new command...\n");
			fgets(buff, SIZE_BUFFER, stdin);
			rdBytes = strlen(buff);
		}

		if(buff[0] == '\n')
		{
			rdBytes = 0;
			idxBuff = 0;
			continue;
		}

		pCommandStart = buff + idxBuff;
		uint8_t *findChar = strchr(pCommandStart, ' ');
		if(!findChar)
			findChar = strchr(pCommandStart, '\n');

		int lenString = findChar - pCommandStart;

		if(!findChar || lenString > SIZE_BUFFER)
		{
			printf("%s\n", pCommandStart);
			rdBytes = 0;
			idxBuff = 0;
			continue;
		}

// printf("lenString: %d\n", lenString);

		if(matchCommand(pCommandStart, "exit", lenString))
			break;

		if(matchCommand(pCommandStart, "help", lenString))
			printHelp();

		if(matchCommand(pCommandStart, "set", lenString))
			write(fd, "\x1a\x01", 2);

		idxBuff += lenString + 1;
// 		printf("1 %s lenString: %d\n", pCommandStart, lenString);
	}

	close(fd);

	return 0;
}

