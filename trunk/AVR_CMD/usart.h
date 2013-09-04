/*
 * usart.h
 *
 *  Created on: 28.08.2013
 *      Author: Grisu
 */
#ifndef _UART_H
	#define _UART_H
//----------------------------------------------------------------------------
// Includes

	#include <avr/pgmspace.h>
	#include <stdio.h>
	#include "config.h"

//----------------------------------------------------------------------------
// Defines

	// should UART echo the received inputs
	#define USART_ECHO	1 // 0=off, 1=on

	// defines for line feeds to use by user
	#define CR "\r"
	#define LF "\n"

	#if LINEFEED == 0
		#define CRLF "\n"
		#define CRLL "\n\n"
	#else
		#define CRLF "\r\n"
		#define CRLL "\r\n\n"
	#endif

	// defines for escape strings to use by user
	#define ESC_DOWN "\x1b[B"
	#define ESC_CLRL "\x1b[K"
	#define ESC_CLS "\x1b[2J\x1b[H"
	#define KEY_UP 'A'
	#define KEY_DOWN 'B'
	#define ESC_CLEAR    "\e[0m"
	#define ESC_BOLD     "\e[1m"
	#define ESC_BLACK    "\e[30m"
	#define ESC_RED      "\e[31m"
	#define ESC_GREEN    "\e[32m"
	#define ESC_YELLOW   "\e[33m"
	#define ESC_BLUE     "\e[34m"
	#define ESC_MAGENTA  "\e[35m"
	#define ESC_CYAN     "\e[36m"
	#define ESC_WHITE    "\e[37m"

//----------------------------------------------------------------------------
// Variables

	// uart recive line buffer
	char usart_rx_buffer[BUFFER_SIZE];

	// struct for global uart status
	struct {
		volatile unsigned char usart_ready:1;
		volatile unsigned char usart_rx_ovl:1;
	}usart_status ;
	
	
//----------------------------------------------------------------------------
// Function prototypes
	
	void uart_init(unsigned long baudrate); 
	uint8_t uart_putc(uint8_t c, FILE *stream);
	void hist_add(char *ptr);


#endif //_UART_H
