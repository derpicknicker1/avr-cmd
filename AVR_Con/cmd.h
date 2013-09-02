/*
 * cmd.h
 *
 *  Created on: 28.08.2013
 *      Author: Grisu
 */

#ifndef CMD_H_
	#define CMD_H_

	#define BUF_C 			16
	#define BUF_P 			16
	#define BUF_V 			16

//	#define ADD(x,y)		(x+=y)
//	#define SUB(x,y)		(x-=y)
//	#define MUL(x,y)		(x*=y)
//	#define MOD(x,y)		(x%=y)
//	#define DIV(x,y)		(x/=y)

	#define PIN_REG ((volatile uint16_t )&PIND-0x20)
	#define DDR_REG ((volatile uint16_t )&DDRD-0x20)
	#define PORT_REG ((volatile uint16_t )&PORTD-0x20)

	#define ERROR 			-1	//	-1 = error
	#define CONV_D 			0	//	 0 = decimal
	#define CONV_H 			1	//	 1 = hex
	#define CONV_B 			2	//	 2 = binary
	#define CONV_O 			3	// 	 3 = octal
	#define CONV_SW 		4	//	 4 = on/off/in/out
	#define CONV_REG 		5	//	 5 = Register
	#define CONV_VA 		6	//	 6 = Var
	#define CMD_SET_STR		"set"
	#define CMD_GET_STR 	"get"
	#define CMD_OPEN_STR	"open"
	#define CMD_DELAY_STR 	"delay"
	#define CMD_PRINT_STR 	"print"
	#define CMD_ADD		 	"add"
	#define CMD_SUB		 	"sub"
	#define CMD_MUL		 	"mul"
	#define CMD_MOD		 	"mod"
	#define CMD_DIV		 	"div"


	void get_group_from_line(uint8_t position, char* line, char* output);
	int8_t HexWordToValue(char word);
	uint8_t AsciiToValue(char word);
	void toUpper(char* in);
	void toLower(char* in);
	int8_t parseValue(char* value, uint16_t* out);
	uint16_t stringLength(char* a);
	uint16_t stringCompare(char* a, char* b);
	void parseLine(char* line);
	int8_t executeSet(char* par, char* val);
	char *my_strcpy(char *destination, char *source);

#endif /* CMD_H_ */
