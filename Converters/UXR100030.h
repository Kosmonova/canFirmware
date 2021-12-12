#ifndef UXR100030_H
#define UXR100030_H

#include "ConverterBase.h"


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
#define GET_CURRENT_ALARM_STATUS 0x0040
#define GET_CURRENT_MODULE_INPUT_WORKING_MODE 0x004B

#define SINGLE_PHASE_AC_MODE 0x00000001
#define DC_MODE 0x00000002
#define THRE_PHASE_AC_MODE 0x00000003
#define PATTREN_MICHMATCH 0x00000005

#define MODULE_FAULT 0
#define MODULE_PROTECTION 1
#define MODULE_INTERNAL_SCI_COMMUNICAION_FAILURE 3
#define INPUT_MODE_ERROR 4
#define INPUT_MODE_SET_BY_MONITOR_DOES_NOT_MATCH 5
#define DCDC_OVERVOLTAGE 7
#define PFC_VOLTAGE_ABNORMAL 8
#define AC_OVERVOLTAGE 9
#define AC_UNDERVOLTAGE 14
#define CAN_COMMUNICATION_FAILURE 16
#define MODULE_CURRENT_IMBALANCE 17
#define DCDC_ON_OFF_STATUS 22
#define MODULE_POWER_LIMITING 23
#define TEMPERATURE_DERATING 24
#define AC_POWER_LIMITING 25
#define FAN_FAULT 27
#define DCDC_SHORT_CIRCIUT 28
#define DCDC_OVER_TEMPERATURE 30
#define DCDC_OUTPUT_OVERVOLTAGE 31


class UXR100030 : public ConverterBase
{
	private:
		uint32_t _generateId();
		void _generateSetMdlData(uint8_t *data, uint16_t reg, uint32_t value);
		void _generateReadMdlData(uint8_t *data, uint16_t reg, uint32_t value);
		uint8_t _getAddressFromId(uint32_t id);
		bool _parseData(uint8_t *data, uint16_t *reg, uint32_t *value,
			bool *isFloat);
		char *_getFormat(char *inBuff, uint32_t value, bool isFloat);
		void _parseStatus(uint32_t value);

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
