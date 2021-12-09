#include <stdio.h>
#include <unistd.h>
#include "CEG1K0100G.h"


CEG1K0100G::CEG1K0100G(int fdSerial) :
	ConverterAbstract(fdSerial)
{
}

void CEG1K0100G::setVoltage(int voltage)
{
	printf("set voltage %d\n", voltage);
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
