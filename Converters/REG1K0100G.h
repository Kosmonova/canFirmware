#ifndef REG1K0100G_H
#define REG1K0100G_H

#include "ConverterAbstract.h"

class REG1K0100G : public ConverterAbstract
{
	public:
		REG1K0100G(int fdSerial);
		void showType();
		void parse(int canId, uint8_t data[]);
		void setVoltage(int voltage);
		void on();
		void off();
};

#endif
