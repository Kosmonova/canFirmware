#ifndef CONVERTER_BASE_H
#define CONVERTER_BASE_H

#include <stdint.h>


class CommCanAbst;

enum Mode{AC_MODE, DC_MODE};

class ConverterBase
{
	protected:
		void _revereseArray(uint8_t arr[], int start, int end);
		void _sendCommand(uint32_t id, uint8_t *data);
		void _cmdNotImplemented(char *nameFunction);
		CommCanAbst *_comPort;
		int _address;
		bool _broadcast;

	public:
		ConverterBase(CommCanAbst *comPort, int address);
		void changeAddress(uint8_t address);
		void readData(uint32_t *id, uint8_t *data, int *size);
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
