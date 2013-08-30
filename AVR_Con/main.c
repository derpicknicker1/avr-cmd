/*
 * main.c
 *
 *  Created on: 28.08.2013
 *      Author: Grisu
 */

#include <avr/interrupt.h>
#include "usart.h"
#include "cmd.h"

#define USE_SD 1 //also set in cmd.h

#if USE_SD == 1
	#include "sd/mmc_config.h"	// Hier werden alle noetigen Konfigurationen vorgenommen, umbedingt anschauen !
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

	// sd/mmc config  **************************************************
	if( FALSE == mmc_init() ){
		usart_write_str(CRLF"ERR | MMC-Init: System halted");
		return 0;
	}

	usart_write_str("...");

	// fat config ******************************************************
	if( FALSE == fat_loadFatData() ){
		usart_write_str(CRLF"ERR | FAT-Init: System halted");
		return 0;
	}

	// wenn auf dem terminal "Boot...OK" zu lesen ist, war initialisierung erfolgreich!
	usart_write_str("OK"CRLL);
#endif


	while(1){

		usart_write("> ");
		usart_status.usart_ready=0;

		while(!usart_status.usart_ready);
		usart_write_str(CRLF);
		parseLine(usart_rx_buffer);

	}

	//HERE BE DRAGONS
	return 0; //returns 0
}

