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

#include "CanAdapter.h"
#include "CommCanAbst.h"

#include "REG1K0100G.h"
#include "BEG75050.h"
#include "CEG1K0100G.h"
#include "UXR100030.h"


#define SIZE_BUFFER 100
#define error_message printf

class MyBoard : public CommCanAbst
{
	private:
		ComPort *_comPort;
	public:
		MyBoard(ComPort *comPort) : _comPort(comPort)
		{}
		void readCan(uint32_t *id, uint8_t *data, int *size)
		{
			_comPort->readCom((uint8_t *)id, 4);
			*size = _comPort->readCom(data, 8);
		}
		void writeCan(uint32_t id, uint8_t *data, int size)
		{
			_comPort->writeCom((uint8_t *)&id, 4);
			_comPort->writeCom(data, size);
		}
};

struct ThreadData
{
	ConverterBase **ppConverter;
};

void *tempReadThread(void* arg)
{
	ConverterBase **ppConverter = ((struct ThreadData*)arg)->ppConverter;
	while(true)
	{
		(*ppConverter)->readTempCyclic(0xFF);
// 		usleep(100000);
		sleep(2);
	}
}

void *readThread(void* arg)
{
	ConverterBase **ppConverter = ((struct ThreadData*)arg)->ppConverter;

	uint8_t buff[SIZE_BUFFER];
	int sizeData = 0;
	uint32_t id;

	while(1)
	{
		(*ppConverter)->readData(&id, buff, &sizeData);
// 		for(int idx = 0; idx < numRcvBytes; idx++)
// 		{
// 			printf("%x, ", buff[idx]);
// 		}

		if(sizeData)
		{
// 			printf("\n");

// 		for(int idx = 0; idx < numRcvBytes; idx++)
// 		{
// 			printf("%c", buff[idx]);
// 		}
			if(sizeData <= 8 && sizeData > 0)
				(*ppConverter)->parse(id, buff);
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
	printf("\tset_voltage <voltage> ,where voltage is in mili Volt unit unsigned integer\n");
	printf("\tset_current <current> ,where current is in mili Amper unit unsigned integer\n");
	printf("\tset_current_limit_point <point> ,where point is pertentagle float\n");
	printf("\tset_input_mode <mode>, where mode is AC or DC\n");
	printf("\tset_address set address of converter\n");
	printf("\tread_temp send request for read temperature\n");
	printf("\tread_input_voltages send request for read input phase voltages\n");
	printf("\tread_power read power in W\n");
	printf("\tread_status read current alarm/status\n");
	printf("\tread_output_current read current on output in Ampers\n");
	printf("\tread_output_voltage read voltage on output in Volt\n");
	printf("\tread_current_address read current address of converter\n");
	printf("\tchange_type <converter, address>,\n");
	printf("\t\twhere converter is BEG75050 or CEG1K0100G or UXR100030 or REG1K0100G\n");
	printf("\t\taddress is number from <0-63>\n");
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
	bool isUsedCanAdapter = false;
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
	CanAdapter *pCanAdapter = nullptr;
	CommCanAbst *pComm;

	isUsedCanAdapter = canBitRate != 0;

	if(canBitRate < 0)
	{
		if(comPort.openCom(portname, B57600) < 0)
			return -1;

		pComm = new MyBoard(&comPort);
	}
	else
	{
		pCanAdapter = new CanAdapter(&comPort, true);
		if(comPort.openCom(portname, B921600) < 0)
			return -1;

		if(!pCanAdapter->closeCan())
		{
			printf("error close can adpapter\n");
			comPort.closeCom();
			return -2;
		}

		if(!pCanAdapter->setBaudRate(canBitRate))
		{
			printf("error set baudrate on can adpapter\n");
			comPort.closeCom();
			return -2;
		}

		if(!pCanAdapter->openCan())
		{
			printf("error open can adapter\n");
			comPort.closeCom();
			return -2;
		}
		pComm = pCanAdapter;
	}

// reading of garbage non readed bytes after previous communication
	uint8_t tmp[10];
	while(comPort.readCom(tmp, 10, false) == 10);

	ConverterBase *pConverter = new UXR100030(pComm, 1);

	pthread_t threadID;
	struct ThreadData threadData = {.ppConverter = &pConverter};

	pthread_create(&threadID, NULL, readThread, (void *)&threadData);
	pthread_create(&threadID, NULL, tempReadThread, (void *)&threadData);
	char buff[SIZE_BUFFER];
	char *pCommandStart = buff;
	int rdBytes = 0;
	unsigned int valueInt;
	float valueFloat;

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
			sscanf(pCommandStart, "%u", &valueInt);
			pConverter->setVoltage(valueInt);
		}
		else if(matchCommand(&pCommandStart, "set_input_mode"))
		{
			if(matchCommand(&pCommandStart, "AC"))
				pConverter->setModuleInputMode(AC_MODE);
			if(matchCommand(&pCommandStart, "DC"))
				pConverter->setModuleInputMode(DC_MODE);
		}
		else if(matchCommand(&pCommandStart, "read_output_voltage"))
		{
			pConverter->readOuputVotage();
		}
		else if(matchCommand(&pCommandStart, "set_current"))
		{
			sscanf(pCommandStart, "%u", &valueInt);
			pConverter->setCurrent(valueInt);
		}
		else if(matchCommand(&pCommandStart, "set_current_limit_point"))
		{
			sscanf(pCommandStart, "%f", &valueFloat);
			pConverter->setCurrentLimitPoint(valueFloat);
		}
		else if(matchCommand(&pCommandStart, "read_temp"))
		{
			pConverter->readTemperature();
		}
		else if(matchCommand(&pCommandStart, "read_input_voltages"))
		{
			pConverter->readInputVoltage();
		}
		else if(matchCommand(&pCommandStart, "read_power"))
		{
			pConverter->readPower();
		}
		else if(matchCommand(&pCommandStart, "read_status"))
		{
			pConverter->readStatus();
		}
		else if(matchCommand(&pCommandStart, "read_output_current"))
		{
			pConverter->readOutputCurrent();
		}
		else if(matchCommand(&pCommandStart, "set_address"))
		{
			int address;
			sscanf(pCommandStart, "%u", &address);
			pConverter->changeAddress(address);
		}
		else if(matchCommand(&pCommandStart, "read_current_address"))
		{
			printf("current address is: %x\n",
				pConverter->getAddress());
		}
		else if(matchCommand(&pCommandStart, "change_type"))
		{
			int address;

			if(matchCommand(&pCommandStart, "REG1K0100G"))
			{
				sscanf(pCommandStart, "%u", &address);
				delete pConverter;
				pConverter = new REG1K0100G(pComm, address);
			}
			else if(matchCommand(&pCommandStart, "BEG75050"))
			{
				sscanf(pCommandStart, "%u", &address);
				delete pConverter;
				pConverter = new BEG75050(pComm, address);
			}
			else if(matchCommand(&pCommandStart, "CEG1K0100G"))
			{
				sscanf(pCommandStart, "%u", &address);
				delete pConverter;
				pConverter = new CEG1K0100G(pComm, address);
			}
			else if(matchCommand(&pCommandStart, "UXR100030"))
			{
				sscanf(pCommandStart, "%u", &address);
				delete pConverter;
				pConverter = new UXR100030(pComm, address);
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

	if(pCanAdapter)
		pCanAdapter->closeCan();

	comPort.closeCom();

	return 0;
}

