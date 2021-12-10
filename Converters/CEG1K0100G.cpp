#include <stdio.h>
#include <unistd.h>
#include "CEG1K0100G.h"


CEG1K0100G::CEG1K0100G(int fdSerial, int address) :
	ConverterAbstract(fdSerial, address)
{
}

void CEG1K0100G::showType()
{
	printf("CEG1K0100G");
}

void CEG1K0100G::parse(int canId, uint8_t data[])
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
	write(_fdSerial, "\x1a\x01", 2);
}

void CEG1K0100G::off()
{
	printf("set off CEG1K0100G\n");
	write(_fdSerial, "\x1a\x01", 2);
}
