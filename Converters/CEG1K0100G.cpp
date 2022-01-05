#include <stdio.h>
#include <unistd.h>
#include "CEG1K0100G.h"


CEG1K0100G::CEG1K0100G(int fdSerial, int address) :
	ConverterBase(fdSerial, address)
{
}

void CEG1K0100G::showType()
{
	printf("CEG1K0100G");
}

void CEG1K0100G::parse(uint32_t canId, uint8_t data[])
{
}

void CEG1K0100G::setVoltage(uint32_t voltage)
{
	printf("set voltage %d\n", voltage);
}

void CEG1K0100G::setCurrent(uint32_t current)
{
}

void CEG1K0100G::on()
{
	printf("set on CEG1K0100G\n");
}

void CEG1K0100G::off()
{
	printf("set off CEG1K0100G\n");
}
