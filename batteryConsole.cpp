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
	uint32_t canId;

	while(true)
	{
		pcanAdapter->readCan(&canId, buff, &sizeData);
		if(sizeData > 0)
		{
			pbattery->parse(canId, buff, sizeData);
		}
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

void printAll(Battery *battery)
{
	printf("\tCharger current packet limit: %d A\n",
		battery->getPackChargeCurrentLimit());
	printf("\tDischarge current packet: %d A\n",
		   battery->getPackDischargeCurrent());
	printf("\tSupply 12 votage: %.1f V\n", battery->getSupplyVoltage12());
	printf("\tState charge : %d %\n", battery->getStateChargePercent());
	printf("\tPacket capacity : %d Ah\n", battery->getPackAmpHours());
	printf("\tPacket voltage : %d V\n", battery->getPackVoltage());
	printf("\tHigh temperature : %d °C\n",
		   battery->getHightTemperatureCelsius());
	printf("\tLow temperature : %d °C\n", battery->getLowTemperatureCelsius());
	printf("\tAverage temperature : %d °C\n",
		battery->getAverageTemperatureCelsius());
	printf("\tBMS temperature : %d °C\n", battery->getBmsTemperatureCelsius());
	printf("\tPacket Current : %.1f A\n", battery->getPackCurrent());
	printf("\tPacket open voltage : %.1f V\n", battery->getPackOpenVoltage());
	printf("\tPacket summary voltage : %.1f V\n",
		battery->getPackSummedVoltage());
	printf("\tTotal package cycles : %d\n", battery->getTotalPackCycles());
	printf("\tPacket health : %d %\n", battery->getPackHealthPercent());
	printf("\tPacket resistance : %.3f Ohm\n", battery->getPackResistanceOhm());
	printf("\tLow open cell voltage : %.3f V\n",
		battery->getLowOpenCellVoltage());
	printf("\tHigh open cell voltage : %.3f V\n",
		battery->getHightOpenCellVoltage());
	printf("\tAverage open cell voltage : %.3f V\n",
		battery->getAverageOpencellVoltage());
	printf("\tNominal Packet Capacity : %d Ah\n",
		battery->getNominalPackCapacityAh());
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

// reading of garbage non readed bytes after previous communication
// 	uint8_t tmp[10];
// 	while(comPort.readCom(tmp, 10, false) == 10);

	CanAdapter canAdapter(&comPort, true);

	if(!canAdapter.closeCan())
	{
		printf("error close can adpapter\n");
// 		comPort.closeCom();
// 		return -2;
	}

	if(!canAdapter.setBaudRate(canBitRate))
	{
		printf("error set baudrate on can adpapter\n");
		comPort.closeCom();
		return -2;
	}

	if(!canAdapter.openCan())
	{
		printf("error open can adapter\n");
		comPort.closeCom();
		return -2;
	}

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
		else if(matchCommand(&pCommandStart, "all"))
			printAll(&battery);
		else if(nextCommand(&pCommandStart) < 0)
			rdBytes = 0;
	}

	canAdapter.closeCan();
	comPort.closeCom();

	return 0;
}


