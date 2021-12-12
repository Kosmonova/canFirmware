#include <stdio.h>
#include <string.h>
#include <byteswap.h>

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

void UXR100030::_generateSetMdlData(uint8_t *data, uint16_t reg, uint32_t value)
{
	memset(data, 0, 8);
	data[0] = 0x03;
	*((uint16_t*)(data + 2)) = __bswap_16(reg);
	*((uint32_t*)(data + 4)) = __bswap_32(value);
	
	printf("data:\n");
	for(int idx = 0; idx < 8 ; idx ++)
	{
		printf("%0x, ", data[idx]);
	}
	printf("\n");
	
}

void UXR100030::_generateReadMdlData(uint8_t *data, uint16_t reg, uint32_t value)
{
	memset(data, 0, 8);
	data[0] = 0x10;
	*((uint16_t*)(data + 2)) = __bswap_16(reg);
	*((uint32_t*)(data + 4)) = __bswap_32(value);
	
	printf("data:\n");
	for(int idx = 0; idx < 8 ; idx ++)
	{
		printf("%0x, ", data[idx]);
	}
	printf("\n");

}

uint8_t UXR100030::_getAddressFromId(uint32_t id)
{
	return (id & 0x7F8) / 8;
}

void UXR100030::showType()
{
	printf("UXR100030");
}

void UXR100030::parse(int canId, uint8_t data[])
{
	if(_address != _getAddressFromId(canId))
		return;
}

void UXR100030::setVoltage(uint32_t voltage)
{
	uint8_t data[8];
	float voltageF = voltage;
	_generateSetMdlData(data, SET_MODULE_OUTPUT_VOLTAGE, *(uint32_t*)&(voltageF));
	_sendCommand(_generateId(), data);
}

void UXR100030::getVoltage()
{
	uint8_t data[8];
	_generateReadMdlData(data, GET_MODULE_VOLTAGE, 0);
	_sendCommand(_generateId(), data);	
}

void UXR100030::setCurrent(uint32_t current)
{
	uint8_t data[8];
	_generateSetMdlData(data, SET_MODULE_OUTPUT_CURRENT, current * 1024);
	_sendCommand(_generateId(), data);
}

void UXR100030::setCurrentLimitPoint(float point)
{
	uint8_t data[8];
	_generateSetMdlData(data, SET_MODULE_CURRENT_LIMIT_POINT, *(uint32_t*)&point);
	_sendCommand(_generateId(), data);
}

void UXR100030::on()
{
	printf("set on UXR100030\n");
	uint8_t data[8];
	_generateSetMdlData(data, POWER_ON_OFF, 0);
	_sendCommand(_generateId(), data);
}

void UXR100030::off()
{
	printf("set off UXR100030\n");
	uint8_t data[8];
	_generateSetMdlData(data, POWER_ON_OFF, 0x00010000);
	_sendCommand(_generateId(), data);
}
