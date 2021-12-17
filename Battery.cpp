#include <stdint.h>
#include <stdio.h>

#include "Battery.h"

Battery::Battery() :
	_packChargeCurrentLimit(-1),
	_packDischargeCurrent(-1),
	_supplyVoltage12On100mV(-1),
	_stateChargePercent(-1),
	_packAmpHours(-1),
	_packVoltage(-1),
	_hightTemperatureCelsius(-1),
	_lowTemperatureCelsius(-1),
	_averageTemperatureCelsius(-1),
	_bmsTemperatureCelsius(-1),
	_packCurrent100mA(-1),
	_packOpenVoltage100mV(-1),
	_packSummedVoltage100mV(-1),
	_totalPackCycles(-1),
	_packHealthPercent(-1),
	_packResistancemOhm(-1),
	_lowOpenCellVoltagemV(-1),
	_hightOpenCellVoltagemV(-1),
	_averageOpencellVoltagemV(-1),
	_nominalPackCapacityAh(-1)
{
}

int Battery::getPackChargeCurrentLimit()
{
	return _packChargeCurrentLimit;
}

int Battery::getPackDischargeCurrent()
{
	return _packDischargeCurrent;
}

float Battery::getSupplyVoltage12()
{
	return _supplyVoltage12On100mV * 0.1;
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

