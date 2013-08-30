/*
 * 	Doku, siehe http://www.mikrocontroller.net/articles/AVR_FAT32
 *  Neuste Version: http://www.mikrocontroller.net/svnbrowser/avr-fat32/
 *	Autor: Daniel R.
 */

#include "mmc_config.h"
#include "uart.h"



//***********************Funktionen**********************************
void uputc(uint8_t c){		//zeichen senden

	loop_until_bit_is_set(UCSRA,UDRE);	// warten bis puffer frei ist
	UDR = c;								// zeichen schreiben
}	

//*******************************************************************
void uputs (uint8_t *s){		//string senden
	
  while(*s) uputc(*s++);				//sendet zeichenkette, bis ende.. '\0'    
  
}

#if (!1)

// *******************************************************************
uint8_t ugetc(void){			//zeichen holen

  while (!(UCSRA & (1<<RXC))) {;}	// warten bis Zeichen verfuegbar
        
  return UDR;  		                // Zeichen aus UDR an Aufrufer zurueckgeben
}


// *******************************************************************
void ugets(int8_t* Buffer, uint8_t MaxLen){	// zeichenkette holen
  uint8_t NextChar;
  uint8_t StringLen = 0;
 
  NextChar = ugetc();         		// Warte auf und empfange das nächste Zeichen  
                                
  while( NextChar != '\n' && StringLen < MaxLen - 1 ) {		//string ende oder puffer voll
    *Buffer++ = NextChar;
    StringLen++;
    NextChar = ugetc();
  }
   
  *Buffer = '\0';					//string abschluss
}
#endif


//*******************************************************************
void uinit (void){					//init usart Tx, 8n1 ,UDRE interupt enable

  UCSRC |= (3<<UCSZ0);    		// URSEL = 1 dann wird UCSRC benutzt sonst UBRRH ; UCSZ0=data register- 8bit; USBS stop bit 1 	
  //Baudrate (high und low byte)
  UBRRH = (uint8_t)(UBRR_VAL>>8);
  UBRRL = (uint8_t)UBRR_VAL;
  UCSRB |= (1<<TXEN)|(1<<RXEN);  	// UART TX,RX einschalten,	data register empty interrupt enable,
}

