#ifndef REG1K0100G_H
#define REG1K0100G_H

#include "ConverterBase.h"

class REG1K0100G : public ConverterBase
{
	private:
		uint32_t _systemVoltage_mV;
		uint32_t _systemCurrent_mA;
		void _setSystemOutputValues();
		uint32_t _generateId(uint8_t command);

	public:
		REG1K0100G(int fdSerial, int address);
		void showType();
		void parse(uint32_t canId, uint8_t data[]);
		void setVoltage(uint32_t voltage);
		void setCurrent(uint32_t current);
		void readTemperature();
		void readInputVoltage();
		void sendRqRdOutputSystemValues();
		void on();
		void off();
};

#endif
