#include <stdint.h>
#include <stdio.h>

#include "Battery.h"

Battery::Battery()
{
}

void Battery::parse(uint32_t canId, uint8_t *data, int dataSize)
{
	if(dataSize < 8)
	{
		printf("wring data size\n");
		return;
	}

	switch(canId)
	{
		case 0x7E0FE0:
			_packChargeCurrentLimit = *(uint16_t*)(data + 3);
			_packDischargeCurrent = *(uint16_t*)(data + 5);
			_supplyVoltage12On100mV = data[7];
			break;
		case 0x7E0FE1:
			_stateChargePercent = data[0];
			_packAmpHours = data[1];
			_packVoltage = *(uint16_t*)(data + 2);
			_hightTemperatureCelsius = data[4];
			_lowTemperatureCelsius = data[5];
			_averageTemperatureCelsius = data[6];
			_bmsTemperatureCelsius = data[7];
			break;
		case 0x7E0FE2:
			_packCurrent100mA = *(uint16_t*)(data);
			_packOpenVoltage100mV = *(uint16_t*)(data + 2);
			_packSummedVoltage100mV = *(uint16_t*)(data + 6);
			break;
		case 0x7E0FE3:
			_totalPackCycles = *(uint16_t*)(data);
			_packHealthPercent = data[2];
			_packResistancemOhm = *(uint16_t*)(data + 3);
			break;
		case 0x7E0FE4:
			_lowOpenCellVoltagemV = *(uint16_t*)(data);
			_hightOpenCellVoltagemV = *(uint16_t*)(data + 2);
			_averageOpencellVoltagemV = *(uint16_t*)(data + 4);
			break;
		case 0x7E0FE6:
			_nominalPackCapacityAh = data[3];
			break;
		default:
			break;
	}
}

