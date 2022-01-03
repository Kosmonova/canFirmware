#include "CanAdapter.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>

// source code on java:
// https://www.ewertenergy.com/products.php?item=candapter&page=samplecode

// manual:
// https://www.ewertenergy.com/products/candapter/downloads/candapter_manual.pdf

CanAdapter::CanAdapter(ComPort *comPort, bool extendId) : 
	_comPort(comPort),
	_extendId(extendId),
	_buffPos(0)
{}

bool CanAdapter::setBaudRate(uint8_t baudRate)
{
	uint8_t data[4] = {015, 'S', '0' + baudRate, 015};
	_comPort->writeCom(data,4);
	int readBytes = _comPort->readCom(data, 1);

	return (readBytes == 1) && (data[0] == ACK);
}

bool CanAdapter::openCan()
{
	uint8_t data[5] = {015, 015, 015, 'O', 015};
	_comPort->writeCom(data, 5);
	int readBytes = _comPort->readCom(data, 1);

	return (readBytes == 1) && (data[0] == ACK);
}

bool CanAdapter::closeCan()
{
	uint8_t data[3] = {015, 'C', 015};
	_comPort->writeCom(data, 3);
	int readBytes = _comPort->readCom(data, 1);

	return (readBytes == 1) && (data[0] == ACK);
}

int CanAdapter::readCan(uint32_t *canId, uint8_t *canData, int *dataSize,
	bool *extendCanId)
{
	uint8_t data[30];
	int readBytes = 0;

// 	for(int idx = 0; idx < 30; idx++)
// 	{
// 		uint8_t dataByte;
// 
// 		if(_comPort->readCom(&dataByte, 1, false) != 1)
// 			break;
// 
// 		readBytes++;
// 		data[idx] = dataByte;
// printf("dataByte %x\n", dataByte);
// 		if(dataByte == 0x0d)
// 		{
// 			printf("dataByte 015 !!!!!!!!\n");
// 			break;
// 		}
// 	}


	readBytes = _comPort->readCom(_buffer, SIZE_BUFFER - _buffPos, false);
    _buffPos += readBytes;


	if(readBytes < 6)
	{
		*dataSize = 0;
		return *dataSize;
	}

	printf("	readBytes: %d\n", readBytes);
	int posData = 0;
	bool extendId = _extendId;
	char buff[10];

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

		extendId = *extendCanId;
	}

	posData++;

	if(extendId)
	{
		memcpy(buff, data + posData, 8);
		buff[8] = '\0';
		sscanf(buff, "%x", canId);
		posData += 8;
	}
	else
	{
		memcpy(buff, data + posData, 6);
		buff[6] = '\0';
		sscanf(buff, "%x", canId);
		posData += 6;
	}

	memcpy(buff, data + posData, 2);
	buff[2] = '\0';
	sscanf(buff, "%x", dataSize);
	posData += 2;

	if(*dataSize > 8)
		return -1;

	for(int idx = 0; idx < *dataSize; idx++)
	{
		memcpy(buff, data + posData, 2);
		buff[2] = '\0';
		sscanf(buff, "%x", canData + idx);
		posData += 2;
	}

	return *dataSize;
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

