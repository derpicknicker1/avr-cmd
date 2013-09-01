/*
 * cmd.c
 *
 *  Created on: 28.08.2013
 *      Author: Grisu
 */
#include <stdio.h>
#include <util/delay.h>
#include "usart.h"
#include "cmd.h"
#include "config.h"

#if USE_SD == 1
	#include "sd/mmc_config.h"	// Hier werden alle noetigen Konfigurationen vorgenommen, unbedingt anschauen !
	#include "sd/file.h"
	#include "sd/fat.h"
	#include "sd/mmc.h"
#endif

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
	else if(value[0] == 'O'){
		if((value[1] == 'N') || ((value[1] == 'U') && (value[2] == 'T'))){
			val = 1;
			status = CONV_SW;
		}
		else if((value[1] == 'F') && value[2] == 'F'){
			val = 0;
			status = CONV_SW;
		}
		else{
			val=0;
			status = ERROR;
		}
	}
	else if(value[0] == 'I'){
		if(value[1] == 'N'){
			val = 0;
			status = CONV_SW;
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


//TODO: mask out usart pins???
int8_t executeSet(char* par, uint16_t val){
	uint8_t reg = 0, offset = 4;

	if(stringLength(par) > 1){
		switch(par[0]){
			case 'D': reg = DDR_REG; break;
			case 'P': reg = PORT_REG; break;
		}
		offset = 3 - (par[1] - 65);
		if((reg > 0) && (offset < 4)){
			if(stringLength(par) == 3){
				if((par[2] >= 0x30) && (par[2] <= 0x37)){
					if(val)
						(_SFR_IO8(reg + (offset * 3))) |= (1 << (par[2] - 0x30));
					else
						(_SFR_IO8(reg + (offset * 3))) &= ~(1 << (par[2] - 0x30));
					return 1;
				}
			}
			else{
				 _SFR_IO8(reg + (offset  *3)) = val;
				 return 1;
			}
		}
	}
	return ERROR;
}

int16_t executeGet(char* par){
	uint8_t reg = 0, offset = 4;

	if(stringLength(par) > 1){
		switch(par[0]){
			case 'P': reg = PIN_REG; break;
			case 'D': reg = DDR_REG; break;
			case 'O': reg = PORT_REG; break;
		}
		offset = 3 - (par[1] - 65);
		if((reg > 0) && (offset < 4)){
			if(stringLength(par) == 3){
				if((par[2] >= 0x30) && (par[2] <= 0x37))
					return ((_SFR_IO8(reg + (offset * 3))) >> (par[2] - 0x30)) & 0x01;
			}
			else
				return _SFR_IO8(reg + (offset * 3));
		}
	}
	return ERROR;
}

void parseLine(char* line){

	char cmd[BUF_C];
	char parameter[BUF_P];
	char value[BUF_V];

	get_group_from_line(0,line,cmd);
	get_group_from_line(1,line,parameter);
	get_group_from_line(2,line,value);
	toLower(cmd);
	toUpper(parameter);
	toUpper(value);

//SET
#if CMD_SET == 1
	if(stringCompare(cmd,CMD_SET_STR) == 0){
		if(stringLength(parameter) > 0){
			uint16_t parsedValue = 0;
			int8_t status = parseValue(value,&parsedValue);
			if(executeSet(parameter,parsedValue) > ERROR)
				usart_write("SET | %i: %s = %i"CRLF,status,parameter,parsedValue);
			else
				usart_write("ERR | %i: %s = %i"CRLF,status,parameter,parsedValue);
		}
		cmd[0]='\0';
	}
#endif

//GET
#if CMD_SET == 1
	if(stringCompare(cmd,CMD_GET_STR) == 0){
		int16_t status = -1;
		if(stringLength(parameter) > 0){
			if((status = executeGet(parameter)) > ERROR)
				usart_write("GET |  %s = %i"CRLF,parameter,status);
			else
				usart_write("ERR |  %s = %i"CRLF,value,status);
		}
		cmd[0]='\0';
	}
#endif


//OPEN
#if USE_SD == 1
#if CMD_OPEN == 1
	if(stringCompare(cmd,CMD_OPEN_STR) == 0){
		uint32_t seek;
		if(stringLength(parameter) > 0){
			if( MMC_FILE_OPENED == ffopen((uint8_t*)parameter,'r') ){
				seek = file.length;
				usart_write("OPEN | %s"CRLL,parameter);
				char line_buf[40] = {0};
				uint8_t cnt = 0;
				do{
					do{
						line_buf[cnt++] = ffread();
							if(line_buf[cnt-1] == '\r'){
								line_buf[cnt-1] = '\0';
								usart_write("  > %s"CRLF"    ",line_buf);
								parseLine(line_buf);
								usart_write(CRLF);
								cnt = 0;
								line_buf[cnt] = '\0';
								ffread();
								seek--;
							}
					}while(--seek && (cnt < 40));
					cnt = 0;
					line_buf[cnt] = '\0';
				}while(seek);
				ffclose();
				usart_write_str(CRLF);
			}
			else
				usart_write("ERR |  OPEN %s"CRLF,parameter);
		}
		cmd[0] = '\0';
	}
#endif

//DELAY - only needed with SD-Card support for scripting
#if CMD_DELAY == 1
	if(stringCompare(cmd,CMD_DELAY_STR) == 0){
		uint16_t parsedValue = 0;
		int8_t status = parseValue(value,&parsedValue);
		if((parameter[0] == 'M') && (status > ERROR)){
			usart_write("DELAY |  %sS = %i"CRLF,parameter,parsedValue);
			for(uint16_t i = 0; i < parsedValue; i++)
				_delay_ms(1);

		}
		else if((parameter[0] == 'U') && (status > ERROR)){
			usart_write("DELAY |  %sS = %i"CRLF,parameter,parsedValue);
			for(uint16_t i = 0; i < parsedValue; i++)
				_delay_us(1);
		}
		else
			usart_write("ERR |  %s = %i"CRLF,value,status);
		cmd[0] = '\0';
	}
#endif
#endif//USE_SD

//!DEFAULT
	if(cmd[0]){
		usart_write("ERR | Unknown Command: %s"CRLF,cmd);
	}
}

char *my_strcpy(char *destination, char *source){
    char *p = destination;
    while (*source != '\0')
        *p++ = *source++;
    *p = '\0';
    return destination;
}
