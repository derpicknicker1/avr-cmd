/*
 * main.c
 *
 *  Created on: 28.08.2013
 *      Author: Grisu
 */

#include <avr/interrupt.h>
#include <stdlib.h>
//#include <stddef.h>
#include <stdio.h>
#include "config.h"
#include "usart.h"
#include "cmd.h"
#include "string.h"

#if USE_SD == 1
	#include "sd/mmc_config.h"	// Hier werden alle noetigen Konfigurationen vorgenommen, unbedingt anschauen !
	#include "sd/file.h"
	#include "sd/fat.h"
	#include "sd/mmc.h"
#endif

FILE mystdout = FDEV_SETUP_STREAM(usart_putchar, NULL, _FDEV_SETUP_WRITE);

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
	stdout = &mystdout;
	sei();
	printf(CRLL"AVR-Con alpha 0.1"CRLF);

#if USE_SD == 1
	printf("Boot SD");

	if( FALSE == mmc_init() ){
		printf(CRLF"ERR | MMC-Init: System halted");
		return 0;
	}

	printf("...");

	if( FALSE == fat_loadFatData() ){
		printf(CRLF"ERR | FAT-Init: System halted");
		return 0;
	}
#endif

	printf("OK"CRLL);

	while(1){

		printf("> ");

		//wait for line input from usart ISR
		usart_status.usart_ready=0;
		while(!usart_status.usart_ready);

		printf(CRLF);

		//parse line (exec cmd)
		parse_line(usart_rx_buffer);

		//add line to cmd history
		hist_add(strcpy(malloc((strlen(usart_rx_buffer) + 1)*sizeof(char)),usart_rx_buffer));

	}
	
	/*        .==.        .==.
	//       //`^\\      //^`\\
	//      // ^ ^\(\__/)/^ ^^\\
	//     //^ ^^ ^/6  6\ ^^ ^ \\
	//    //^ ^^ ^/( .. )\^ ^ ^ \\
	//   // ^^ ^/\| v""v |/\^ ^ ^\\
	//  // ^^/\/ /  `~~`  \ \/\^ ^\\
	//  -----------------------------
	/// HERE BE DRAGONS */

	return 0; //returns 0
	
}

