#include <stdio.h>
#include <unistd.h>
#include "BEG75050.h"


BEG75050::BEG75050(CommCanAbst *comPort, int address) :
	ConverterBase(comPort, address)
{
}

void BEG75050::showType()
{
	printf("BEG75050");
}

void BEG75050::parse(uint32_t canId, uint8_t data[])
{
	_revereseArray((uint8_t*)&canId, 0, 3);
}

void BEG75050::setVoltage(uint32_t voltage)
{
	printf("set voltage %d\n", voltage);
}

void BEG75050::setCurrent(uint32_t current)
{
	
}

void BEG75050::on()
{
	printf("set on BEG75050\n");
}

void BEG75050::off()
{
	printf("set off BEG75050\n");
}
