/*
 * cmd.h
 *
 *  Created on: 28.08.2013
 *      Author: Grisu
 */

#ifndef CMD_H_
	#define CMD_H_

	#define ADD(x,y)		(x+=y)
	#define SUB(x,y)		(x-=y)
	#define MUL(x,y)		(x*=y)
	#define MOD(x,y)		(x%=y)
	#define DIV(x,y)		(x/=y)

	#define PIN_REG (0x00)
	#define DDR_REG (0x01)
	#define PORT_REG (0x02)

enum{
	ERROR 	= -1,	//	-1 = error
	CONV_D 	=  0,	//	 0 = decimal
	CONV_H 	=  1,	//	 1 = hex
	CONV_B 	=  2,	//	 2 = binary
	CONV_O 	=  3,	// 	 3 = octal
	CONV_SW =  4,	//	 4 = on/off/in/out
	CONV_REG=  5,	//	 5 = Register
	CONV_VA =  6,	//	 6 = Var
	CONV_ARG=  7	// 	 7 = File-Arg
};

	typedef struct
	{
		char* cmd; 			//CMD Name
		int8_t(*fp)(void);  	// function pointer
	} COMMAND_STRUCTUR;

	void parse_line(char* line);

	#if USE_SD ==1
		void file_args_init(void);
	#endif

#endif /* CMD_H_ */
