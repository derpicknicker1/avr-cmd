/*
 * main.c
 *
 *  Created on: 28.08.2013
 *      Author: Grisu
 *
 *      This is the Version for Mega2560 Hardware.
 *      Using USART0 @ 9600Baud,8N1
 */

#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "usart.h"
#include "cmd.h"
#include "string.h"

// only needed if SD is in use
#if USE_SD == 1
	#include "sd/mmc_config.h"
	#include "sd/file.h"
	#include "sd/fat.h"
	#include "sd/mmc.h"
#endif

// this magic pice of code redirects printf to uart
FILE mystdout = FDEV_SETUP_STREAM(uart_putc, NULL, _FDEV_SETUP_WRITE);

int main(void){

	//all ports output low
	DDRA=DDRB=DDRC=DDRD=0xFF;
	PORTA=PORTB=PORTC=PORTD=0x00;

	//init uart
	uart_init(BAUD);

	// some more magic for printf
	stdout = &mystdout;

	// enable interrupts
	sei();

	printf(ESC_CLS""ESC_GREEN""ESC_BOLD"AVR-Con alpha 0.1"CRLF""ESC_CLEAR);


#if USE_SD == 1

	// initialize file_arg list
	file_args_init();
	printf("Boot SD");

	// init mmc
	if( FALSE == mmc_init() ){
		printf(CRLF""ESC_RED"ERR | MMC-Init: System halted"ESC_CLEAR);
		return 0;
	}

	printf("...");

	//init fat
	if( FALSE == fat_loadFatData() ){
		printf(CRLF""ESC_RED"ERR | FAT-Init: System halted"ESC_CLEAR);
		return 0;
	}
#endif

	printf(ESC_GREEN"OK"ESC_CLEAR""CRLL"> ");
	while(1){

		// when new command line received
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

