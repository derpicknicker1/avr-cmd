/*
 * main.c
 *
 *  Created on: 28.08.2013
 *      Author: Grisu
 */
#include <util/delay.h>
#include "usart.h"
#include "cmd.h"
#include <avr/interrupt.h>


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

	while(1){

		usart_write("> ");
		usart_status.usart_ready=0;

		while(!usart_status.usart_ready);
				if(stringLength(usart_rx_buffer) < 2){
					//printf("Makro: %s\n",line);
					if(stringCompare(usart_rx_buffer,"W") ==0){
						parseLine("set power 1");
					}
					if(stringCompare(usart_rx_buffer,"w") == 0){
						parseLine("set power 0");
					}
				}
				else{
					parseLine(usart_rx_buffer);
				}

	}

	//HERE BE DRAGONS
	return 0; //returns 0
}

