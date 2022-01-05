#include <unistd.h>
#include <stdio.h>

#include "ConverterBase.h"
#include "CanAdapter.h"
#include "ComPort.h"

ConverterBase::ConverterBase(int fdSerial, int address) :
	_fdSerial(fdSerial),
	_comPort(nullptr),
	_canAdapter(nullptr),
	_address(address),
	_broadcast(false)
{
	_broadcast = (_address > 0x65);
}

ConverterBase::ConverterBase(ComPort *comPort, int address) :
	_fdSerial(0),
	_comPort(comPort),
	_canAdapter(nullptr),
	_address(address),
	_broadcast(false)
{
	_broadcast = (_address > 0x65);
}

ConverterBase::ConverterBase(CanAdapter *canAdapter, int address) :
	_fdSerial(0),
	_comPort(nullptr),
	_canAdapter(canAdapter),
	_address(address),
	_broadcast(false)
{
	_broadcast = (_address > 0x65);
}

void ConverterBase::changeAddress(uint8_t address)
{
	_address = address;
	_broadcast = (_address > 0x65);
}

uint8_t ConverterBase::getAddress()
{
	return _address;
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

	if(_fdSerial)
	{
		write(_fdSerial, &id, 4);
		write(_fdSerial, data, 8);
		return;
	}

	if(_comPort)
	{
		_comPort->writeCom((uint8_t*)&id, 4);
		_comPort->writeCom(data, 8);
		return;
	}

	if(_canAdapter)
	{
		_canAdapter->writeCan(id, data, 8);
		return;
	}
} 

void ConverterBase::_cmdNotImplemented(char *nameFunction)
{
	printf("command %s not implemented for convertor type ", nameFunction);
	showType();
	printf("\n");
}

void ConverterBase::parse(uint32_t canId, uint8_t data[])
{
	_cmdNotImplemented("parse");
}

void ConverterBase::setVoltage(uint32_t voltage)
{
	_cmdNotImplemented("set_voltage");
}

void ConverterBase::readOuputVotage()
{
	_cmdNotImplemented("read_output_voltage");
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

void ConverterBase::readTempCyclic(uint8_t address)
{
	_cmdNotImplemented("readTempCyclic");
}

void ConverterBase::readInputVoltage()
{
	_cmdNotImplemented("read_input_voltages");
}

void ConverterBase::readOutputCurrent()
{
	_cmdNotImplemented("read_output_current");
}

void ConverterBase::on()
{
	_cmdNotImplemented("set on");
}

void ConverterBase::off()
{
	_cmdNotImplemented("set off");
}
