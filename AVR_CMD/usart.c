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

volatile uint8_t hist_fill = 0;

char usart_rx_buffer[BUFFER_SIZE];
char *hist_buffer_pointer[HIST_BUFFER_SIZE];
	

void uart_init(unsigned long baudrate) {
		uint16_t UBRR_BAUD  = ((F_CPU / (16UL * baudrate)) - 1);
	    UBRR0H = (uint8_t) (UBRR_BAUD>>8);
	    UBRR0L = (uint8_t) (UBRR_BAUD & 0x0ff);
	    UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
	    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}


uint8_t uart_putc(uint8_t c, FILE *stream)
{
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = c;
	return 0;
}


ISR (USART0_RX_vect)
{
	static  int 			buffercounter = 0,
							esc_flag1 = 0,
							esc_flag2 = 0,
							histpos = -1;
	unsigned char rx_char;

	rx_char = (UDR0);

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
		if(histpos < ((int)hist_fill - 1)){
			strcpy(usart_rx_buffer,hist_buffer_pointer[++histpos]);
			printf(CR"> "ESC_CLRL"%s",usart_rx_buffer);
			buffercounter = strlen(usart_rx_buffer);
		}
		return;
	}
	else if((rx_char == KEY_DOWN) && esc_flag2){ //Detect Arrow-Down ESC-Sequence an drop char
		esc_flag1 = 0;
		esc_flag2 = 0;
		if(histpos > 0){
			strcpy(usart_rx_buffer,hist_buffer_pointer[--histpos]);
			printf(CR"> "ESC_CLRL"%s",usart_rx_buffer);
			buffercounter = strlen(usart_rx_buffer);
		}
		else if(histpos>-1){
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


	#if USART_ECHO
	if(!((buffercounter == 0) && (rx_char == 0x08 || rx_char == 0x7F)))
		uart_putc(rx_char,NULL);
	#endif

	if (usart_status.usart_ready){
		usart_status.usart_rx_ovl = 1;
		return;
	}
	
	//handle backspace
		//some Terminals use DEL (0x7f) instead
		//see below
	if (rx_char == 0x08){
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
	if (rx_char == 0x7F){
		if (buffercounter){
			buffercounter--;
			#if USART_ECHO
			printf(ESC_CLRL);
			#endif
		}
		return;
	}

	if (rx_char == '\r' && (!(usart_rx_buffer[buffercounter - 1] == '\\'))){
		usart_rx_buffer[buffercounter] = 0;
		buffercounter = 0;
		histpos = -1;
		usart_status.usart_ready = 1;
		return;
	}

	if (buffercounter < BUFFER_SIZE - 1)
		usart_rx_buffer[buffercounter++] = rx_char;
	else{
		usart_rx_buffer[buffercounter] = 0;
		buffercounter = 0;
		histpos = -1;
		usart_status.usart_ready = 1;
	}
	return;
}


void hist_add(char *ptr){
	if(hist_fill == HIST_BUFFER_SIZE)
		free(hist_buffer_pointer[HIST_BUFFER_SIZE - 1]);
	for(int i = (HIST_BUFFER_SIZE-2); i >= 0; i--){
		hist_buffer_pointer[i + 1] = hist_buffer_pointer[i];
	}
	hist_buffer_pointer[0] = ptr;
	if(hist_fill < HIST_BUFFER_SIZE)
		hist_fill++;

}
