#ifndef CEG1K0100G_H
#define CEG1K0100G_H

#include "ConverterBase.h"

class CEG1K0100G : public ConverterBase
{
	public:
		CEG1K0100G(int fdSerial, int address);
		void showType();
		void parse(uint32_t canId, uint8_t data[]);
		void setVoltage(uint32_t voltage);
		void setCurrent(uint32_t current);
		void sendRqRdTemperature(){}
		void sendRqRdInputVoltage(){}
		void on();
		void off();
};

#endif
