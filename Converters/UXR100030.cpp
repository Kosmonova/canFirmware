#include <stdio.h>
#include <unistd.h>
#include "UXR100030.h"


UXR100030::UXR100030(int fdSerial) :
	ConverterAbstract(fdSerial)
{
}

void UXR100030::setVoltage(int voltage)
{
	printf("set voltage %d\n", voltage);
}

void UXR100030::on()
{
	printf("set on UXR100030\n");
	write(_fdSerial, "\x1a\x01", 2);
}

void UXR100030::off()
{
	printf("set off UXR100030\n");
	write(_fdSerial, "\x1a\x01", 2);
}
