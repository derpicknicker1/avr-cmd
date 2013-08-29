/*
 * cmd.c
 *
 *  Created on: 28.08.2013
 *      Author: Grisu
 */
#include <stdio.h>
#include "usart.h"
#include "cmd.h"

void get_group_from_line(uint8_t position, char* line, char* output){
	uint8_t spaces = 0;
	uint8_t i = 0;
	while(line[i] != '\0'){
		if(line[i] == ' '){
			spaces++;
			while(line[++i]==' ');
		}
		if(position == spaces){
			*output++=line[i];
		}
		i++;

	}
	*output = '\0';
}

int16_t HexWordToValue(char word){
	if(word >= 0x30 && word <= 0x39){	// zahl
		return (word-0x30);
	}
	else if(word >= 0x41 && word <=0x46){ // grosser buchstabe
		return (word-0x37);
	}
	else if(word >= 0x61 && word <= 0x66){ // kleiner buchstabe
		return (word-0x57);
	}
	return -1;
}


int8_t AsciiToValue(char word){
	if(word >= 0x30&& word <= 0x39){	// zahl
		return (word-0x30);
	}
	return -1;
}

void toUpper(char* in){
	do{
		if(*in >=0x61 && *in <=0x7a)
			*in -= 32;
	}while(*in++);
}

void toLower(char* in){
	do{
		if(*in >=0x41 && *in <=0x5a)
			*in += 32;
	}while(*in++);
}

int8_t parseValue(char* value, uint16_t* out){

	uint16_t val = 0;
	uint8_t i = 0;
	int8_t status = ERROR;

	if((value[0] == '0') && ((value[1] == 'x') || (value[1] == 'b') || (value[1] == 'o'))){	//maybe hex, binary or octal
	// HEX
		if(value[1] == 'x'){	// hex
			uint8_t i = 2;
			status = CONV_H;
			while(value[i] != '\0'){
				int16_t decValue = HexWordToValue(value[i]);

				if(decValue != -1){
					val = (val << 4) + decValue;
					i++;
				}
				else{
					val = 0;
					status = ERROR;
					break;
				}
			}
		}
	// BINARY
		else if(value[1] == 'b'){
			uint8_t i = 2;
			status = CONV_B;
			while(value[i] != '\0'){
				if(value[i] == '0'){
					val = (val << 1) + 0;
				}
				else if(value[i] == '1'){
					val = (val << 1) + 1;
				}
				else{
					status = ERROR;
					val = 0;
					break;
				}
				i++;
			}
		}
	// OCTAL
		else if(value[1] == 'o'){
			uint8_t i = 2;
			status = CONV_O;
			while(value[i] != '\0'){
				if(value[i] >= 0x30 && value[i] <= 0x37){
					val = val * 8  + AsciiToValue(value[i]);
					i++;
				}
				else{
					val = 0;
					status = ERROR;
					break;
				}
			}
		}
	}
	// ON / OFF
	else if(value[0] == 'o' || value[0] == 'O'){
		if(value[1] == 'n' || value[1] == 'N'){
			val = 1;
			status = CONV_SW;
		}
		else if((value[1] == 'f') || ((value[1] == 'F') &&
				(value[2] == 'f')) || (value[2] == 'F')){
			val = 0;
			status = CONV_SW;
		}
		else{
			val=0;
			status = ERROR;
		}
	}
	// DECIMAL
	else{
		status = CONV_D;
		while(value[i] != '\0'){
			if(value[i] >= 0x30 && value[i] <= 0x37)
				val = val*10 + AsciiToValue(value[i]);
			else{
				val = 0;
				status = ERROR;
				break;
			}
			i++;
		}

	}

	*out = val;
	return status;
}

uint8_t stringLength(char* a){
	uint8_t i = 0;
	while(a[i] != '\0'){
		i++;
	}
	return i;
}

int8_t stringCompare(char* a, char* b){
	uint8_t i = 0;
	if(stringLength(b) > stringLength(a))
		return -1;
	while(a[i] != '\0'){
		if(a[i] != b[i]){
			return i+1;
		}
		i++;
	}
	return 0;
}

int8_t executeSet(char* par, uint16_t val){
	toUpper(par);
	usart_write(CRLF"%s"CRLF,par);
	if((par[0] == 'P') && (stringLength(par) == 2)){
		switch(par[1]){
		case 'A': PORTA = val; return 1; break;
		case 'B': PORTB = val; return 1; break;
		case 'C': PORTC = val; return 1; break;
		case 'D': 	val &= 0xFC; // protect USART
					PORTD = val; return 1; break;
		default: return 0; break;
		}
	}
	else if((par[0] == 'P') && (stringLength(par) == 3)){
		par[2] -= 0x30;
		if((par[2] >= 0) && (par[2] <= 7)){
			switch(par[1]){
				case 'A': (val)?(PORTA |= (1 << (par[2]))):(PORTA &= ~(1 << (par[2]))); return 1; break;
				case 'B': (val)?(PORTB |= (1 << (par[2]))):(PORTB &= ~(1 << (par[2]))); return 1; break;
				case 'C': (val)?(PORTC |= (1 << (par[2]))):(PORTC &= ~(1 << (par[2]))); return 1; break;
				case 'D':
					if(par[2]<2){ //protect USART
						(val)?(PORTD |= (1 << (par[2]))):(PORTD &= ~(1 << (par[2])));
						return 1;
					}
					else
						return 0;
					break;
				default: return 0; break;
			}
		}
		else{
			return 0;
		}

	}

	return 1;
}

void parseLine(char* line){

	char cmd[BUF_C];

	get_group_from_line(0,line,cmd);

//SET
	if(stringCompare(cmd,CMD_SET) == 0){
		char parameter[BUF_P];
		char value[BUF_V];
		get_group_from_line(1,line,parameter);
		get_group_from_line(2,line,value);
		if(stringLength(parameter)>0){
			uint16_t parsedValue = 0;
			int8_t status = parseValue(value,&parsedValue);
//			TODO: DO SET STUFF HERE.
			executeSet(parameter,parsedValue);
			usart_write(CRLF"%i: %s = %i"CRLF,status,parameter,parsedValue);
		}
	}
//GET
	else if(stringCompare(cmd,CMD_GET) == 0){
		char parameter[BUF_P];
		get_group_from_line(1,line,parameter);
		if(stringLength(parameter)>0){
//			TODO: DO GET STUFF HERE.
			usart_write(CRLF"%i: %s = %i"CRLF,0,parameter,255);
		}
	}
//!DEFAULT
	else{
		usart_write(CRLF"Unknown Command: %s"CRLF,cmd);
	}
}
