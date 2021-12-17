#include "CanAdapter.h"
#include <unistd.h>

// source code on java:
// https://www.ewertenergy.com/products.php?item=candapter&page=samplecode

// manual:
// https://www.ewertenergy.com/products.php?item=candapter&page=utilities

CanAdapter::CanAdapter(int serialPort, bool extendId) : 
	_SerialPort(serialPort),
	_extendId(extendId)
{}

int CanAdapter::open(uint8_t baudRate)
{
	uint8_t data[4] = {0x15, 'S', baudRate, 0x15};
	write(_SerialPort, data, 4);
	read(_SerialPort, data, 1);

	if(data[0] != ACK)
		return -1;

	data[0] = 'O';
	data[1] = 0x15;
	write(_SerialPort, data, 2);
	read(_SerialPort, data, 1);

	if(data[0] != ACK)
		return -2;

	return 0;
}

int CanAdapter::close()
{
	uint8_t data = 0x15;
	write(_SerialPort, &data, 1);
	read(_SerialPort, &data, 1);

	if(data != ACK)
		return -2;

	return 0;
}

int CanAdapter::read(uint32_t *canId, int *dataSize, uint8_t *canData)
{
	uint8_t data[20];
	int readBytes = readCom(_SerialPort, data, 20);
	int posData = 0;

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

int CanAdapter::write(uint32_t canId, int dataSize, uint8_t *data)
{
	if(dataSize > 8)
	{
		printf("err, data size is abowe than 8\n");
		return -1;
	}

	uint8_t canData[20];
	int canSendIndex = 0;
	canData[canSendIndex++] = extendCanId ? 'x' : 't';

	if(extendCanId)
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

	int ret = write(_SerialPort, canData, canSendIndex);
	read(_SerialPort, canData, 1);

	if(canData[0] != ACK)
		return -2;

	return ret;
}

