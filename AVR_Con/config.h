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
	#define HIST_BUFFER_SIZE 5

	// number of vars that can be used by user
	#define VA_BUF 10 // 0-99

	// select Chars used for line feed
		// 0 = without carriage return 	(\n)
		// 1 = with carriage return		(\r\n)
	#define LINEFEED 1

	// select commands included to build
		// 0 = exclude
		// 1 = include
	#define CMD_SET 1
	#define CMD_PRINT 1
	#define CMD_DELAY 1 //only included when USE_SD = 1
	#define CMD_OPEN 1	//only included when USE_SD = 1

#endif /* CONFIG_H_ */
