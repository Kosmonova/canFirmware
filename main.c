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


void cangFncSendMsg(int cmdNo, int ar1, int ar2)
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

// on/off
		case 0x1a:
			msg.id = 0x029a3ff0;
			memset(msg.data, 0, 8);
			
			if(ar1)
				msg.data[0] = 0x01;
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

// vycita externe napatie a vystupny prud
		case 0x0c:
			msg.id = 0x028c00f0;
			memset(msg.data, 0, 8);
			break;

// citanie informacii modulu
		case 0x0A:
			msg.id = 0x028a00f0;
			memset(msg.data, 0, 8);
			break;

// citanie barcodu
		case 0x0B:
			msg.id = 0x028b00f0;
			memset(msg.data, 0, 8);
			break;

// nastavuje led
		case 0x14:
			msg.id = 0x029400f0;
			memset(msg.data, 0, 8);

			if(ar1)
				msg.data[0] = 0x01;
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

	can_t msg;
	msg.id = 0x12345678;

	char datacp[8] =  {0x0F, 0xB4, 0x0F, 0xA5, 0x0F, 0xA7, 0x00, 0x00};
	for(idx = 0; idx < 8; idx++)
		msg.data[idx] = datacp[idx];

char testData[4][12] ={
	{0x02, 0x86, 0xF0, 0x01, 0x0F, 0xB4, 0x0F, 0xA5, 0x0F, 0xA7, 0x00, 0x00},
	{0x02, 0x8A, 0xF0, 0x00, 0x02, 0xEE, 0x00, 0x64, 0x01, 0x00, 0x05, 0xDC},
	{0x02, 0x8B, 0xF0, 0x00, 0x56, 0x13, 0x0C, 0x15, 0xA3, 0xFB, 0x06, 0xA8},
	{0x02, 0x81, 0xF0, 0x3F, 0x43, 0xFA, 0x00, 0x00, 0x42, 0x48, 0x00, 0x00}
};
	
// 	msg.id = ;
// 	memcpy(msg.data, (char*), 8);

	while(1)
	{
		idx %= 4;
// 		uart_write(&msg.id, 4);
// 		uart_write(msg.data, 8);
		uart_write(testData[idx++], 12);
		_delay_ms(3000);
	}

// 	while (1)
// 	{
// 		char comNo = uart_getc1();
// 		if (comNo)
// 		{
// 			sprintf(str, "uart receive %x\n", comNo);
// 			uart_puts(str);
// 			int arg1 = uart_getc1();
// 			int arg2 = uart_getc1();
// 
// 			cangFncSendMsg(comNo, arg1, arg2);
// 		}
// 
// 		// Check if a new messag was received
// 		if (can_check_message())
// 		{
// 			can_t msg;
// 			msg.id = 0;
// 			// Try to read the message
// 			if (can_get_message(&msg))
// 			{
// 				uart_write(&msg.id, 4);
// 				uart_write(&msg.data, 8);
// 			}
// 		}
// 	}
	
	return 0;
}
