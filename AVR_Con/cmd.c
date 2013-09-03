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
#include "string.h"
#include "cmd.h"
#include "config.h"

#if USE_SD == 1
	#include "sd/mmc_config.h"	// Hier werden alle noetigen Konfigurationen vorgenommen, unbedingt anschauen !
	#include "sd/file.h"
	#include "sd/fat.h"
	#include "sd/mmc.h"

	static int8_t cmd_open(void);
	static int8_t cmd_delay(void);
#endif

static void get_arg_from_line(uint8_t position, char* line, char* output);
static int8_t parse_value(char* value, uint16_t* out);
static int8_t cmd_set(void);
static int8_t cmd_print(void);

uint16_t public_vars[VAR_BUF] = {0};
char* arg_ptr[ARG_BUF] = {0};
char* file_arg_ptr[ARG_BUF] = {0};


COMMAND_STRUCTUR COMMAND_TABELLE[] = // Befehls-Tabelle
{
	{"set",cmd_set},
	{"print",cmd_print},
//SD Functions
#if USE_SD == 1
		{"open",cmd_open},
		{"delay",cmd_delay},
#endif
	{NULL,NULL}
};


void parse_line(char* line){

	char gr_buf[ARG_TMP_BUF];
	uint8_t i = 0;

	//get groups of instructions -> stored in val_ptr[]
	get_arg_from_line(0,line,gr_buf);
	while(strlen(gr_buf) > 0 && i < ARG_BUF){
		free(arg_ptr[i]);
		arg_ptr[i] = strcpy(malloc((strlen(gr_buf) + 1) * sizeof(char)),gr_buf);
		i++;
		get_arg_from_line(i,line,gr_buf);
	}

	//search command
	i = 0;
	while(strcmp(COMMAND_TABELLE[i].cmd,arg_ptr[0])){
		//if CMD not found
		if (COMMAND_TABELLE[++i].cmd == 0) {
			printf(ESC_RED"ERR | Unknown Command: %s"ESC_CLEAR,arg_ptr[0]);
			return;
		}
	}

	//Exec command
	COMMAND_TABELLE[i].fp();

	//free instructions memory and set to '\0'
	for(i = 0; i < ARG_BUF; i++){
		free(arg_ptr[i]);
		arg_ptr[i]=strcpy(malloc(sizeof(char)), "");
	}
}


static void get_arg_from_line(uint8_t position, char* line, char* output){
	uint8_t spaces = 0, i = 0, strflag = 0;
	while(line[i] != '\0' && position >= spaces){

		if(line[i] == ' ' && !strflag){
			spaces++;
			while(line[++i] == ' ');
			if(line[i] == '"'){
				i++;
				strflag = 1;
			}

		}

		if(position == spaces){
			if(line[i] == '"' && strflag){
				if(line[i-1] == '\\')
					*(output-1) = '"';
			}
			else
				*output++ = line[i];
		}

		i++;

	}
	*output = '\0';
}


static int8_t parse_value(char* value, uint16_t* out){

	uint16_t val = 0;
	int8_t status = ERROR;
	char* ptr;

	if((value[0] == '0') && ((value[1] == 'x') || (value[1] == 'b') || (value[1] == 'o'))){	//maybe hex, binary or octal
// HEX
		if(value[1] == 'x'){	// hex
			val = strtoul(value,&ptr,16);
			if(*ptr == '\0')
				status = CONV_H;
		}
// BINARY
		else if(value[1] == 'b'){
			val = strtoul(value + 2,&ptr,2);
			if(*ptr == '\0')
				status = CONV_B;
		}
// OCTAL
		else if(value[1] == 'o'){
			val = strtoul(value + 2,&ptr,8);
			if(*ptr == '\0')
				status = CONV_O;
		}
	}
// ON / OUT
	else if(!strcmp(value,"on") || !strcmp(value,"out")){
		val = 1;
		status = CONV_SW;
	}
//OFF / IN
	else if(!strcmp(value,"off") || !strcmp(value,"in")){
		val = 0;
		status = CONV_SW;
	}
//VAR or REG
	else if(value[0] == 'p' || value[0] == 'd' || value[0] == 'o' || value[0] == '$' || value[0] == '@'){
		uint8_t reg = 0, offset = 4;
		switch(value[0]){
			case 'p': reg = PIN_REG; break;
			case 'd': reg = DDR_REG; break;
			case 'o': reg = PORT_REG; break;
		}
		offset = 3 - (value[1] - 65);
		if((reg > 0) && (offset < 4)){
			if(strlen(value) == 3){
				val = strtoul(value + 2,&ptr,10);
				if(*ptr == '\0'){
					val = ((_SFR_IO8(reg + (offset * 3))) >> val) & 0x01;
					status = CONV_REG;
				}
			}
			else{
				val = _SFR_IO8(reg + (offset * 3));
				status = CONV_REG;
			}
		}
		else if(strlen(value) > 1 && value[0] == '$'){
			val = strtoul(value + 1,&ptr,10);
			if(*ptr == '\0' && val < VAR_BUF){
				val = public_vars[val];
				status = CONV_VA;
			}
		}
		else if(strlen(value) > 1 && value[0] == '@'){
			val = (uint16_t)strtoul(value + 1,&ptr,10);
			if(*ptr == '\0' && (val + 2) < ARG_BUF){
				if(parse_value(file_arg_ptr[val + 2],&val) > ERROR)
					status = CONV_ARG;
			}
		}
	}
// DECIMAL
	else{
		val = strtoul(value,&ptr,10);
		if(*ptr == '\0')
			status = CONV_D;
	}
	*out = val;
	return status;
}


