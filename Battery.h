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

	public:
		Battery();
		void parse(uint32_t canId, uint8_t *data, int dataSize);
};

#endif
