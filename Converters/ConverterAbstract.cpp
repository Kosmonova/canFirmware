#include <unistd.h>
#include <stdio.h>

#include "ConverterAbstract.h"

ConverterAbstract::ConverterAbstract(int fdSerial, int address) :
	_fdSerial(fdSerial),
	_address(address)
{
}

void ConverterAbstract::_cmdNotImplemented(char *nameFunction)
{
	printf("command %s not implemented for convertor type ", nameFunction);
	showType();
	printf("\n");
}

void ConverterAbstract::sendRqRdOutputSystemValues()
{
	_cmdNotImplemented("read_out_values");
}

void ConverterAbstract::_sendCommand(uint32_t id, uint8_t *data)
{
	_revereseArray((uint8_t*)&id, 0, 3);
	write(_fdSerial, &id, 4);
	write(_fdSerial, data, 8);
}

/* Function to reverse arr[] from start to end*/
void ConverterAbstract::_revereseArray(uint8_t arr[], int start, int end)
{
    if (start >= end)
    return;
     
    uint8_t temp = arr[start];
    arr[start] = arr[end];
    arr[end] = temp;
     
    // Recursive Function calling
    _revereseArray(arr, start + 1, end - 1);
} 