//TODO: mask out usart pins???
static int8_t cmd_set(void){
	int8_t reg = 0, offset = 4, ret = ERROR;
	uint16_t val = 0, tmp;
	char* ptr;
	strlwr(arg_ptr[1]);
	strlwr(arg_ptr[2]);
	if((strlen(arg_ptr[1]) > 1) && (strlen(arg_ptr[2]) > 0)){
		if(parse_value(arg_ptr[2],&val) > ERROR){
			switch(arg_ptr[1][0]){
				case 'd': reg = DDR_REG; break;
				case 'p': reg = PORT_REG; break;
			}
			offset = 3 - (arg_ptr[1][1] - 97);

			printf(ESC_YELLOW);

			if((reg > 0) && (offset < 4)){
				if(strlen(arg_ptr[1]) == 3){
					tmp = strtoul(arg_ptr[1] + 2,&ptr,10);
					if(*ptr == '\0'){
						if(val)
						(_SFR_IO8(reg + (offset * 3))) |= (1 << tmp);
						else
						(_SFR_IO8(reg + (offset * 3))) &= ~(1 << tmp);

						printf("SET | %s = %u",arg_ptr[1],val);
						ret = 1;
					}
				}
				else{
					_SFR_IO8(reg + (offset  *3)) = val;
					printf("SET | %s = %u",arg_ptr[1],val);
					ret = 1;
				}
			}
			else if(strlen(arg_ptr[1]) > 1 && arg_ptr[1][0] == '$'){
				tmp = strtoul(arg_ptr[1] + 1,&ptr,10);
				if(*ptr == '\0' && tmp < VAR_BUF){
					public_vars[tmp]=val;
					printf("SET | %s = %u",arg_ptr[1],val);
					ret = 1;
				}
			}
		}
	}
	if(!(ret > ERROR))
		printf(ESC_RED"ERR | %s = %s",arg_ptr[1],arg_ptr[2]);
	printf(ESC_CLEAR);
	return ERROR;
}

static int8_t cmd_print(void){
	int8_t ret= ERROR;
	uint16_t tmp;
	if(((arg_ptr[1][0] == '@') || (arg_ptr[1][0] == '$')) && (strlen(arg_ptr[1])>1) && (strlen(arg_ptr[2])==1) && (parse_value(arg_ptr[1],&tmp) > ERROR)){
		strlwr(arg_ptr[2]);
		printf(ESC_YELLOW);
		switch(arg_ptr[2][0]){
			case 'i':
				printf("PRINT | %s = %u",arg_ptr[1],tmp);
				ret = 1;
				break;
			case 'x':
				printf("PRINT | %s = 0x%x",arg_ptr[1],tmp);
				ret = 1;
				break;
			case 'o':
				printf("PRINT | %s = 0o%o",arg_ptr[1],tmp);
				ret = 1;
				break;
			case 'b':{
				char illuminati_out[23];
				uint_to_bin(illuminati_out,tmp);
				printf("PRINT | %s = %s",arg_ptr[1],illuminati_out);
				ret = 1;
				break;
			}
//			case 'S': printf("DISP | %s = %s"CRLF,arg_ptr[1],tmp;break;
			default:
				 //ALL YOUR BASE ARE BELONG TO US
				printf(ESC_RED"PRINT | PRINT: '%s' wrong base",arg_ptr[2]);
				ret = ERROR;
				break;
		}

	}
	else if((arg_ptr[1][0] == '@') && (strlen(arg_ptr[1]) >1) && (parse_value((arg_ptr[1] + 1),&tmp) > ERROR)){
			printf(ESC_YELLOW"%s"ESC_YELLOW""CRLF,file_arg_ptr[tmp+2]);
			ret=1;
	}
	else if(strlen(arg_ptr[1]) > 0){
		printf(ESC_YELLOW"%s"ESC_YELLOW""CRLF,arg_ptr[1]);
		ret=1;
	}
	if(!(ret > ERROR))
		printf(ESC_RED"ERR |  PRINT: no value");
	printf(ESC_CLEAR);
	return ret;
}


