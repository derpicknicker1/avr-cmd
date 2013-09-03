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

	
volatile unsigned int 	buffercounter = 0,
						esc_flag1 = 0,
						esc_flag2 = 0,
						hist_fill = 0;
volatile int histpos = -1;

char usart_rx_buffer[BUFFER_SIZE];
char *hist_buffer_pointer[HIST_BUFFER_SIZE];
	
//----------------------------------------------------------------------------
//Init serielle Schnittstelle
void usart_init(unsigned long baudrate) 
{ 
  	//Enable TXEN im Register UCR TX-Data Enable
	UCR =(1 << TXEN | 1 << RXEN | 1<< RXCIE);
	UBRR=(F_CPU / (baudrate * 16L) - 1);
}

//----------------------------------------------------------------------------
//Routine für die Serielle Ausgabe eines Zeichens (Schnittstelle0)
uint8_t usart_putchar(uint8_t c, FILE *stream)
{
	//Warten solange bis Zeichen gesendet wurde
	while(!(USR & (1<<UDRE)));
	//Ausgabe des Zeichens
	UDR = c;
	return 0;
}

//------------------------------------------------------------------------------
//void usart_write_P (const char *Buffer,...)
//{
//	va_list ap;
//	va_start (ap, Buffer);
//
//	int format_flag;
//	char str_buffer[10];
//	char str_null_buffer[10];
//	char move = 0;
//	char Base = 0;
//	int tmp = 0;
//	char by;
//	char *ptr;
//
//	//Ausgabe der Zeichen
//    for(;;)
//	{
//		by = pgm_read_byte(Buffer++);
//		if(by==0) break; // end of format string
//
//		if (by == '%')
//		{
//            by = pgm_read_byte(Buffer++);
//			if (isdigit(by)>0)
//				{
//
// 				str_null_buffer[0] = by;
//				str_null_buffer[1] = '\0';
//				move = atoi(str_null_buffer);
//                by = pgm_read_byte(Buffer++);
//				}
//
//			switch (by)
//				{
//                case 's':
//                    ptr = va_arg(ap,char *);
//                    while(*ptr) { usart_putchar(*ptr++,NULL); }
//                    break;
//				case 'b':
//					Base = 2;
//					goto ConversionLoop;
//				case 'c':
//					//Int to char
//					format_flag = va_arg(ap,int);
//					usart_putchar (format_flag++,NULL);
//					break;
//				case 'i':
//					Base = 10;
//					goto ConversionLoop;
//				case 'o':
//					Base = 8;
//					goto ConversionLoop;
//				case 'x':
//					Base = 16;
//					//****************************
//					ConversionLoop:
//					//****************************
//					itoa(va_arg(ap,int),str_buffer,Base);
//					int b=0;
//					while (str_buffer[b++] != 0){};
//					b--;
//					if (b<move)
//						{
//						move -=b;
//						for (tmp = 0;tmp<move;tmp++)
//							{
//							str_null_buffer[tmp] = '0';
//							}
//						//tmp ++;
//						str_null_buffer[tmp] = '\0';
//						strcat(str_null_buffer,str_buffer);
//						strcpy(str_buffer,str_null_buffer);
//						}
//					usart_write_str (str_buffer);
//					move =0;
//					break;
//				}
//
//			}
//		else
//		{
//			usart_putchar ( by , NULL);
//		}
//	}
//	va_end(ap);
//}
//
////----------------------------------------------------------------------------
////Ausgabe eines Strings
//void usart_write_str(char *str)
//{
//	while (*str)
//	{
//		usart_putchar(*str++,NULL);
//	}
//}

//----------------------------------------------------------------------------
//Empfang eines Zeichens

ISR (USART_RX)
{

	unsigned char receive_char;
	receive_char = (UDR);
	if(buffercounter == 0){
		usart_rx_buffer[0] = '\0';
	}

	//handle ESC-Sequence
	//drop all ESC-Sequence Chars and detect selected Sequences
	//TODO: drop following '~' eg. when Page-Up is pressed('\x1b[5~')
	if(receive_char == 0x1b){ //Drop begin of ESC-Sequence
		esc_flag1 = 1;
		return;
	}
	else if((receive_char == '[') && esc_flag1){ //Drop second Char of ESC-Sequence
		esc_flag2 = 1;
		return;
	}
	else if((receive_char == KEY_UP) && esc_flag2){ //Detect Arrow-Up ESC-Sequence an drop char
		esc_flag1 = 0;
		esc_flag2 = 0;
		if(histpos < ((int)hist_fill - 1)){
			strcpy(usart_rx_buffer,hist_buffer_pointer[++histpos]);
			printf(CR"> "ESC_CLRL"%s",usart_rx_buffer);
			buffercounter = strlen(usart_rx_buffer);
		}
		return;
	}
	else if((receive_char == KEY_DOWN) && esc_flag2){ //Detect Arrow-Down ESC-Sequence an drop char
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
	if(!((buffercounter == 0) && (receive_char == 0x08 || receive_char == 0x7F)))
		usart_putchar(receive_char,NULL);
	#endif

	if (usart_status.usart_ready){
		usart_status.usart_rx_ovl = 1;
		return;
	}
	
	//handle backspace
		//some Terminals use DEL (0x7f) instead
		//see below
	if (receive_char == 0x08){
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
	if (receive_char == 0x7F){
		if (buffercounter){
			buffercounter--;
			#if USART_ECHO
			printf(ESC_CLRL);
			#endif
		}
		return;
	}

	if (receive_char == '\r' && (!(usart_rx_buffer[buffercounter - 1] == '\\'))){
		usart_rx_buffer[buffercounter] = 0;
		buffercounter = 0;
		histpos = -1;
		usart_status.usart_ready = 1;
		return;
	}

	if (buffercounter < BUFFER_SIZE - 1)
		usart_rx_buffer[buffercounter++] = receive_char;
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
