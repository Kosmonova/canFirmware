#ifndef CONVERTER_ABSTRACT_H
#define CONVERTER_ABSTRACT_H

#include <stdint.h>

class ConverterAbstract
{
	protected:
		void revereseArray(uint8_t arr[], int start, int end);
		int _fdSerial;

	public:
		ConverterAbstract(int fdSerial);
		virtual void parse(int canId, uint8_t data[]) = 0;
		virtual void setVoltage(int voltage) = 0;
		virtual void on() = 0;
		virtual void off() = 0;
};

#endif