#ifndef UXR100030_H
#define UXR100030_H

#include "ConverterBase.h"

class UXR100030 : public ConverterBase
{
	public:
		UXR100030(int fdSerial, int address);
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
