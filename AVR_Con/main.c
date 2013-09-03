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

	printf(ESC_CLS""ESC_GREEN""ESC_BOLD"AVR-Con alpha 0.1"CRLF""ESC_CLEAR);

	file_args_init();
#if USE_SD == 1
	printf("Boot SD");

	if( FALSE == mmc_init() ){
		printf(CRLF""ESC_RED"ERR | MMC-Init: System halted"ESC_CLEAR);
		return 0;
	}

	printf("...");

	if( FALSE == fat_loadFatData() ){
		printf(CRLF""ESC_RED"ERR | FAT-Init: System halted"ESC_CLEAR);
		return 0;
	}
#endif

	printf(ESC_GREEN"OK"ESC_CLEAR""CRLL"> ");
	while(1){

		if(usart_status.usart_ready==1){
			printf(CRLF);

			//parse line (exec cmd)
			parse_line(usart_rx_buffer);

			//add line to cmd history
			hist_add(strcpy(malloc((strlen(usart_rx_buffer) + 1)*sizeof(char)),usart_rx_buffer));

			//get ready for next line
			printf(CRLF"> ");
			usart_status.usart_ready=0;

		}






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

