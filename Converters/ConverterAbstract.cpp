#include "ConverterAbstract.h"

ConverterAbstract::ConverterAbstract(int fdSerial) : _fdSerial(fdSerial)
{
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
