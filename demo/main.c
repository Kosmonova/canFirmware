// coding: utf-8

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "can.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>

// #define SUPPORT_EXTENDED_CANID


// -----------------------------------------------------------------------------
/** Set filters and masks.
 *
 * The filters are divided in two groups:
 *
 * Group 0: Filter 0 and 1 with corresponding mask 0.
 * Group 1: Filter 2, 3, 4 and 5 with corresponding mask 1.
 *
 * If a group mask is set to 0, the group will receive all messages.
 *
 * If you want to receive ONLY 11 bit identifiers, set your filters
 * and masks as follows:
 *
 *	uint8_t can_filter[] PROGMEM = {
 *		// Group 0
 *		MCP2515_FILTER(0),				// Filter 0
 *		MCP2515_FILTER(0),				// Filter 1
 *		
 *		// Group 1
 *		MCP2515_FILTER(0),				// Filter 2
 *		MCP2515_FILTER(0),				// Filter 3
 *		MCP2515_FILTER(0),				// Filter 4
 *		MCP2515_FILTER(0),				// Filter 5
 *		
 *		MCP2515_FILTER(0),				// Mask 0 (for group 0)
 *		MCP2515_FILTER(0),				// Mask 1 (for group 1)
 *	};
 *
 *
 * If you want to receive ONLY 29 bit identifiers, set your filters
 * and masks as follows:
 *
 * \code
 *	uint8_t can_filter[] PROGMEM = {
 *		// Group 0
 *		MCP2515_FILTER_EXTENDED(0),		// Filter 0
 *		MCP2515_FILTER_EXTENDED(0),		// Filter 1
 *		
 *		// Group 1
 *		MCP2515_FILTER_EXTENDED(0),		// Filter 2
 *		MCP2515_FILTER_EXTENDED(0),		// Filter 3
 *		MCP2515_FILTER_EXTENDED(0),		// Filter 4
 *		MCP2515_FILTER_EXTENDED(0),		// Filter 5
 *		
 *		MCP2515_FILTER_EXTENDED(0),		// Mask 0 (for group 0)
 *		MCP2515_FILTER_EXTENDED(0),		// Mask 1 (for group 1)
 *	};
 * \endcode
 *
 * If you want to receive both 11 and 29 bit identifiers, set your filters
 * and masks as follows:
 */
const uint8_t can_filter[] PROGMEM = 
{
	// Group 0
	MCP2515_FILTER(0),				// Filter 0
	MCP2515_FILTER(0),				// Filter 1
	
	// Group 1
	MCP2515_FILTER_EXTENDED(0),		// Filter 2
	MCP2515_FILTER_EXTENDED(0),		// Filter 3
	MCP2515_FILTER_EXTENDED(0),		// Filter 4
	MCP2515_FILTER_EXTENDED(0),		// Filter 5
	
	MCP2515_FILTER(0),				// Mask 0 (for group 0)
	MCP2515_FILTER_EXTENDED(0),		// Mask 1 (for group 1)
};
// You can receive 11 bit identifiers with either group 0 or 1.


/* Function to reverse arr[] from start to end*/
void revereseArray(uint8_t arr[], int start, int end)
{
    if (start >= end)
    return;
     
    uint8_t temp = arr[start];
    arr[start] = arr[end];
    arr[end] = temp;
     
    // Recursive Function calling
    revereseArray(arr, start + 1, end - 1);
}  



