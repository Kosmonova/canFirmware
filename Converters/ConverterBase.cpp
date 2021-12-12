#include <unistd.h>
#include <stdio.h>

#include "ConverterBase.h"

ConverterBase::ConverterBase(int fdSerial, int address) :
	_fdSerial(fdSerial),
	_address(address),
	_broadcast(false)
{
}

/* Function to reverse arr[] from start to end*/
void ConverterBase::_revereseArray(uint8_t arr[], int start, int end)
{
    if (start >= end)
    return;
     
    uint8_t temp = arr[start];
    arr[start] = arr[end];
    arr[end] = temp;
     
    // Recursive Function calling
    _revereseArray(arr, start + 1, end - 1);
}

void ConverterBase::_sendCommand(uint32_t id, uint8_t *data)
{
	_revereseArray((uint8_t*)&id, 0, 3);
	write(_fdSerial, &id, 4);
	write(_fdSerial, data, 8);
} 

void ConverterBase::_cmdNotImplemented(char *nameFunction)
{
	printf("command %s not implemented for convertor type ", nameFunction);
	showType();
	printf("\n");
}
		
void ConverterBase::setVoltage(uint32_t voltage)
{
	_cmdNotImplemented("set_voltage");
}

void ConverterBase::readOuputVotage()
{
	_cmdNotImplemented("get_out_voltage");
}

void ConverterBase::setCurrent(uint32_t current)
{
	_cmdNotImplemented("set_current");
}

void ConverterBase::setCurrentLimitPoint(float point)
{
	_cmdNotImplemented("set_current_limit_point");
}

void ConverterBase::readPower()
{
	_cmdNotImplemented("read_power");
}

void ConverterBase::readStatus()
{
	_cmdNotImplemented("read_status");
}

void ConverterBase::setModuleInputMode(Mode mode)
{
	_cmdNotImplemented("set_input_mode");
}

void ConverterBase::readTemperature()
{
	_cmdNotImplemented("read_temp");	
}

void ConverterBase::readInputVoltage()
{
	_cmdNotImplemented("read_input_voltages");
}

void ConverterBase::on()
{
	_cmdNotImplemented("set on");
}

void ConverterBase::off()
{
	_cmdNotImplemented("set off");
}
