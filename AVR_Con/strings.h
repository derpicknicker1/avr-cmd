/*
 * string.h
 *
 *  Created on: 02.09.2013
 *      Author: Grisu
 */

#ifndef STRINGS_H_
	#define STRINGS_H_

	int8_t HexWordToValue(char word);
	uint8_t AsciiToValue(char word);
	void toUpper(char* in);
	void toLower(char* in);
	uint16_t stringLength(char* a);
	uint16_t stringCompare(char* a, char* b);
	char *my_strcpy(char *destination, char *source);

#endif /* STRING_H_ */
