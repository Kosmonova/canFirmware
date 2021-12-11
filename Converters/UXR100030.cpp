#include <stdio.h>
#include <string.h>
#include "UXR100030.h"


UXR100030::UXR100030(int fdSerial, int address) :
	ConverterBase(fdSerial, address)
{
}

uint32_t UXR100030::_generateId()
{
	uint32_t id;

	if(_broadcast)
		id = 0x60 * 0x100000 + 0x80000 + 0x800 * 0xFF + 8 * 0xF0;
	else
		id = 0x60 * 0x10000 + 0x800 * _address + 8 * 0xF0;

	return id;
}

void UXR100030::showType()
{
	printf("UXR100030");
}

void UXR100030::parse(int canId, uint8_t data[])
{
}

void UXR100030::setVoltage(uint32_t voltage)
{
	printf("set voltage %d\n", voltage);
}

void UXR100030::setCurrent(uint32_t current)
{
}

void UXR100030::on()
{
	printf("set on UXR100030\n");
	uint8_t data[8];
	memset(data, 0, 8);
	data[0] = 0x03;
	data[3] = 0x30;
	_sendCommand(_generateId(), data);
}

void UXR100030::off()
{
	printf("set off UXR100030\n");
	uint8_t data[8];
	memset(data, 0, 8);
	data[0] = 0x03;
	data[3] = 0x30;
	data[5] = 0x01;
	_sendCommand(_generateId(), data);
}
