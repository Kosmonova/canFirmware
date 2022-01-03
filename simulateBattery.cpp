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
	int idx = 0;
	uint32_t canId;

	while(true)
	{
		pcanAdapter->readCan(&canId, buff, &sizeData);
		if(sizeData != 0)
		{
			int readBytes = pcomPort->readCom(buff, SIZE_BUFFER, false);

			for(int idx = 0; idx < readBytes; idx++)
			{
				if(buff[idx] == 'O' || buff[idx] == 'C')
					pcomPort->writeCom({ACK}, 1)
			}
			printf("canId: %x, data size: %d\n", canId, sizeData);
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

