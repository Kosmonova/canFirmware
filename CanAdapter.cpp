#include "CanAdapter.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>

// source code on java:
// https://www.ewertenergy.com/products.php?item=candapter&page=samplecode

// manual:
// https://www.ewertenergy.com/products.php?item=candapter&page=utilities

CanAdapter::CanAdapter(ComPort *comPort, bool extendId) : 
	_comPort(comPort),
	_extendId(extendId)
{}

int CanAdapter::openCan(uint8_t baudRate)
{
	uint8_t data[4] = {0x15, 'S', baudRate, 0x15};
	_comPort->writeCom(data, 4);
	_comPort->readCom(data, 1);

	if(data[0] != ACK)
		return -1;

	data[0] = 'O';
	data[1] = 0x15;
	_comPort->writeCom(data, 2);
	_comPort->readCom(data, 1);

	if(data[0] != ACK)
		return -2;

	return 0;
}

int CanAdapter::closeCan()
{
	uint8_t data = 0x15;
	_comPort->writeCom(&data, 1);
	_comPort->readCom(&data, 1);

	if(data != ACK)
		return -2;

	return 0;
}

int CanAdapter::readCan(uint32_t *canId, uint8_t *canData, int *dataSize,
	bool *extendCanId)
{
	uint8_t data[20];
	int readBytes = _comPort->readCom(data, 20);
	int posData = 0;

	if(extendCanId != nullptr)
	{
		switch(data[0])
		{
			case 'x':
			case 'X':
					*extendCanId = true;
					break;
			case 't':
			case 'T':
					*extendCanId = false;
					break;
			default:
				return -1;
		}
	}

	posData++;

	if(*extendCanId)
	{
		*canId = *(uint32_t*)(data + posData);
		posData += 4;
	}
	else
	{
		*canId = (*(uint32_t*)(data + posData)) & 0x7FF;
		posData += 3;
	}

	*dataSize = data[posData++];
	memcpy(canData, data + posData, *dataSize);

	return 0;
}

int CanAdapter::writeCan(uint32_t canId, int dataSize, uint8_t *data)
{
	if(dataSize > 8)
	{
		printf("err, data size is abowe than 8\n");
		return -1;
	}

	uint8_t canData[20];
	int canSendIndex = 0;
	canData[canSendIndex++] = _extendId ? 'x' : 't';

	if(_extendId)
	{
		*(uint32_t*)(canData + canSendIndex) = canId;
		canSendIndex += 4;
	}
	else
	{
		*(uint32_t*)(canData + canSendIndex) = canId & 0x7FF;
		canSendIndex += 3;
	}

	canData[canSendIndex++] = dataSize;
	memcpy(canData + canSendIndex, data, dataSize);
	canSendIndex += dataSize;
	canData[canSendIndex++] = 0x15;

	int ret = _comPort->writeCom(canData, canSendIndex);
	_comPort->readCom(canData, 1);

	if(canData[0] != ACK)
		return -2;

	return ret;
}

