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

	char canData[COUNT_ASCII_EXTEND_PACKET_ID + 8 * COUNT_ASCII_BYTE + 1];
	int canDataSendPosFill = 0;
	canData[canDataSendPosFill++] = _extendId ? 'X' : 'T';

	if(_extendId)
	{
		sprintf(canData + canDataSendPosFill, "%.8X", canId);
		canDataSendPosFill += COUNT_ASCII_EXTEND_PACKET_ID;
	}
	else
	{
		sprintf(canData + canDataSendPosFill, "%.6X", canId & 0x7FF);
		canDataSendPosFill += COUNT_ASCII_STANDART_PACKET_ID;
	}

	sprintf(canData + canDataSendPosFill, "%.2X", dataSize);
	canData[canDataSendPosFill++] = COUNT_ASCII_BYTE;

	for(int idx = 0; idx < dataSize; idx++)
	{
		sprintf(canData + canDataSendPosFill, "%.2X", dataSize);
		canDataSendPosFill += COUNT_ASCII_BYTE;
	}

	canData[canDataSendPosFill++] = CR;

	int ret = _comPort->writeCom((uint8_t *)canData, canDataSendPosFill);
	_comPort->readCom((uint8_t *)canData, 1);

printf("send data: %s\n", canData);
	if(canData[0] != ACK)
		return -2;
	return ret;
}

void CanAdapter::flush()
{
	_bufferPosFill = 0;
}
