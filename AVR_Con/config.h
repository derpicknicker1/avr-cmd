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
	#define USE_SD 1

	// USART RX Buffer Size in Bytes (=Characters)
    #define BUFFER_SIZE	100

	// select Chars used for line feed
		// 0 = without carriage return 	(\n)
		// 1 = with carriage return		(\r\n)
	#define LINEFEED 1

	// select commands included to build
		// 0 = exclude
		// 1 = include
	#define CMD_SET 1
	#define CMD_GET 1
	#define CMD_DELAY 1
	#define CMD_OPEN 1

#endif /* CONFIG_H_ */
