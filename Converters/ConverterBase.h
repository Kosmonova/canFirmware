#ifndef CONVERTER_BASE_H
#define CONVERTER_BASE_H

#include <stdint.h>

class ConverterBase
{
	protected:
		void _revereseArray(uint8_t arr[], int start, int end);
		void _sendCommand(uint32_t id, uint8_t *data);
		void _cmdNotImplemented(char *nameFunction);
		int _fdSerial;
		int _address;
		bool _broadcast;

	public:
		ConverterBase(int fdSerial, int address);
		virtual void showType() = 0;
		virtual void parse(int canId, uint8_t data[]) = 0;
		virtual void setVoltage(uint32_t voltage);
		virtual void getVoltage();
		virtual void setCurrent(uint32_t current);
		virtual void setCurrentLimitPoint(float point);
		virtual void sendRqRdTemperature();
		virtual void sendRqRdInputVoltage();
		virtual void sendRqRdOutputSystemValues();
		virtual void on();
		virtual void off();
};

#endif
