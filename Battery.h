#ifndef BATTERY_H
#define BATTERY_H


class Battery
{
	public:
		Battery();
		void parse(uint32_t canId, uint8_t *data);
};

#endif
