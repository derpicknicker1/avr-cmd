/*
 * config.h
 *
 *  Created on: 01.09.2013
 *      Author: Grisu
 */

#ifndef CONFIG_H_
	#define CONFIG_H_

	// include SD-Card functionality
		// CMD_OPEN and CMD_DELAY are excluded also
		// they are only needed when working with SD_Card
		// see below at CMD select section
	#define USE_SD 0

	// USART RX Buffer (Line-Buffer) Size in Bytes (=Chars)
	#define BUFFER_SIZE	100 //max. 255?

	// number of lines stored in command history
	#define HIST_BUFFER_SIZE 10

	// number of vars that can be used by user
	#define VAR_BUF 10 // 0-99

	// number of parameters taht can be extracted from one line
	#define ARG_BUF 20 // 0-99

	// length one parameter can have
	#define ARG_TMP_BUF 16

	// select Chars used for line feed
		// 0 = without carriage return 	(\n)
		// 1 = with carriage return		(\r\n)
	#define LINEFEED 1

	// Baud rate select
	#define BAUD 9600
#endif /* CONFIG_H_ */
