#ifndef BEG75050_H
#define BEG75050_H

#include "ConverterAbstract.h"

class BEG75050 : public ConverterAbstract
{
	public:
		BEG75050(int fdSerial);
		void showType();
		void parse(int canId, uint8_t data[]);
		void setVoltage(int voltage);
		void on();
		void off();
};

#endif
