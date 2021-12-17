#ifndef CAN_ADAPTER_H
#define CAN_ADAPTER_H

#include <stdint.h>

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

class CanAdapter
{
	private:
		int _SerialPort;
		bool _extendId;

	public:
		CanAdapter(int serialPort, bool extendId);
		int open(uint8_t baudRate);
		int close();
		int read();
		int write();
};

#endif
