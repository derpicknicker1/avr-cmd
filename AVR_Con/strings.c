/*
 * strings.c
 *
 *  Created on: 02.09.2013
 *      Author: Grisu
 */
#include <stdio.h>
#include "strings.h"

int8_t HexWordToValue(char word){
	if(word >= 0x30 && word <= 0x39){	// zahl
		return (word-0x30);
	}
	else if(word >= 0x41 && word <=0x46){ // grosser buchstabe
		return (word-0x37);
	}
	else if(word >= 0x61 && word <= 0x66){ // kleiner buchstabe
		return (word-0x57);
	}
	return -1;
}


uint8_t AsciiToValue(char word){
	if(word >= 0x30&& word <= 0x39){	// zahl
		return (word-0x30);
	}
	return -1;
}

void toUpper(char* in){
	do{
		if(*in >=0x61 && *in <=0x7a)
			*in -= 32;
	}while(*in++);
}

void toLower(char* in){
	do{
		if(*in >=0x41 && *in <=0x5a)
			*in += 32;
	}while(*in++);
}

uint16_t strLen(char* a){
	uint8_t i = 0;
	while(a[i] != '\0'){
		i++;
	}
	return i;
}

uint16_t strCmp(char* a, char* b){
	uint8_t i = 0;
	if(strLen(b) > strLen(a))
		return -1;
	while(a[i] != '\0'){
		if(a[i] != b[i]){
			return i+1;
		}
		i++;
	}
	return 0;
}

char *strCpy(char *destination, char *source){
    char *p = destination;
    while (*source != '\0')
        *p++ = *source++;
    *p = '\0';
    return destination;
}