//SD functions
#if USE_SD == 1

static int8_t cmd_open(void){
	uint32_t seek;
	uint8_t i;
	if(strlen(arg_ptr[1]) > 0){
		if( MMC_FILE_OPENED == ffopen((uint8_t*)arg_ptr[1],'r') ){
			seek = file.length;
			printf(ESC_YELLOW"OPEN | %s ... ",arg_ptr[1]);
			for(i = 0; i < ARG_BUF; i++)
				file_arg_ptr[i] = strcpy(malloc((strlen(arg_ptr[i])+1)*sizeof(char)),arg_ptr[i]);
			printf(ESC_GREEN"OK"ESC_CLEAR""CRLL);
			char line_buf[BUFFER_SIZE] = {0};
			uint8_t cnt = 0;
			do{
				do{
					line_buf[cnt++] = ffread();
						if(line_buf[cnt-1] == '\r'){
							line_buf[cnt-1] = '\0';
							printf(ESC_CYAN"  > %s"CRLF"    ",line_buf);
							parse_line(line_buf);
							printf(CRLF);
							cnt = 0;
							line_buf[cnt] = '\0';
							ffread();
							seek--;
						}
				}while(--seek && (cnt < BUFFER_SIZE));
				cnt = 0;
				line_buf[cnt] = '\0';
			}while(seek);
			printf(CRLF""ESC_YELLOW"CLOSE | %s ... ",file_arg_ptr[1]);
			ffclose();
			file_args_init();
			printf(ESC_GREEN"OK"ESC_CLEAR""CRLF);
			return 1;
		}
	}
	printf(ESC_RED"ERR |  OPEN %s"ESC_CLEAR,arg_ptr[1]);
	return ERROR;
}


static int8_t cmd_delay(void){
	uint16_t parsedval = 0;
	int8_t status = parse_value(arg_ptr[2],&parsedval);
	if((arg_ptr[1][0] == 'm') && (status > ERROR)){
		printf(ESC_YELLOW"DELAY |  %sS = %u"ESC_CLEAR,arg_ptr[1],parsedval);
		for(uint16_t i = 0; i < parsedval; i++)
			_delay_ms(1);
		return 1;

	}
	else if((arg_ptr[1][0] == 'u') && (status > ERROR)){
		printf(ESC_YELLOW"DELAY |  %sS = %u"ESC_CLEAR,arg_ptr[1],parsedval);
		for(uint16_t i = 0; i < parsedval; i++)
			_delay_us(1);
		return 1;
	}
	printf(ESC_RED"ERR |  %s = %i"ESC_CLEAR,arg_ptr[2],status);
	return ERROR;
}
#endif


void file_args_init(void){
	for(uint16_t i = 0; i < ARG_BUF; i++){
		free(file_arg_ptr[i]);
		file_arg_ptr[i]=strcpy(malloc(sizeof(char) ), "");
	}
}

void uint_to_bin(char* out, uint16_t value){
	uint8_t i = 0;
	char* h = out;
	uint8_t m = 12;
	if(value > 0xF)
		m = 8;
	if(value > 0xFF)
		m = 4;
	if(value > 0xFFF)
		m = 0;
	*h++ = '0';
	*h++ = 'b';
	do{
		if(i>0 && !(i%4))
			*h++ = '.';

		uint8_t t = ((value << ((i++*1)+m)) & 0x8000) >> 15;
		*h++ = t + 0x30;
	}while(i<(16-m));
	*h = '\0';
}
