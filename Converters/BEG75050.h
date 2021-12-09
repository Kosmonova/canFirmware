#include "ConverterAbstract.h"

class BEG75050 : public ConverterAbstract
{
	public:
		BEG75050(int fdSerial);
		void setVoltage(int voltage);
		void on();
		void off();
};

