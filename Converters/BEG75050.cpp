#include <stdio.h>
#include <unistd.h>
#include "BEG75050.h"


BEG75050::BEG75050(int fdSerial) :
	ConverterAbstract(fdSerial)
{
}

void BEG75050::parse(int canId, uint8_t data[])
{
	char str[100];
	int idx;

	switch(canId & 0xffff0000)
	{
		case 0x7570000:
			break;
		case 0x02810000:
		{
			revereseArray(data, 0, 3);
			revereseArray(data + 4, 0, 3);
			float voltageIn = *((float*)data);
			float currentIn = *((float*)(data + 4));
			printf("voltage %f, current %f\n", voltageIn, currentIn);
			break;
		}
		case 0x028a0000:
		{
			revereseArray(data, 0, 1);
			revereseArray(data, 2, 3);
			revereseArray(data, 4, 5);
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
			revereseArray((uint8_t*)(&cislo), 0, 4);
			revereseArray(data, 6, 7);

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
			revereseArray(data, 0, 1);
			revereseArray(data, 2, 3);
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
			revereseArray(data, 0, 1);
			revereseArray(data, 2, 3);
			revereseArray(data, 4, 5);
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
		

			for(idx = 0; idx < 8; idx++)
			{
				printf("%x ", data[idx]);
			}

			printf("\n");
			break;
		}
	}
}

void BEG75050::setVoltage(int voltage)
{
	printf("set voltage %d\n", voltage);
}

void BEG75050::on()
{
	printf("set on BEG75050\n");
	write(_fdSerial, "\x1a\x01", 2);
}

void BEG75050::off()
{
	printf("set off BEG75050\n");
	write(_fdSerial, "\x1a\x01", 2);
}
