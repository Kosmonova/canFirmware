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
	_bufferPosFill(0)
{}

bool CanAdapter::setBaudRate(uint8_t baudRate)
{
	uint8_t data[4] = {CR, 'S', '0' + baudRate, CR};
	_comPort->writeCom(data,4);
	int readBytes = _comPort->readCom(data, 1);

	return (readBytes == 1) && (data[0] == ACK);
}

bool CanAdapter::openCan()
{
	uint8_t data[5] = {CR, CR, CR, 'O', CR};
	_comPort->writeCom(data, 5);
	int readBytes = _comPort->readCom(data, 1);

	return (readBytes == 1) && (data[0] == ACK);
}

bool CanAdapter::closeCan()
{
	uint8_t data[3] = {CR, 'C', CR};
	_comPort->writeCom(data, 3);
	int readBytes = _comPort->readCom(data, 1);

	return (readBytes == 1) && (data[0] == ACK);
}

int CanAdapter::readCan(uint32_t *canId, uint8_t *canData, int *dataSize,
	bool *isExtedId)
{
	char data[30];
	int dataLen = 0;
	int posData = 0;
	char tmp[10];

	if(_bufferPosFill >= SIZE_BUFFER)
		_bufferPosFill = 0;

	_bufferPosFill += _comPort->readCom(_buffer + _bufferPosFill,
		SIZE_BUFFER - _bufferPosFill, false);

	for(int idx = 0; idx < _bufferPosFill; idx++)
	{
		if(_buffer[idx] == CR)
		{
			dataLen = idx + 1;
			memcpy(data, _buffer, dataLen);
			_bufferPosFill -= dataLen;
			memcpy(_buffer, _buffer + dataLen, _bufferPosFill);
			break;
		}
	}

	bool extendPacket;

	switch(data[0])
	{
		case 'x':
		case 'X':
				extendPacket = true;
				break;
		case 't':
		case 'T':
				extendPacket = false;
				break;
		default:
			return -1;
	}

	if(isExtedId)
		*isExtedId = extendPacket;

	posData++;

	if(extendPacket)
	{
		sscanf(strncpy(tmp, data + posData, COUNT_ASCII_EXTEND_PACKET_ID),
			"%x", canId);
		posData += COUNT_ASCII_EXTEND_PACKET_ID;
	}
	else
	{
		sscanf(strncpy(tmp, data + posData, COUNT_ASCII_STANDART_PACKET_ID),
			"%x", canId);
		posData += COUNT_ASCII_STANDART_PACKET_ID;
	}

	sscanf(strncpy(tmp, data + posData, COUNT_ASCII_BYTE), "%x", dataSize);
	posData += COUNT_ASCII_BYTE;

	if(*dataSize > 8)
		return -1;

	for(int idx = 0; idx < *dataSize; idx++)
	{
		sscanf(strncpy(tmp, data + posData, COUNT_ASCII_BYTE), "%x", canData + idx);
		posData += COUNT_ASCII_BYTE;
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

void CanAdapter::flush()
{
	_bufferPosFill = 0;
}
