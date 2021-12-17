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
	Battery *pbattery;
};

void *readThread(void* arg)
{
	CanAdapter *pcanAdapter = ((struct ThreadData*)arg)->pcanAdapter;
	Battery *pbattery = ((struct ThreadData*)arg)->pbattery;

	uint8_t buff[SIZE_BUFFER];
	int sizeData;
	int idx = 0;
	uint32_t canId;

	while(true)
	{
		pcanAdapter->readCan(&canId, buff, &sizeData);
		if(sizeData != 0)
			pbattery->parse(canId, buff, sizeData);
	}
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

void printHelp()
{
	printf("usage:\n");
	printf("\thelp print this message\n");
	printf("\tall print all information about battery\n");
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
	Battery battery;

	pthread_t threadID;
	struct ThreadData threadData = {.pcanAdapter = &canAdapter,
		.pbattery = &battery};

	pthread_create(&threadID, NULL, readThread, (void *)&threadData);

	char buff[SIZE_BUFFER];
	char *pCommandStart = buff;
	int rdBytes = 0;

	while(true)
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
		else if(nextCommand(&pCommandStart) < 0)
			rdBytes = 0;
	}

	canAdapter.closeCan();
	comPort.closeCom();

	return 0;
}


