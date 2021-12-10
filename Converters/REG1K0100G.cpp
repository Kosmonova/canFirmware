#include <stdio.h>
#include <string.h>
#include "REG1K0100G.h"


REG1K0100G::REG1K0100G(int fdSerial) :
	ConverterAbstract(fdSerial),
	_systemVoltage_mV(0),
	_systemCurrent_mA(0)
{
}

void REG1K0100G::_setSystemOutputValues()
{
	uint32_t id = 0x029b3ff0;
	uint8_t data[8];
	*((uint32_t*)data) = _systemVoltage_mV;
	_revereseArray(data, 0, 3);
	*((uint32_t*)(data + 4)) = _systemCurrent_mA;
	_revereseArray(data, 4, 7);
	_sendCommand(id, data);
}

void REG1K0100G::showType()
{
	printf("REG1K0100G");
}

void REG1K0100G::parse(int canId, uint8_t data[])
{
	_revereseArray((uint8_t*)&canId, 0, 3);

	switch(canId & 0xffff0000)
	{
		case 0x7570000:
			break;
		case 0x02810000:
		{
			_revereseArray(data, 0, 3);
			_revereseArray(data + 4, 0, 3);
			float voltageIn = *((float*)data);
			float currentIn = *((float*)(data + 4));
			printf("voltage %f, current %f\n", voltageIn, currentIn);
			break;
		}
		case 0x028a0000:
		{
			_revereseArray(data, 0, 1);
			_revereseArray(data, 2, 3);
			_revereseArray(data, 4, 5);
			uint16_t maxVoltage = *((uint16_t*)data);
			uint16_t minVoltage = *((uint16_t*)(data + 2));
			uint16_t maxCurrent = *((uint16_t*)(data + 4));

			printf("max voltage: %u V, "
				"min voltage: %u V, max current: %u.%.1u A\n",
				maxVoltage,
				minVoltage,
				maxCurrent / 10, maxCurrent % 10);
			break;
		}
		case 0x028b0000:
		{
			uint64_t cislo = *(uint32_t*)(data + 1);
			cislo += (*(data + 5)) * 0x100000000;
			_revereseArray((uint8_t*)(&cislo), 0, 4);
			_revereseArray(data, 6, 7);

			printf("barcode: %.3u",
				cislo / 1000000000);

			printf("%.9lu",
				cislo % 1000000000, 0x0FFF & *((uint16_t*)(data + 6)));

			printf("V%.4d\n",
					0x0FFF & *((uint16_t*)(data + 6)));
			break;
		}
		case 0x02840000:
		{
			printf("group #%d temperature %d C\n", data[2], data[4]);
			break;
		}
		case 0x028c0000:
		{
			_revereseArray(data, 0, 1);
			_revereseArray(data, 2, 3);
			uint16_t extVoltage = *((uint16_t*)data);
			uint16_t outCurrent = *((uint16_t*)(data + 2));

			printf("external voltage %u.%.1u V vailable output current: %u.%.1u A\n",
					extVoltage / 10, extVoltage % 10,
					outCurrent / 10, outCurrent % 10);
			break;
		}
		case 0x02860000:
		{
			printf("input voltatages: ");
			_revereseArray(data, 0, 1);
			_revereseArray(data, 2, 3);
			_revereseArray(data, 4, 5);
			uint16_t uAB = *((uint16_t*)data);
			uint16_t uBC = *((uint16_t*)(data + 2));
			uint16_t uCA = *((uint16_t*)(data + 4));
			printf("AB = %u.%.1u V, BC = %u.%.1u V, CA = %u.%.1u V\n", 
					uAB / 10, uAB % 10,
					uBC / 10, uBC % 10,
					uCA / 10, uCA % 10);
			break;
		}
		default:
		{
			printf("can in: ");
			printf("id: %lx ", canId);

			printf("data: ");
		

			for(int idx = 0; idx < 8; idx++)
			{
				printf("%x ", data[idx]);
			}

			printf("\n");
			break;
		}
	}
}

void REG1K0100G::setVoltage(int voltage)
{
	_systemVoltage_mV = voltage;
	_setSystemOutputValues();
}

void REG1K0100G::on()
{
	printf("set on REG1K0100G\n");
	uint32_t id = 0x029a3ff0;
	uint8_t data[8];
	memset(data, 0, 8);
	_sendCommand(id, data);
}

void REG1K0100G::off()
{
	printf("set off REG1K0100G\n");
	uint32_t id = 0x029a3ff0;
	uint8_t data[8];
	memset(data, 0, 8);
	data[0] = 0x01;
	_sendCommand(id, data);
}
