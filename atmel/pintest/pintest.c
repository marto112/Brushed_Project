/* Brushed project
Main code file. 
 */

//Set your controller here
#include "../pindefs/F20a.h"


#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>
#include <avr/eeprom.h>

inline void setALow();
inline void clrALow();
inline void setAHigh();
inline void clrAHigh();
inline void setBLow();
inline void clrBLow();
inline void setBHigh();
inline void clrBHigh();
inline void setCLow();
inline void clrCLow();
inline void setCHigh();
inline void clrCHigh();

#define A 0
#define B 0
#define C 1

#define testHigh 	1
#define testLow		1

int main() {

    //Setup Outputs
    _delay_ms(10);

	if(A){
		//Setup A
		clrALow();
		clrAHigh();

    	SET_HIGH_A_PORT |= (1 << HIGH_A);
  	  	SET_LOW_A_PORT |= (1 << LOW_A);
	}

	if(B){
		//Setup A
		clrBLow();
		clrBHigh();

    	SET_HIGH_B_PORT |= (1 << HIGH_B);
  	  	SET_LOW_B_PORT |= (1 << LOW_B);
	}

	if(C){
		//Setup A
		clrCLow();
		clrCHigh();

    	SET_HIGH_C_PORT |= (1 << HIGH_C);
  	  	SET_LOW_C_PORT |= (1 << LOW_C);
	}

    //Main loop
    while (1) {

		if (testHigh){
			if (A) {
				setAHigh();
				_delay_ms(500);
				clrAHigh();
				_delay_ms(500);	
			} 	
				
			if (B) {
				setBHigh();
				_delay_ms(500);
				clrBHigh();
				_delay_ms(500);	
			} 
					
			if (C) {
				setCHigh();
				_delay_ms(500);
				clrCHigh();
				_delay_ms(500);	
			} 
		}
		if (testLow){
			if (A) {
				setALow();
				_delay_ms(500);
				clrALow();
				_delay_ms(500);	
			} 	
				
			if (B) {
				setBLow();
				_delay_ms(500);
				clrBLow();
				_delay_ms(500);	
			} 
					
			if (C) {
				setCLow();
				_delay_ms(500);
				clrCLow();
				_delay_ms(500);	
			} 
		}							
    }
}

//Motor Pin functions
inline void setALow(){
    if (LOW_ACTIVE_LOW){
        LOW_A_PORT &= ~(1 << LOW_A); 
    }
    else {
        LOW_A_PORT |= (1 << LOW_A);     
    }
}

inline void clrALow(){
    if (LOW_ACTIVE_LOW){
        LOW_A_PORT |= (1 << LOW_A); 
    }
    else {
        LOW_A_PORT &= ~(1 << LOW_A);
    }
}

inline void setAHigh(){
    if (HIGH_ACTIVE_LOW){
        HIGH_A_PORT &= ~(1 << HIGH_A);
    }
    else {
        HIGH_A_PORT |= (1 << HIGH_A);
    }
}



inline void clrAHigh(){
    if (HIGH_ACTIVE_LOW){
        HIGH_A_PORT |= (1 << HIGH_A);
    }
    else {
        HIGH_A_PORT &= ~(1 << HIGH_A);
    }
}

inline void setBLow(){
    if (LOW_ACTIVE_LOW){
        LOW_B_PORT &= ~(1 << LOW_B); 
    }
    else {
        LOW_B_PORT |= (1 << LOW_B);     
    }
}

inline void clrBLow(){
    if (LOW_ACTIVE_LOW){
        LOW_B_PORT |= (1 << LOW_B); 
    }
    else {
        LOW_B_PORT &= ~(1 << LOW_B);
    }
}

inline void setBHigh(){
    if (HIGH_ACTIVE_LOW){
        HIGH_B_PORT &= ~(1 << HIGH_B);
    }
    else {
        HIGH_B_PORT |= (1 << HIGH_B);
    }
}

inline void clrBHigh(){
    if (HIGH_ACTIVE_LOW){
        HIGH_B_PORT |= (1 << HIGH_B);
    }
    else {
        HIGH_B_PORT &= ~(1 << HIGH_B);
    }
}

inline void setCLow(){
    if (LOW_ACTIVE_LOW){
        LOW_C_PORT &= ~(1 << LOW_C); 
    }
    else {
        LOW_C_PORT |= (1 << LOW_C);     
    }
}

inline void clrCLow(){
    if (LOW_ACTIVE_LOW){
        LOW_C_PORT |= (1 << LOW_C); 
    }
    else {
        LOW_C_PORT &= ~(1 << LOW_C);
    }
}

inline void setCHigh(){
    if (HIGH_ACTIVE_LOW){
        HIGH_C_PORT &= ~(1 << HIGH_C);
    }
    else {
        HIGH_C_PORT |= (1 << HIGH_C);
    }
}

inline void clrCHigh(){
    if (HIGH_ACTIVE_LOW){
        HIGH_C_PORT |= (1 << HIGH_C);
    }
    else {
        HIGH_C_PORT &= ~(1 << HIGH_C);
    }
}
