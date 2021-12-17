#include <unistd.h>

#include "ComPort.h"


ComPort::ComPort() :
	_SerialPort(-1)
{
}

int ComPort::_set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        if (tcgetattr (fd, &tty) != 0)
        {
                error_message ("error %d from tcgetattr\n", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                error_message ("error %d from tcsetattr\n", errno);
                return -1;
        }
        return 0;
}

void ComPort::_set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                error_message ("error %d from tggetattr\n", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                error_message ("error %d setting term attributes\n", errno);
}

int ComPort::openCom(char *portname, int baudRate)
{
	_SerialPort = open (portname, O_RDWR | O_NOCTTY | O_SYNC);

	if (_SerialPort < 0)
	{
		error_message ("error %d opening %s: %s\n", errno, portname, strerror (errno));
		return -1;
	}

	_set_interface_attribs (_SerialPort, baudRate, 0);  // set speed to 115,200 bps, 8n1 (no parity)
	_set_blocking (_SerialPort, 0); 

	return 0;
}

int ComPort::closeCom()
{
	int ret = close(_SerialPort);
	_SerialPort = -1;
	return ret;
}

int ComPort::readCom(uint8_t * pBuff, uint32_t BytesToRead)
{
	struct pollfd fd = { .fd = _SerialPort, .events = POLLIN };
    size_t      bytesread = 0;

    while (poll (&fd, 1, 25) == 1)
    {
        int chunksize = read (_SerialPort, pBuff + bytesread, BytesToRead);
        if (chunksize == -1)
            break;

        bytesread += chunksize;
        BytesToRead -= chunksize;

        if (BytesToRead == 0)
           break;
    }

	return bytesread;
}

int ComPort::writeCom(uint8_t *data, int dataSize)
{
	return write(_SerialPort, data, dataSize);
}

