/*
 * cmd.c
 *
 *  Created on: 28.08.2013
 *      Author: Grisu
 */
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include "usart.h"
#include "strings.h"
#include "cmd.h"
#include "config.h"

#if USE_SD == 1
	#include "sd/mmc_config.h"	// Hier werden alle noetigen Konfigurationen vorgenommen, unbedingt anschauen !
	#include "sd/file.h"
	#include "sd/fat.h"
	#include "sd/mmc.h"
#endif


static void get_group_from_line(uint8_t position, char* line, char* output);
static int8_t parseValue(char* value, uint16_t* out);
static int8_t cmd_set(void);
static int8_t cmd_open(void);
static int8_t cmd_delay(void);
static int8_t cmd_print(void);

uint16_t cmd_vars[VAR_BUF] = {0};
char* val_ptr[VAL_BUF] = {0};


COMMAND_STRUCTUR COMMAND_TABELLE[] = // Befehls-Tabelle
{
#if CMD_SET == 1
	{"SET",cmd_set},
#endif
#if CMD_OPEN == 1
	{"PRINT",cmd_print},
#endif
//SD Functions
#if USE_SD == 1
	#if CMD_OPEN == 1
		{"OPEN",cmd_open},
	#endif
	#if CMD_OPEN == 1
		{"DELAY",cmd_delay},
	#endif
#endif
	{NULL,NULL}
};


void parseLine(char* line){

	char gr_buf[VAL_TMP_BUF];
	uint8_t i = 0;

	//get groups of instructions -> stored in val_ptr[]
	get_group_from_line(0,line,gr_buf);
	while(strLen(gr_buf) > 0 && i < VAL_BUF){
		toUpper(gr_buf);
		free(val_ptr[i]);
		val_ptr[i] = strCpy(malloc((strLen(gr_buf)+1)*sizeof(char)),gr_buf);
		i++;
		get_group_from_line(i,line,gr_buf);
	}

	//search command
	i = 0;
	while(strCmp(COMMAND_TABELLE[i].cmd,val_ptr[0])){
		//if CMD not found
		if (COMMAND_TABELLE[++i].cmd == 0) {
			usart_write("ERR | Unknown Command: %s"CRLF,val_ptr[0]);
			return;
		}
	}

	//Exec command
	COMMAND_TABELLE[i].fp();

	//free instructions memory and set to '\0'
	for(i=0;i<VAL_BUF;i++){
		free(val_ptr[i]);
		val_ptr[i]=strCpy(malloc(sizeof(char)), '\0');
	}
}


