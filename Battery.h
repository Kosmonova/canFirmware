#ifndef BATTERY_H
#define BATTERY_H


class Battery
{
	private:
		int _packChargeCurrentLimit;
		int _packDischargeCurrent;
		int _supplyVoltage12On100mV;
		int _stateChargePercent;
		int _packAmpHours;
		int _packVoltage;
		int _hightTemperatureCelsius;
		int _lowTemperatureCelsius;
		int _averageTemperatureCelsius;
		int _bmsTemperatureCelsius;
		int _packCurrent100mA;
		int _packOpenVoltage100mV;
		int _packSummedVoltage100mV;
		int _totalPackCycles;
		int _packHealthPercent;
		int _packResistancemOhm;
		int _lowOpenCellVoltagemV;
		int _hightOpenCellVoltagemV;
		int _averageOpencellVoltagemV;
		int _nominalPackCapacityAh;

	public:
		Battery();
		void parse(uint32_t canId, uint8_t *data, int dataSize);
		int getPackChargeCurrentLimit();
		int getPackDischargeCurrent();
		float getSupplyVoltage12();
};

#endif
