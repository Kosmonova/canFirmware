// coding: utf-8

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "can.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

// Function Implementation
void wdt_init(void)
{
    MCUSR = 0;
    wdt_disable();

    return;
}

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
 

// -----------------------------------------------------------------------------
// Main loop for receiving and sending messages.

volatile unsigned long timeMilis;

unsigned long milis()
{
	return timeMilis;
}

ISR (TIMER1_OVF_vect)    // Timer1 ISR
{
	timeMilis++;	
	TCNT1 = 63974;   // for 1 sec at 16 MHz
}

void setTimer()
{
	TCNT1 = 63974;   // for 1 sec at 16 MHz	

	TCCR1A = 0x00;
	TCCR1B = (1<<CS10) | (1<<CS12);;  // Timer mode with 1024 prescler
	TIMSK = (1 << TOIE1) ;   // Enable timer1 overflow interrupt(TOIE1)
}

int main(void)
{
	while(1)
	{
			// Initialize MCP2515
		can_init(BITRATE_125_KBPS);
		uart_init(57600);

						
		// Load filters and masks
		can_static_filter(can_filter);
		setTimer();
		sei();


		char uartMsg[BUF_SIZE];
		memset(uartMsg, 0xAB, BUF_SIZE);
		int sizeUart = 0;
		long lastTime = milis();
		
	// 	can_t msg1;
	// 	uint8_t dataSetOn[8] = {0x3, 0, 0, 0x30, 0, 0, 0, 0,};
	// 	msg1.id = 0x60 * 0x100000  + 0x80000 + 0x800 * 0xFF + 8 * 0xF0;
	// 	memcpy( msg1.data, dataSetOn, 8);
	// 	while(1)
	// 	{
	// 		
	// 		can_send_message(&msg1);
	// 		_delay_ms(500);
	// 	}
		
		
		while (1)
		{
			can_t msg;
			// Check if a new messag was received
			if (can_check_message())
			{
				// Try to read the message
				if (can_get_message(&msg))
				{
					uart_write(&msg.id, 4);
					uart_write(msg.data, 8);
				}
			}

			if(lastTime + 100 < milis() || (sizeUart > BUF_SIZE /2))
			{
// 				sizeUart = 0;
// 				lastTime = milis();
				cli();
				break;
			}
		
			sizeUart += uart_read(uartMsg + sizeUart);

			if(sizeUart < 12)
				continue;

// 			lastTime = milis();

			sizeUart = 0;
			memcpy(&msg.id, uartMsg, 4);
			memcpy(msg.data, uartMsg + 4, 8);
			can_send_message(&msg);
	// 		uart_write(&msg.id, 4);
	// 		uart_write(msg.data, 8);
		}
	}


	return 0;
}