void cangFncSendMsg(int cmdNo)
{
	// Create a test messsage
	can_t msg;

	msg.length = 8;

	switch(cmdNo)
	{
// vycita vystupne napatie
		case 0x01:

			msg.id = 0x02813ff0;
			memset(msg.data, 0, 8);
			break;

// vycita vystupne napatie
		case 0x03:
			msg.id = 0x028300f0;
			memset(msg.data, 0, 8);
			break;

// vycita teplotu
		case 0x04:
			msg.id = 0x028400f0;
			memset(msg.data, 0, 8);
			break;

// vycita fazove vstupne napatia
		case 0x06:
			msg.id = 0x028600f0;
			memset(msg.data, 0, 8);
			break;

// nastavi vystupne napatie a prud
		case 0x1b:
#define U_MV 300000
#define I_MA 10000
			msg.id = 0x029b3ff0;
			*((uint32_t*)msg.data) = U_MV;
			revereseArray(msg.data, 0, 3);
			*((uint32_t*)(msg.data + 4)) = I_MA;
			revereseArray(msg.data, 4, 7);
			break;

// nastavi vystupne napatie a prud
		case 0x1c:
#define U_MV 300000
#define I_MA 10000
			msg.id = 0x029c00f0;
			*((uint32_t*)msg.data) = U_MV;
			revereseArray(msg.data, 0, 3);
			*((uint32_t*)(msg.data + 4)) = I_MA;
			revereseArray(msg.data, 4, 7);
			break;
	}

	char str[100];
	int idx;

	sprintf(str, "\ncan out id: %lx ", msg.id);
	uart_puts(str);

	uart_puts("data: ");
	for(idx = 0; idx < 8; idx++)
	{
		sprintf(str, "%x ", msg.data[idx]);
		uart_puts(str);
	}
	
	uart_puts("\n");

	msg.flags.rtr = 0;
	msg.flags.extended = 1;
	

	// Send the message
	can_send_message(&msg);
}

// -----------------------------------------------------------------------------
// Main loop for receiving and sending messages.

int main(void)
{
	// Initialize MCP2515
	can_init(BITRATE_125_KBPS);
	uart_init(9600);

	// Load filters and masks
	can_static_filter(can_filter);
	char str[100];
	int idx;

	while (1)
	{
		char comNo = uart_getc1();
		if (comNo)
		{
			sprintf(str, "uart receive %x\n", comNo);
			uart_puts(str);
			cangFncSendMsg(comNo);
		}


		// Check if a new messag was received
		if (can_check_message())
		{
			can_t msg;
			msg.id = 0;
			// Try to read the message
			if (can_get_message(&msg))
			{
				switch(msg.id & 0xffff0000)
				{
					case 0x02810000:
					{
						uart_puts("output: ");
						revereseArray(msg.data, 0, 3);
						revereseArray(msg.data + 4, 0, 3);

						float voltageIn = *((float*)msg.data);
						float currentIn = *(float*)(msg.data + 4);
						char strVoltage[50];
						char strCurrent[50];
						dtostrf(voltageIn, 10, 5, strVoltage);
						dtostrf(currentIn, 10, 5, strCurrent);
						sprintf(str, "voltage %s, current %s\n", strVoltage, strCurrent);
						uart_puts(str);
						break;
					}
					case 0x02840000:
					{
						sprintf(str, "group #%d temperature %d C\n", msg.data[2], msg.data[4]);
						uart_puts(str);
						break;
					}
					case 0x02860000:
					{
						uart_puts("input voltatages: ");
						revereseArray(msg.data, 0, 1);
						revereseArray(msg.data, 2, 3);
						revereseArray(msg.data, 4, 5);
						uint16_t uAB = *((uint16_t*)msg.data);
						uint16_t uBC = *((uint16_t*)(msg.data + 2));
						uint16_t uCA = *((uint16_t*)(msg.data + 4));
						sprintf(str, "AB = %u.%.1u V, BC = %u.%.1u V, CA = %u.%.1u V\n", 
								uAB / 10, uAB % 10,
								uBC / 10, uBC % 10,
								uCA / 10, uCA % 10);
						uart_puts(str);
						break;
					}
					default:
					{
						uart_puts("can in: ");
						sprintf(str, "id: %lx ", msg.id);
						uart_puts(str);
						sprintf(str, "data len: %d ", msg.length);
						uart_puts(str);

						uart_puts("data: ");
					

						for(idx = 0; idx < msg.length; idx++)
						{
							sprintf(str, "%x ", msg.data[idx]);
							uart_puts(str);
						}

						uart_puts("\n");
						break;
					}
				}
			}
		}
	}
	
	return 0;
}
