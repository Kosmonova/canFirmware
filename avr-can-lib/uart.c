#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "uart.h"


#define RX_UKONCENE 1
#define RX_NEUKONCENE 2


#define	BUF_SIZE 128 					// Velkost rx pola
char rx_buff[BUF_SIZE];	//inicializacia rx pola
volatile unsigned char rx_pos,rx_stav = RX_NEUKONCENE;				// ukazovatel pola, stav rx dat


/******************************************************
    inicializacia
    - nastavenie baudRate - ako vstupna premenna
    - zapne vysielac a prijimac
    - 8 datovych a 1 stop bity
******************************************************/

void uart_init(unsigned int baud)
{
    unsigned int ubrr = F_CPU/16/baud-1;

	//nastavenie prenosovej rychlosti v UBRR
	UBRRH = (unsigned char) (ubrr>>8);
	UBRRL = (unsigned char) ubrr;

	//zapni prijmac a vysielac, prerusenie pri prijime noveho znaku
	UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE);

	//nastav format ramca: 8data, 1stop bity
	UCSRC = (1<<URSEL)|(3<<UCSZ0);
}

/******************************************************
    odosle bajt (znak)
******************************************************/

void uart_putc(unsigned char data)
{
	//cakaj na vyprazdnenie buffera
	while (!( UCSRA & (1<<UDRE)));

	//vloz data do buffer-a a odosli
	UDR = data;
}

/******************************************************
    odosle retazec bajtov (znakov)
******************************************************/

void uart_puts(char *s)
{
    while (*s)
      uart_putc(*s++);
}

void uart_write(char *buff, int size)
{
    while (size-- > 0)
      uart_putc(*buff++);
}

/******************************************************
    prijem bajt-u (znak)
******************************************************/

unsigned char uart_getc(void)
{
	//cakaj na prijatie dat
	while (!(UCSRA & (1<<RXC)));

	//zober data z buffer-a
	return UDR;
}

unsigned char uart_getc1()
{
	//cakaj na prijatie dat
	if (!(UCSRA & (1<<RXC)))
		return 0;

	//zober data z buffer-a
	return UDR;
}

/******************************************************
    prijem retazca znakov
******************************************************/

unsigned char uart_gets(char* rx_pole){
	
	// ak je prijimanie retazca znakov ukoncene...
	if(rx_stav == RX_UKONCENE) {
		// skopiruj buffer do pola
		strcpy(rx_pole, rx_buff);
		//zacni prijimanie retazca znova
		rx_stav = RX_NEUKONCENE;
		rx_pos = 0;
		// navratova hodnota, boli prijate data	
		return 1;	

	}else{
		// data neboli prijate...
		rx_pole = 0;
		return 0;	
	}

}

int uart_read(char* rx_pole){
	UCSRB &= ~(1<<RXCIE);
	if(rx_pos != 0)
		memcpy(rx_pole, rx_buff, rx_pos);
	int result = rx_pos;
	rx_pos = 0;
	UCSRB |= (1<<RXCIE);
	return result;
}

/******************************************************
prerusenie rx complete
******************************************************/
ISR(USART_RXC_vect)
{
	rx_pos = 0;
	rx_stav = 0;

	while(UCSRA & (1<<RXC))
	{
		rx_buff[rx_pos] = UDR;	// ulozenie znaku do buffer-a
		rx_pos++;	 // inkrementuj pocitadlo
	}
	rx_stav = RX_UKONCENE;
}


/******************************************************
    vycistenie buffer-a
******************************************************/

void uart_flush(void)
{
	unsigned char dummy;

	while ( UCSRA & (1<<RXC) ) dummy = UDR;
}
