#ifndef CAN_ADAPTER_H
#define CAN_ADAPTER_H

#include <stdint.h>

#include "ComPort.h"

#define CAN_10Kb  0
#define CAN_20Kb  1
#define CAN_50Kb  2
#define CAN_100Kb 3
#define CAN_125Kb 4
#define CAN_250Kb 5
#define CAN_500Kb 6
#define CAN_800Kb 7
#define CAN_1Mb   8

#define ACK 0x06

#define SIZE_BUFFER 100

class CanAdapter
{
	private:
		ComPort *_comPort;
		bool _extendId;
        uint8_t _buffer[SIZE_BUFFER];
        int _buffPos;

	public:
		CanAdapter(ComPort *comPort, bool extendId);
		bool setBaudRate(uint8_t baudRate);
		bool openCan();
		bool closeCan();
		int readCan(uint32_t *canId, uint8_t *canData, int *dataSize,
			bool *extendCanId = nullptr);
		int writeCan(uint32_t canId, int dataSize, uint8_t *data);
};

#endif
