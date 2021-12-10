#ifndef REG1K0100G_H
#define REG1K0100G_H

#include "ConverterAbstract.h"

class REG1K0100G : public ConverterAbstract
{
	private:
		uint32_t _systemVoltage_mV;
		uint32_t _systemCurrent_mA;
		void _setSystemOutputValues();

	public:
		REG1K0100G(int fdSerial);
		void showType();
		void parse(int canId, uint8_t data[]);
		void setVoltage(uint32_t voltage);
		void setCurrent(uint32_t current);
		void sendRqRdTemperature();
		void sendRqRdInputVoltage();
		void on();
		void off();
};

#endif
