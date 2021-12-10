#ifndef CEG1K0100G_H
#define CEG1K0100G_H

#include "ConverterAbstract.h"

class CEG1K0100G : public ConverterAbstract
{
	public:
		CEG1K0100G(int fdSerial);
		void showType();
		void parse(int canId, uint8_t data[]);
		void setVoltage(uint32_t voltage);
		void setCurrent(uint32_t current);
		void sendRqRdTemperature(){}
		void sendRqRdInputVoltage(){}
		void on();
		void off();
};

#endif
