/*----------------------------------------------------------------------------
 Copyright:      Radig Ulrich  mailto: mail@ulrichradig.de
 Author:         Radig Ulrich
 Remarks:        
 known Problems: none
 Version:        24.10.2007
 Description:    RS232 Routinen

 Dieses Programm ist freie Software. Sie können es unter den Bedingungen der 
 GNU General Public License, wie von der Free Software Foundation veröffentlicht, 
 weitergeben und/oder modifizieren, entweder gemäß Version 2 der Lizenz oder 
 (nach Ihrer Option) jeder späteren Version. 

 Die Veröffentlichung dieses Programms erfolgt in der Hoffnung, 
 daß es Ihnen von Nutzen sein wird, aber OHNE IRGENDEINE GARANTIE, 
 sogar ohne die implizite Garantie der MARKTREIFE oder der VERWENDBARKEIT 
 FÜR EINEN BESTIMMTEN ZWECK. Details finden Sie in der GNU General Public License. 

 Sie sollten eine Kopie der GNU General Public License zusammen mit diesem 
 Programm erhalten haben. 
 Falls nicht, schreiben Sie an die Free Software Foundation, 
 Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA. 
------------------------------------------------------------------------------*/
#ifndef _UART_H
	#define _UART_H

	#include <avr/pgmspace.h>
	#include <stdio.h>
	#include "config.h"

	#define USART_ECHO	1

	#define CR "\r"
	#define LF "\n"

	#if LINEFEED == 0
		#define CRLF "\n"
		#define CRLL "\n\n"
	#else
		#define CRLF "\r\n"
		#define CRLL "\r\n\n"
	#endif

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

	char usart_rx_buffer[BUFFER_SIZE];

	struct {
		volatile unsigned char usart_ready:1;
		volatile unsigned char usart_rx_ovl:1;
	}usart_status ;
	
	//Anpassen der seriellen Schnittstellen Register wenn ein ATMega128 benutzt wird
	#if defined (__AVR_ATmega128__)
		#define USR UCSR0A
		#define UCR UCSR0B
		#define UDR UDR0
		#define UBRR UBRR0L
		#define USART_RX USART0_RX_vect 
	#endif
	
	#if defined (__AVR_ATmega644__) || defined (__AVR_ATmega644P__)
		#define USR UCSR0A
		#define UCR UCSR0B
		#define UBRR UBRR0L
		#define EICR EICRB
		#define TXEN TXEN0
		#define RXEN RXEN0
		#define RXCIE RXCIE0
		#define UDR UDR0
		#define UDRE UDRE0
		#define USART_RX USART0_RX_vect   
	#endif
	
	#if defined (__AVR_ATmega32__)
		#define USR UCSRA
		#define UCR UCSRB
		#define UBRR UBRRL
		#define EICR EICRB
		#define USART_RX USART_RXC_vect  
	#endif
	
	#if defined (__AVR_ATmega8__)
		#define USR UCSRA
		#define UCR UCSRB
		#define UBRR UBRRL
	#endif
	
	#if defined (__AVR_ATmega88__)
		#define USR UCSR0A
		#define UCR UCSR0B
		#define UBRR UBRR0L
		#define TXEN TXEN0
		#define UDR UDR0
		#define UDRE UDRE0
	#endif
	//----------------------------------------------------------------------------
	
	void uart_init(unsigned long baudrate); 
	uint8_t uart_putc(uint8_t c, FILE *stream);
//	void usart_write_str(char *str);
//
//	void usart_write_P (const char *Buffer,...);
//	#define printf(format, args...)   usart_write_P(PSTR(format) , ## args)

	void hist_add(char *ptr);
	//----------------------------------------------------------------------------

#endif //_UART_H
