#include <stdio.h>
#include <unistd.h>
#include "BEG75050.h"


BEG75050::BEG75050(int fdSerial) :
	ConverterAbstract(fdSerial)
{
}

void BEG75050::setVoltage(int voltage)
{
	printf("set voltage %d\n", voltage);
}

void BEG75050::on()
{
	printf("set on BEG75050\n");
	write(_fdSerial, "\x1a\x01", 2);
}

void BEG75050::off()
{
	printf("set off BEG75050\n");
	write(_fdSerial, "\x1a\x01", 2);
}
