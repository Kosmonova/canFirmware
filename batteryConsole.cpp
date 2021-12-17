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
	int fdSerial;
// 	ConverterBase **ppConverter;
};

void *readThread(void* arg)
{
	int fd = ((struct ThreadData*)arg)->fdSerial;
	uint8_t buff[SIZE_BUFFER];
	int idx = 0;

// 	while(1)
// 	{
// // 		int numRcvBytes = readCom(fd, buff, SIZE_BUFFER);
// 
// // 		if(numRcvBytes)
// 		{
// // 			printf("\n");
// 
// // 		for(idx = 0; idx < numRcvBytes; idx++)
// // 		{
// // 			printf("%c", buff[idx]);
// // 		}
// 			if(numRcvBytes == 12)
// 				/*(*ppConverter)->parse(*((uint32_t*)buff), buff + 4)*/;
// 			else
// 				printf("wrong data received\n");
// 		}
// 	}
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
	ComPort comPort;

	if(comPort.openCom(portname, B921600) < 0)
		return -1;

	CanAdapter canAdapter(&comPort, true);
	canAdapter.openCan(canBitRate);

	return 0;
}


