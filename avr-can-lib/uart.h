#ifndef UART_H_INCLUDED
#define UART_H_INCLUDED

#include <avr/io.h>


#define	BUF_SIZE 80 

/******************************************************
    inicializacia
    - nastavenie baudRate - ako vstupna premenna
    - zapne vysielac a prijimac
    - 8 datovych a 1 stop bity
******************************************************/

extern void uart_init(unsigned int baud);

/******************************************************
    odosle bajt (znak)
******************************************************/

extern void uart_putc(unsigned char data);

/******************************************************
    odosle retazec bajtov (znakov)
******************************************************/

extern void uart_puts(char *s);
extern void uart_write(char *buff, int size);

/******************************************************
    prijem bajt-u (znak)
******************************************************/

extern unsigned char uart_getc(void);

/******************************************************
    prijem retazca znakov
******************************************************/


unsigned char uart_gets(char *pole);
int uart_read(char* rx_pole);

/******************************************************
    vycistenie buffer-a
******************************************************/

extern void uart_flush(void);

#endif // UART_H_INCLUDED
