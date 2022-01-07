#ifndef COMM_CAN_ABST_H
#define COMM_CAN_ABST_H

#include <stdint.h>

class CommCanAbst
{
	public:
		virtual int readCan(uint32_t *id, uint8_t *data, int *size) = 0;
		virtual int writeCan(uint32_t id, uint8_t *data, int size) = 0;
};

#endif
