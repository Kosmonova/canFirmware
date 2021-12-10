#ifndef CONVERTER_ABSTRACT_H
#define CONVERTER_ABSTRACT_H

#include <stdint.h>

class ConverterAbstract
{
	protected:
		void _revereseArray(uint8_t arr[], int start, int end);
		void _sendCommand(uint32_t id, uint8_t *data);
		int _fdSerial;

	public:
		ConverterAbstract(int fdSerial);
		virtual void showType() = 0;
		virtual void parse(int canId, uint8_t data[]) = 0;
		virtual void setVoltage(uint32_t voltage) = 0;
		virtual void setCurrent(uint32_t current) = 0;
		virtual void sendRqRdTemperature() = 0;
		virtual void sendRqRdInputVoltage() = 0;
		virtual void on() = 0;
		virtual void off() = 0;
};

#endif
