#ifndef UXR100030_H
#define UXR100030_H

#include "ConverterAbstract.h"

class UXR100030 : public ConverterAbstract
{
	public:
		UXR100030(int fdSerial);
		void parse(int canId, uint8_t data[]);
		void setVoltage(int voltage);
		void on();
		void off();
};

#endif
