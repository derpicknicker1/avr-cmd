/*
 * cmd.h
 *
 *  Created on: 28.08.2013
 *      Author: Grisu
 */

#ifndef CMD_H_
	#define CMD_H_

//	#define ADD(x,y)		(x+=y)
//	#define SUB(x,y)		(x-=y)
//	#define MUL(x,y)		(x*=y)
//	#define MOD(x,y)		(x%=y)
//	#define DIV(x,y)		(x/=y)

	#define PIN_REG ((volatile uint16_t )&PIND-0x20)
	#define DDR_REG ((volatile uint16_t )&DDRD-0x20)
	#define PORT_REG ((volatile uint16_t )&PORTD-0x20)

enum{
	ERROR 	= -1,	//	-1 = error
	CONV_D 	=  0,	//	 0 = decimal
	CONV_H 	=  1,	//	 1 = hex
	CONV_B 	=  2,	//	 2 = binary
	CONV_O 	=  3,	// 	 3 = octal
	CONV_SW =  4,	//	 4 = on/off/in/out
	CONV_REG=  5,	//	 5 = Register
	CONV_VA =  6	//	 6 = Var
};

	typedef struct
	{
		char* cmd; 			//CMD Name
		int8_t(*fp)(void);  	// function pointer
	} COMMAND_STRUCTUR;

	void parse_line(char* line);

#endif /* CMD_H_ */
