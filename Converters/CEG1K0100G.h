#ifndef CEG1K0100G_H
#define CEG1K0100G_H

#include "ConverterAbstract.h"

class CEG1K0100G : public ConverterAbstract
{
	public:
		CEG1K0100G(int fdSerial);
		void parse(int canId, uint8_t data[]);
		void setVoltage(int voltage);
		void on();
		void off();
};

#endif
