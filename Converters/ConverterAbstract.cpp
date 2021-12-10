#include <unistd.h>
#include "ConverterAbstract.h"

ConverterAbstract::ConverterAbstract(int fdSerial) : _fdSerial(fdSerial)
{
}

void ConverterAbstract::sendCommand(uint32_t id, uint8_t *data)
{
	revereseArray((uint8_t*)&id, 0, 3);
	write(_fdSerial, &id, 4);
	write(_fdSerial, data, 8);
}

/* Function to reverse arr[] from start to end*/
void ConverterAbstract::revereseArray(uint8_t arr[], int start, int end)
{
    if (start >= end)
    return;
     
    uint8_t temp = arr[start];
    arr[start] = arr[end];
    arr[end] = temp;
     
    // Recursive Function calling
    revereseArray(arr, start + 1, end - 1);
} 
