/*
 * main.c
 *
 *  Created on: 28.08.2013
 *      Author: Grisu
 */

#include <avr/interrupt.h>
#include <stdlib.h>
#include "config.h"
#include "usart.h"
#include "cmd.h"

#if USE_SD == 1
	#include "sd/mmc_config.h"	// Hier werden alle noetigen Konfigurationen vorgenommen, unbedingt anschauen !
	#include "sd/file.h"
	#include "sd/fat.h"
	#include "sd/mmc.h"
#endif


int main(void){
	DDRA=0xFF;
	DDRB=0xFF;
	DDRC=0xFF;
	DDRD=0xFF;
	PORTA=0x00;
	PORTB=0x00;
	PORTC=0x00;
	PORTD=0x00;
	usart_init(9600);
	sei();
	usart_write_str(CRLL"AVR-Con alpha 0.1"CRLF);

#if USE_SD == 1
	usart_write_str("Boot SD");

	if( FALSE == mmc_init() ){
		usart_write_str(CRLF"ERR | MMC-Init: System halted");
		return 0;
	}

	usart_write_str("...");

	if( FALSE == fat_loadFatData() ){
		usart_write_str(CRLF"ERR | FAT-Init: System halted");
		return 0;
	}
#endif

	usart_write_str("OK"CRLL);

	while(1){

		usart_write("> ");
		usart_status.usart_ready=0;

		while(!usart_status.usart_ready);
		usart_write_str(CRLF);
		parseLine(rx_buffer_pointer);
		hist_add(my_strcpy(malloc((stringLength(rx_buffer_pointer) + 1)*sizeof(char)),rx_buffer_pointer));

	}

	//HERE BE DRAGONS
	return 0; //returns 0
}

