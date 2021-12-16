#ifndef CAN_ADAPTER_H
#define CAN_ADAPTER_H

class CanAdapter
{
	private:
		int _SerialPort;
		bool _extendId;

	public:
		CanAdapter(int serialPort, bool extendId) : 
			_SerialPort(serialPort),
			_extendId(extendId){}
		int open(uint8_t baudRate);
		int read();
		int write();
};

#endif
