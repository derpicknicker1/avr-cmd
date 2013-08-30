/*
 * cmd.h
 *
 *  Created on: 28.08.2013
 *      Author: Grisu
 */

#ifndef CMD_H_
	#define CMD_H_

	#include <stdio.h>

	#define USE_SD 1 //also set in main.c

	#define BUF_C 		16
	#define BUF_P 		16
	#define BUF_V 		16

	#define ERROR 		-1	//	-1 = error
	#define CONV_D 		0	//	 0 = decimal
	#define CONV_H 		1	//	 1 = hex
	#define CONV_B 		2	//	 2 = binary
	#define CONV_O 		3	// 	 3 = octal
	#define CONV_SW 	4	//	 4 = on/off
	#define CMD_SET 	"set"
	#define CMD_CFG 	"cfg"
	#define CMD_GET 	"get"
	#define CMD_OPEN 	"open"

	void get_group_from_line(uint8_t position, char* line, char* output);
	int16_t HexWordToValue(char word);
	int8_t AsciiToValue(char word);
	void toUpper(char* in);
	void toLower(char* in);
	int8_t parseValue(char* value, uint16_t* out);
	uint8_t stringLength(char* a);
	int8_t stringCompare(char* a, char* b);
	void parseLine(char* line);
	int8_t executeSet(char* par, uint16_t val);
	int16_t executeGet(char* par, char* val);
	int8_t executeCfg(char* par, uint16_t val);

#endif /* CMD_H_ */
