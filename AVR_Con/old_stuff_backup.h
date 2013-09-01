/*
 * old_stuff_backup.h
 *
 *  Created on: 01.09.2013
 *      Author: Grisu
 */

#ifndef OLD_STUFF_BACKUP_H_
#define OLD_STUFF_BACKUP_H_
//int8_t executeSet(char* par, uint16_t val){
//
//﻿  if((par[0] == 'P') && (stringLength(par) == 2)){
//﻿  ﻿  switch(par[1]){
//﻿  ﻿  case 'A': PORTA = val; return 1; break;
//﻿  ﻿  case 'B': PORTB = val; return 1; break;
//﻿  ﻿  case 'C': PORTC = val; return 1; break;
//﻿  ﻿  case 'D': ﻿  val &= 0xFC; // protect USART
//﻿  ﻿  ﻿  ﻿  ﻿  PORTD = val; return 1; break;
//﻿  ﻿  }
//﻿  }
//﻿  else if((par[0] == 'P') && (stringLength(par) == 3)){
//﻿  ﻿  if((par[2] >= 0x30) && (par[2] <= 0x37)){
//﻿  ﻿  ﻿  switch(par[1]){
//﻿  ﻿  ﻿  ﻿  case 'A': (val)?(PORTA |= (1 << (par[2] - 0x30))):(PORTA &= ~(1 << (par[2] - 0x30))); return 1; break;
//﻿  ﻿  ﻿  ﻿  case 'B': (val)?(PORTB |= (1 << (par[2] - 0x30))):(PORTB &= ~(1 << (par[2] - 0x30))); return 1; break;
//﻿  ﻿  ﻿  ﻿  case 'C': (val)?(PORTC |= (1 << (par[2] - 0x30))):(PORTC &= ~(1 << (par[2] - 0x30))); return 1; break;
//﻿  ﻿  ﻿  ﻿  case 'D':
//﻿  ﻿  ﻿  ﻿  ﻿  if((par[2] - 0x30) > 1){ //protect USART
//﻿  ﻿  ﻿  ﻿  ﻿  ﻿  (val)?(PORTD |= (1 << (par[2] - 0x30))):(PORTD &= ~(1 << (par[2] - 0x30)));
//﻿  ﻿  ﻿  ﻿  ﻿  ﻿  return 1;
//﻿  ﻿  ﻿  ﻿  ﻿  }
//﻿  ﻿  ﻿  ﻿  ﻿  break;
//﻿  ﻿  ﻿  }
//﻿  ﻿  }
//﻿  }
//﻿  else if((par[0] == 'D') && (stringLength(par) == 2)){
//﻿  ﻿  switch(par[1]){
//﻿  ﻿  case 'A': DDRA = val; return 1; break;
//﻿  ﻿  case 'B': DDRB = val; return 1; break;
//﻿  ﻿  case 'C': DDRC = val; return 1; break;
//﻿  ﻿  case 'D': ﻿  val &= 0xFC; // protect USART
//﻿  ﻿  ﻿  ﻿  ﻿  DDRD = val; return 1; break;
//﻿  ﻿  }
//﻿  }
//﻿  else if((par[0] == 'D') && (stringLength(par) == 3)){
//﻿  ﻿  if((par[2] >= 0x30) && (par[2] <= 0x37)){
//﻿  ﻿  ﻿  switch(par[1]){
//﻿  ﻿  ﻿  ﻿  case 'A': (val)?(DDRA |= (1 << (par[2] - 0x30))):(DDRA &= ~(1 << (par[2] - 0x30))); return 1; break;
//﻿  ﻿  ﻿  ﻿  case 'B': (val)?(DDRB |= (1 << (par[2] - 0x30))):(DDRB &= ~(1 << (par[2] - 0x30))); return 1; break;
//﻿  ﻿  ﻿  ﻿  case 'C': (val)?(DDRC |= (1 << (par[2] - 0x30))):(DDRC &= ~(1 << (par[2] - 0x30))); return 1; break;
//﻿  ﻿  ﻿  ﻿  case 'D':
//﻿  ﻿  ﻿  ﻿  ﻿  if((par[2] - 0x30) > 1){ //protect USART
//﻿  ﻿  ﻿  ﻿  ﻿  ﻿  (val)?(DDRD |= (1 << (par[2]))):(DDRD &= ~(1 << (par[2])));
//﻿  ﻿  ﻿  ﻿  ﻿  ﻿  return 1;
//﻿  ﻿  ﻿  ﻿  ﻿  }
//﻿  ﻿  ﻿  ﻿  ﻿  break;
//﻿  ﻿  ﻿  }
//﻿  ﻿  }
//﻿  }
//﻿  return ERROR;
//}
//
//int16_t executeGet(char* par){
//
//﻿  if((par[0] == 'D')){
//﻿  ﻿  if(stringLength(par) == 2){
//﻿  ﻿  ﻿  switch(par[1]){
//﻿  ﻿  ﻿  case 'A': return DDRA; break;
//﻿  ﻿  ﻿  case 'B': return DDRB; break;
//﻿  ﻿  ﻿  case 'C': return DDRC; break;
//﻿  ﻿  ﻿  case 'D': return (DDRD & 0xFC); break;
//﻿  ﻿  ﻿  }
//﻿  ﻿  }
//﻿  ﻿  else if(stringLength(par) == 3){
//﻿  ﻿  ﻿  if((par[2] >= 0x30) && (par[2] <= 0x37)){
//﻿  ﻿  ﻿  ﻿  switch(par[1]){
//﻿  ﻿  ﻿  ﻿  ﻿  case 'A': return ((DDRA >> (par[2] - 0x30)) & 0x01); break;
//﻿  ﻿  ﻿  ﻿  ﻿  case 'B': return ((DDRB >> (par[2] - 0x30)) & 0x01); break;
//﻿  ﻿  ﻿  ﻿  ﻿  case 'C': return ((DDRB >> (par[2] - 0x30)) & 0x01); break;
//﻿  ﻿  ﻿  ﻿  ﻿  case 'D':
//﻿  ﻿  ﻿  ﻿  ﻿  ﻿  if((par[2] - 0x30) > 1) //protect USART
//﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  return ((DDRD >> (par[2] - 0x30)) & 0x01);
//﻿  ﻿  ﻿  ﻿  ﻿  ﻿  break;
//﻿  ﻿  ﻿  ﻿  }
//﻿  ﻿  ﻿  }
//﻿  ﻿  }
//﻿  }
//﻿  else if(par[0] == 'P'){
//﻿  ﻿  if(stringLength(par) == 2){
//﻿  ﻿  ﻿  switch(par[1]){
//﻿  ﻿  ﻿  case 'A': return PINA; break;
//﻿  ﻿  ﻿  case 'B': return PINB; break;
//﻿  ﻿  ﻿  case 'C': return PINC; break;
//﻿  ﻿  ﻿  case 'D': return (PIND & 0xFC); break;
//﻿  ﻿  ﻿  }
//﻿  ﻿  }
//﻿  ﻿  else if(stringLength(par) == 3){
//﻿  ﻿  ﻿  if((par[2] >= 0x30) && (par[2] <= 0x37)){
//﻿  ﻿  ﻿  ﻿  switch(par[1]){
//﻿  ﻿  ﻿  ﻿  ﻿  case 'A': return ((PINA >> (par[2] - 0x30)) & 0x01); break;
//﻿  ﻿  ﻿  ﻿  ﻿  case 'B': return ((PINB >> (par[2] - 0x30)) & 0x01); break;
//﻿  ﻿  ﻿  ﻿  ﻿  case 'C': return ((PINB >> (par[2] - 0x30)) & 0x01); break;
//﻿  ﻿  ﻿  ﻿  ﻿  case 'D':
//﻿  ﻿  ﻿  ﻿  ﻿  ﻿  if((par[2] - 0x30) > 1) //protect USART
//﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  return ((PIND >> (par[2] - 0x30)) & 0x01);
//﻿  ﻿  ﻿  ﻿  ﻿  ﻿  break;
//﻿  ﻿  ﻿  ﻿  }
//﻿  ﻿  ﻿  }
//﻿  ﻿  }
//﻿  }
//﻿  else if(par[0] == 'O'){
//﻿  ﻿  if(stringLength(par) == 2){
//﻿  ﻿  ﻿  switch(par[1]){
//﻿  ﻿  ﻿  case 'A': return PORTA; break;
//﻿  ﻿  ﻿  case 'B': return PORTB; break;
//﻿  ﻿  ﻿  case 'C': return PORTC; break;
//﻿  ﻿  ﻿  case 'D': return (PORTD & 0xFC); break;
//﻿  ﻿  ﻿  }
//﻿  ﻿  }
//﻿  ﻿  else if(stringLength(par) == 3){
//﻿  ﻿  ﻿  if((par[2] >= 0x30) && (par[2] <= 0x37)){
//﻿  ﻿  ﻿  ﻿  switch(par[1]){
//﻿  ﻿  ﻿  ﻿  ﻿  case 'A': return ((PORTA >> (par[2] - 0x30)) & 0x01); break;
//﻿  ﻿  ﻿  ﻿  ﻿  case 'B': return ((PORTB >> (par[2] - 0x30)) & 0x01); break;
//﻿  ﻿  ﻿  ﻿  ﻿  case 'C': return ((PORTB >> (par[2] - 0x30)) & 0x01); break;
//﻿  ﻿  ﻿  ﻿  ﻿  case 'D':
//﻿  ﻿  ﻿  ﻿  ﻿  ﻿  if((par[2] - 0x30) > 1) //protect USART
//﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  return ((PORTD >> (par[2]-0x30)) & 0x01);
//﻿  ﻿  ﻿  ﻿  ﻿  ﻿  break;
//﻿  ﻿  ﻿  ﻿  }
//﻿  ﻿  ﻿  }
//﻿  ﻿  }
//﻿  }
//﻿  return ERROR;
//}
#endif /* OLD_STUFF_BACKUP_H_ */
