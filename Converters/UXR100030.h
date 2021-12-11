#ifndef UXR100030_H
#define UXR100030_H


#define GET_MODULE_VOLTAGE 0x0001
#define GET_MODULE_CURRENT 0x0002
#define GET_MODULE_CURRENT_LIMIT_POINT 0x0003
#define GET_MODULE_DC_BOARD_TEMPERATURE 0x004
#define GET_MODULE_INPUT_PHASE_VOLTAGE 0x0005
#define GET_MODULE_FPCO_VOLTAGE_POSITIVE_HALF 0x0008
#define GET_MODULE_PFCO_VOLTAGE_NEGATIVE_HALF 0x000A
#define GET_MODULE_PANEL_TEMPEARTURE 0x000B
#define GET_MODULE_AC_PHASE_A_VOLTAGE 0x000C
#define GET_MODULE_AC_PHASE_B_VOLTAGE 0x000D
#define GET_MODULE_AC_PHASE_C_VOLTAGE 0x000E
#define GET_MODULE_PFC_BOARD_TEMPERATURE 0x0010
#define GET_MODLE_RATED_OUTPUT_POWER 0x0011
#define GET_MODULE_RATED_OUTPUT_CURRENT 0x0012
#define SET_MODULE_OUTPUT_CURRENT 0x001B
#define SET_MODULE_OUTPUT_VOLTAGE 0x0021
#define SET_MODULE_CURRENT_LIMIT_POINT 0x0022
#define POWER_ON_OFF 0x0030


#include "ConverterBase.h"

class UXR100030 : public ConverterBase
{
	private:
		uint32_t _generateId();
		void _generateSetMdlData(uint8_t *data, uint16_t reg, uint32_t value);
		void _generateReadMdlData(uint8_t *data, uint16_t reg, uint32_t value);

	public:
		UXR100030(int fdSerial, int address);
		void showType();
		void parse(int canId, uint8_t data[]);
		void setVoltage(uint32_t voltage);
		void getVoltage();
		void setCurrent(uint32_t current);
		void setCurrentLimitPoint(float point);
		void sendRqRdTemperature(){}
		void sendRqRdInputVoltage(){}
		void on();
		void off();
};

#endif
