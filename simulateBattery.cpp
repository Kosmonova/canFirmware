#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h> 
#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>


#include "CanAdapter.h"
#include "Battery.h"

#define SIZE_BUFFER 100


struct ThreadData
{
	CanAdapter *pcanAdapter;
	ComPort *pcomPort;
};

void *readThread(void* arg)
{
	CanAdapter *pcanAdapter = ((struct ThreadData*)arg)->pcanAdapter;
	ComPort *pcomPort = ((struct ThreadData*)arg)->pcomPort;

	uint8_t buff[SIZE_BUFFER];
	int sizeData;
	uint32_t canId;
	int idx;
	bool canOpened = false;

	while(true)
	{
		int readBytes = pcomPort->readCom(buff, SIZE_BUFFER, false);

		if(canOpened)
		{
			pcanAdapter->writeCan(0x7E0FE0, 8,
				(uint8_t *)"\x00\x00\x00\x03\x00\x07\x00\x08");
		}

		if(readBytes > 0)
		{
			for(idx = 0; idx < readBytes; idx++)
			{
				if(buff[idx] == 'O')
				{
					pcomPort->writeCom((uint8_t*)"\x06", 1);
					canOpened = true;
					printf("send ACK on open Can\n");
				}
				else if(buff[idx] == 'S')
				{
					pcomPort->writeCom((uint8_t*)"\x06", 1);
					printf("send ACK on set baudrate Can\n");
				}
				else if(buff[idx] == 'C')
				{
					pcomPort->writeCom((uint8_t*)"\x06", 1);
					canOpened = false;
					printf("send ACK on close Can\n");
				}
				else
					continue;

				break;
			}
		}
	}
}


int main(int argc, char *argv[])
{
	char portname[20];

	if(argc >= 2)
		strcpy(portname, argv[1]);
	else
		strcpy(portname, "/dev/ttyUSB0");

	printf("trying open %s\n", portname);
	ComPort comPort;

	if(comPort.openCom(portname, B921600) < 0)
		return -1;

	CanAdapter canAdapter(&comPort, true);

	pthread_t threadID;
	struct ThreadData threadData = {.pcanAdapter = &canAdapter,
		.pcomPort = &comPort};

	pthread_create(&threadID, NULL, readThread, (void *)&threadData);

	char buff[SIZE_BUFFER];
	char *pCommandStart = buff;
	int rdBytes = 0;

	while(true)
	{
	}

	return 0;
}

