/*
 * usart.c
 *
 *  Created on: 28.08.2013
 *      Author: Grisu
 */
#include "usart.h"
#include "config.h"
#include "cmd.h"
#include "string.h"

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <avr/io.h>
#include <avr/iomxx0_1.h>


//----------------------------------------------------------------------------
// Variables

// counts how many entries are in the command line history
volatile uint8_t hist_fill = 0;

// uart recive line buffer
char usart_rx_buffer[BUFFER_SIZE];

// pointer array for command line history
char *hist_buffer_pointer[HIST_BUFFER_SIZE];
	

//----------------------------------------------------------------------------------------------------
// uart_init() does the initial settings to UART HW and enables it
	//
	// baudrate = numeric value of baud rate to set (e.g. 9600UL)
	//
	// no return
void uart_init(unsigned long baudrate) {
		// Calculate value for baud rate register
//		uint16_t UBRR_BAUD  = ((F_CPU / (16UL * baudrate)) - 1);
//		//set baud rate
//	    UBRR0H = (uint8_t) (UBRR_BAUD>>8);
//	    UBRR0L = (uint8_t) (UBRR_BAUD & 0x0ff);
		UBRR0 = baudrate;
		//UCSR0A|=(1<<U2X0);
	    //enable rx, tx and rx interrupt
	    UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
	    //8 data bits
	    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}


//----------------------------------------------------------------------------------------------------
// uart_putc() puts a char to the uart
	//
	// c = char to output
	// stream = needed for printf macro, set to 0 or NULL
	//
	// returns 0
uint8_t uart_putc(uint8_t c, FILE *stream)
{
	while(!(UCSR0A & (1<<UDRE0))); // while UART is not ready
	UDR0 = c;
	return 0; //returns 0
}


//----------------------------------------------------------------------------------------------------
// ISR (USART0_RX_vect) interrupt service routine for receiving chars
	//
	//
	//
	// no return
ISR (USART0_RX_vect)
{
	static  int 			buffercounter = 0,
							esc_flag1 = 0,
							esc_flag2 = 0,
							histpos = -1;
	unsigned char rx_char;

	// get char from UART data buffer
	rx_char = (UDR0);

	// new line? clear buffer
	if(buffercounter == 0){
		usart_rx_buffer[0] = '\0';
	}

	//handle ESC-Sequence
	//drop all ESC-Sequence Chars and detect selected Sequences
	//TODO: drop following '~' eg. when Page-Up is pressed('\x1b[5~')
	if(rx_char == 0x1b){ //Drop begin of ESC-Sequence
		esc_flag1 = 1;
		return;
	}
	else if((rx_char == '[') && esc_flag1){ //Drop second Char of ESC-Sequence
		esc_flag2 = 1;
		return;
	}
	else if((rx_char == KEY_UP) && esc_flag2){ //Detect Arrow-Up ESC-Sequence an drop char
		esc_flag1 = 0;
		esc_flag2 = 0;
		if(histpos < ((int)hist_fill - 1)){ //not upper end of history?
			// load line buffer with value from history
			strcpy(usart_rx_buffer,hist_buffer_pointer[++histpos]);
			// display history value
			printf(CR"> "ESC_CLRL"%s",usart_rx_buffer);
			// set buffercounter to history string length
			buffercounter = strlen(usart_rx_buffer);
		}
		return;
	}
	else if((rx_char == KEY_DOWN) && esc_flag2){ //Detect Arrow-Down ESC-Sequence an drop char
		esc_flag1 = 0;
		esc_flag2 = 0;
		if(histpos > 0){ // not lower end of history?
			// load line buffer with value from history
			strcpy(usart_rx_buffer,hist_buffer_pointer[--histpos]);
			//print history value
			printf(CR"> "ESC_CLRL"%s",usart_rx_buffer);
			// set buffercounter to history string length
			buffercounter = strlen(usart_rx_buffer);
		}
		else if(histpos>-1){ // lower end of history?
			// go back to normal input
			printf(CR"> "ESC_CLRL);
			usart_rx_buffer[0] = '\0';
			buffercounter = 0;
			histpos--;
		}
		return;
	}
	else if (esc_flag2){ //drop any other ESC-Sequence char
		esc_flag1 = 0;
		esc_flag2 = 0;
		return;
	}
	else{ // ESC-Sequence incomplete
		esc_flag1 = 0;
		esc_flag2 = 0;
	}

// only if we should echo
#if USART_ECHO
	// if buffer counter is empty, we won't respond to del or backspace
	if(!((buffercounter == 0) && (rx_char == 0x08 || rx_char == 0x7F)))
		uart_putc(rx_char,NULL);
#endif

	// line buffer is full? then we indicate an overflow
	if (usart_status.usart_ready){
		usart_status.usart_rx_ovl = 1;
		return;
	}
	
	//handle backspace
		//some Terminals use DEL (0x7f) instead
		//see below
	else if (rx_char == 0x08){
		if (buffercounter){
			buffercounter--;
			#if USART_ECHO
			printf(ESC_CLRL);
			#endif
		}
		return;
	}

	//handle DEL
		//some Terminals use backspace (0x08) instead
		//see above
	else if (rx_char == 0x7F){
		if (buffercounter){
			buffercounter--;
			#if USART_ECHO
			printf(ESC_CLRL);
			#endif
		}
		return;
	}

	// received return?
	else if (rx_char == '\r' && (!(usart_rx_buffer[buffercounter - 1] == '\\'))){
		usart_rx_buffer[buffercounter] = 0; // terminate string
		buffercounter = 0;
		histpos = -1;
		usart_status.usart_ready = 1; // indicate that string is ready for parsing
		return;
	}

	// not ant and of buffer? then add char
	else if (buffercounter < BUFFER_SIZE - 1){
		usart_rx_buffer[buffercounter++] = rx_char;
		return;
	}

	// last option is, that buffer is full.
	//  so we terminate and indicate that it is ready
	else{
		usart_rx_buffer[buffercounter] = 0;
		buffercounter = 0;
		histpos = -1;
		usart_status.usart_ready = 1;
	}
	return;
}


//----------------------------------------------------------------------------------------------------
// hist_add() adds an entry to history buffer
	//
	// ptr = pointer to string to add
	//
	// no return
void hist_add(char *ptr){

	if(hist_fill == HIST_BUFFER_SIZE) //when history buffer is full
		// discard oldest entry
		free(hist_buffer_pointer[HIST_BUFFER_SIZE - 1]);
	else
		// rise the fill state
		hist_fill++;


	// move all entries one position up
	for(int i = (HIST_BUFFER_SIZE-2); i >= 0; i--)
		hist_buffer_pointer[i + 1] = hist_buffer_pointer[i];

	// add new history entry
	hist_buffer_pointer[0] = ptr;
}
