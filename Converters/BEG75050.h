#ifndef BEG75050_H
#define BEG75050_H

#include "ConverterBase.h"

class CommCanAbst;

class BEG75050 : public ConverterBase
{
	public:
		BEG75050(CommCanAbst *comPort, int address);
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
