#ifndef CAN_ADAPTER_H
#define CAN_ADAPTER_H

#include <stdint.h>

#include "ComPort.h"
#include "CommCanAbst.h"

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
#define CR 015

#define COUNT_ASCII_STANDART_PACKET_ID 6
#define COUNT_ASCII_EXTEND_PACKET_ID 8
#define COUNT_ASCII_BYTE 2
#define COUNT_ASCII_DEC_DIGIT 1
#define SIZE_BUFFER 100

#define STR1(x)  #x
#define STR(x)  STR1(x)
 
class CanAdapter : public CommCanAbst
{
	private:
		ComPort *_comPort;
		bool _extendId;
        uint8_t _buffer[SIZE_BUFFER];
        int _bufferPosFill;

	public:
		CanAdapter(ComPort *comPort, bool extendId);
		bool setBaudRate(uint8_t baudRate);
		bool openCan();
		bool closeCan();
		int readCan(uint32_t *canId, uint8_t *canData, int *dataSize,
			bool skipReadOnFull, bool *isExtedId = nullptr);
		int readCan(uint32_t *id, uint8_t *data, int *size);
		int writeCan(uint32_t canId, uint8_t *data, int dataSize);
		void flush();
};

#endif
