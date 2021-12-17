#ifndef CAN_ADAPTER_H
#define CAN_ADAPTER_H

#include <stdint.h>

class ComPort
{
	private:
		int _SerialPort;
		int _set_interface_attribs (int fd, int speed, int parity);
		void _set_blocking (int fd, int should_block);

	public:
		ComPort();
		int openCom(char *portname, int baudRate);
		int closeCom();
		int readCom(uint8_t * pBuff, uint32_t BytesToRead);
		int writeCom(uint8_t *data, int dataSize);
};

#endif
