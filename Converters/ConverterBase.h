#ifndef CONVERTER_BASE_H
#define CONVERTER_BASE_H

#include <stdint.h>


class ComPort;
class CanAdapter;

enum Mode{AC_MODE, DC_MODE};

class ConverterBase
{
	protected:
		void _revereseArray(uint8_t arr[], int start, int end);
		void _sendCommand(uint32_t id, uint8_t *data);
		void _readData(uint32_t id, uint8_t *data);
		void _cmdNotImplemented(char *nameFunction);
		ComPort *_comPort;
		CanAdapter *_canAdapter;
		int _address;
		bool _broadcast;

	public:
		ConverterBase(int fdSerial, int address);
		ConverterBase(ComPort *comPort, int address);
		ConverterBase(CanAdapter *canAdapter, int address);
		void changeAddress(uint8_t address);
		uint8_t getAddress();
		virtual void showType() = 0;
		virtual void parse(uint32_t canId, uint8_t data[]);

		virtual void setVoltage(uint32_t voltage);
		virtual void readInputVoltage();
		virtual void readOuputVotage();

		virtual void setCurrent(uint32_t current);
		virtual void setCurrentLimitPoint(float point);
		virtual void readOutputCurrent();

		virtual void readTemperature();
		virtual void readTempCyclic(uint8_t address = 0xFF);
		virtual void readPower();
		virtual void readStatus();
		virtual void setModuleInputMode(Mode mode);
		virtual void on();
		virtual void off();
};

#endif