static void get_group_from_line(uint8_t position, char* line, char* output){
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


static int8_t parseValue(char* value, uint16_t* out){

	uint16_t val = 0;
	uint8_t i = 0;
	int8_t status = ERROR;

	if((value[0] == '0') && ((value[1] == 'X') || (value[1] == 'B') || (value[1] == 'O'))){	//maybe hex, binary or octal
	// HEX
		if(value[1] == 'X'){	// hex
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
		else if(value[1] == 'B'){
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
		else if(value[1] == 'O'){
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
	// ON / OUT
	else if((value[0] == 'O') && ((value[1] == 'N') || ((value[1] == 'U') && (value[2] == 'T')))){
		val = 1;
		status = CONV_SW;
	}
	//OFF
	else if((value[0] == 'O') && (value[1] == 'F') && value[2] == 'F'){
		val = 0;
		status = CONV_SW;
	}
	//IN
	else if(value[0] == 'I'&& value[1] == 'N'){
		val = 0;
		status = CONV_SW;
	}
	//VAR or REG
	else if(value[0]=='P' || value[0] == 'D' || value[0] == 'O' || value[0] == '$'){
		uint8_t reg = 0, offset = 4;
		switch(value[0]){
			case 'P': reg = PIN_REG; break;
			case 'D': reg = DDR_REG; break;
			case 'O': reg = PORT_REG; break;
		}
		offset = 3 - (value[1] - 65);
		if((reg > 0) && (offset < 4)){
			if(strLen(value) == 3){
				if((value[2] >= 0x30) && (value[2] <= 0x37)){
					val = ((_SFR_IO8(reg + (offset * 3))) >> (value[2] - 0x30)) & 0x01;
					status = CONV_REG;
				}
			}
			else{
				val = _SFR_IO8(reg + (offset * 3));
				status = CONV_REG;
			}
		}
		else if(strLen(value) > 2 && value[0] == '$'){
			if((value[1] >= 0x30) && (value[1] <= 0x39) && (value[2] >= 0x30) && (value[2] <= 0x39)){
				val = cmd_vars[((value[1] - 0x30) * 10) + (value[2] - 0x30)];
				status = CONV_VA;
			}
		}
	}
	// DECIMAL
	else{
		status = CONV_D;
		while(value[i]){
			if(value[i] >= 0x30 && value[i] <= 0x39)
				val = val*10 + (value[i]-0x30);
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


#if CMD_SET == 1

//TODO: mask out usart pins???
static int8_t cmd_set(void){
	uint8_t reg = 0, offset = 4;
	uint16_t val = 0;

	if((strLen(val_ptr[1]) > 1) && (strLen(val_ptr[2]) > 0)){
		if(parseValue(val_ptr[2],&val) > ERROR){
			switch(val_ptr[1][0]){
				case 'D': reg = DDR_REG; break;
				case 'P': reg = PORT_REG; break;
			}
			offset = 3 - (val_ptr[1][1] - 65);
			if((reg > 0) && (offset < 4)){
				if(strLen(val_ptr[1]) == 3){
					if((val_ptr[1][2] >= 0x30) && (val_ptr[1][2] <= 0x37)){
						if(val)
						(_SFR_IO8(reg + (offset * 3))) |= (1 << (val_ptr[1][2] - 0x30));
						else
						(_SFR_IO8(reg + (offset * 3))) &= ~(1 << (val_ptr[1][2] - 0x30));
						usart_write("SET | %s = %i"CRLF,val_ptr[1],val);
						return 1;
					}
				}
				else{
					_SFR_IO8(reg + (offset  *3)) = val;
					usart_write("SET | %s = %i"CRLF,val_ptr[1],val);
					return 1;
				}
			}
			else if(strLen(val_ptr[1]) > 2 && val_ptr[1][0] == '$'){
				if((val_ptr[1][1] >= 0x30) && (val_ptr[1][1] <= 0x39) && (val_ptr[1][2] >= 0x30) && (val_ptr[1][2] <= 0x39)){
					cmd_vars[((val_ptr[1][1] - 0x30) * 10) + (val_ptr[1][2] - 0x30)] = val;
					usart_write("SET | %s = %i"CRLF,val_ptr[1],val);
					return 1;
				}
			}
		}
	}
	usart_write("ERR | %s = %s"CRLF,val_ptr[1],val_ptr[2]);
	return ERROR;
}
#endif


#if CMD_PRINT == 1

static int8_t cmd_print(void){
	if((val_ptr[1][0] == '$') && (strLen(val_ptr[1])>2) && (strLen(val_ptr[2])>0)){
		if((val_ptr[1][1] >= 0x30) && (val_ptr[1][1] <= 0x39) && (val_ptr[1][2] >= 0x30) && (val_ptr[1][2] <= 0x39)){
			switch(val_ptr[2][0]){
				case 'I':
					usart_write("PRINT | %s = %i"CRLF,val_ptr[1],cmd_vars[((val_ptr[1][1] - 0x30) * 10) + (val_ptr[1][2] - 0x30)]);
					return 1;
					break;
				case 'X':
					usart_write("PRINT | %s = 0x%x"CRLF,val_ptr[1],cmd_vars[((val_ptr[1][1] - 0x30) * 10) + (val_ptr[1][2] - 0x30)]);
					return 1;
					break;
				case 'O':
					usart_write("PRINT | %s = 0o%o"CRLF,val_ptr[1],cmd_vars[((val_ptr[1][1] - 0x30) * 10) + (val_ptr[1][2] - 0x30)]);
					return 1;
					break;
				case 'B':
					usart_write("PRINT | %s = 0b%b"CRLF,val_ptr[1],cmd_vars[((val_ptr[1][1] - 0x30) * 10) + (val_ptr[1][2] - 0x30)]);
					return 1;
					break;
	//			case 'S': usart_write("DISP | %s = %s"CRLF,val_ptr[1],cmd_vars[((val_ptr[1][1] - 0x30) * 10) + (val_ptr[1][2] - 0x30)]);break;
				default:
					usart_write("PRINT | DISP: '%s' wrong base"CRLF,val_ptr[2]); //ALL YOUR BASE ARE BELONG TO US
					return ERROR;
					break;
			}
		}
	}
	usart_write("ERR |  PRINT: %s not found"CRLF,val_ptr[1]);
	return ERROR;
}
#endif


#if USE_SD == 1
#if CMD_OPEN == 1

static int8_t cmd_open(void){
	uint32_t seek;
	if(strLen(val_ptr[1]) > 0){
		if( MMC_FILE_OPENED == ffopen((uint8_t*)val_ptr[1],'r') ){
			seek = file.length;
			usart_write("OPEN | %s"CRLL,val_ptr[1]);
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
			return 1;
		}
	}
	usart_write("ERR |  OPEN %s"CRLF,val_ptr[1]);
	return ERROR;
}
#endif


#if CMD_DELAY == 1

static int8_t cmd_delay(void){
	uint16_t parsedval = 0;
	int8_t status = parseValue(val_ptr[2],&parsedval);
	if((val_ptr[1][0] == 'M') && (status > ERROR)){
		usart_write("DELAY |  %sS = %i"CRLF,val_ptr[1],parsedval);
		for(uint16_t i = 0; i < parsedval; i++)
			_delay_ms(1);
		return 1;

	}
	else if((val_ptr[1][0] == 'U') && (status > ERROR)){
		usart_write("DELAY |  %sS = %i"CRLF,val_ptr[1],parsedval);
		for(uint16_t i = 0; i < parsedval; i++)
			_delay_us(1);
		return 1;
	}
	usart_write("ERR |  %s = %i"CRLF,val_ptr[2],status);
	return ERROR;
}
#endif
#endif


