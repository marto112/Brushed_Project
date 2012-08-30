/* Brushed project
Main code file. 
 */

//Set your controller here
#include "../pindefs/tpy1-25.h"


#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>
#include <avr/eeprom.h>

//Define RC States
#define unitialised  0
#define forward  1
#define backward  2
#define brake  3

//Define Temp States
#define HOT  2
#define WARM 1
#define COOL 0

volatile uint8_t state = 0;
volatile uint16_t prevTime = 0;
volatile uint16_t rcTime = 0;
volatile uint8_t time0 = 0;
volatile uint8_t failsafe = 0;
volatile uint8_t timeout = 50;
volatile uint8_t newState = 0;
volatile uint8_t lets_get_high = 0;
volatile uint8_t tempState = 0;
volatile uint8_t rcTick = 0;
volatile uint8_t desiredPWM = 0;

//Stored data variables
volatile uint16_t rcLow = RC_LOW;
volatile uint16_t rcHigh = RC_HIGH;
volatile uint16_t rcMid = (RC_LOW + RC_HIGH) / 2;
volatile uint8_t maxSlew = MAX_SLEW;
volatile uint8_t tempLimit = TEMP_LIMIT;
volatile uint8_t goExpo = EXPO;

//Define memory states
uint16_t EEMEM storedRcLow = RC_LOW;
uint16_t EEMEM storedRcHigh = RC_HIGH;
uint8_t EEMEM storedSlewRate = MAX_SLEW;
uint8_t EEMEM storedExpo = EXPO;
uint8_t EEMEM storedTempLimit = TEMP_LIMIT;

//Setup 2 ports which we are using for H bridge. e.g.
/* Forward = C High, A Low

   Reverse = C Low,  A High
 */

#define setForwardLow  setALow
#define clrForwardLow   clrALow

#define setForwardHigh setCHigh
#define clrForwardHigh  clrCHigh

#define setBackwardLow  setCLow
#define clrBackwardLow  clrCLow

#define setBackwardHigh setAHigh
#define clrBackwardHigh clrAHigh


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

//Motor State Functions
void goForwards();
void braking();
void goBackwards();

//Programming functions
void motorBeep(uint8_t length);
void fastBeep(uint8_t length);
void readData();


int main() {
    //Initialisation

    //Setup Outputs
    _delay_ms(10);

    clrBackwardLow();
    clrBackwardHigh();
    clrForwardLow();
    clrForwardHigh();

    SET_HIGH_A_PORT |= (1 << HIGH_A);
    SET_HIGH_B_PORT |= (1 << HIGH_B);
    SET_HIGH_C_PORT |= (1 << HIGH_C);

    SET_LOW_A_PORT |= (1 << LOW_A);
    SET_LOW_B_PORT |= (1 << LOW_B);
    SET_LOW_C_PORT |= (1 << LOW_C);


    //Setup Timer
    TCCR0 = 0b00000011; //Setup Timer 0 - scaling of ?? (go check the datasheet and fill it in)
    TIMSK = (1 << TOIE0) | (1 << OCIE2) | (1 << TOIE2); //Enable Timer0 Overflow interrupt, Timer2 Output Compare and Timer2 Overflow interrupt

    //Setup PWM Timer
    OCR2 = 0x00; //Set the compare value to 0 or off
    TCCR2 = (1 << CS21); //Set the timer 2 scaling of (go check the datasheet and fill it in)

    //Enable RC Interrupt
    MCUCR |= (1 << ISC00); //Set INT0 to trigger on both edges
    GICR |= (1 << INT0); //Enable INT0

    newState = unitialised;
    braking();

    _delay_ms(100);



    //Setup ADC
#ifdef TEMP_ADMUX
	if (tempLimit){
	    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ADC prescaler to 128 - 125KHz sample rate @ 16MHz

    	ADMUX |= (1 << REFS0); // Set ADC reference to AVCC
    	ADMUX |= (1 << ADLAR); // Left adjust ADC result to allow easy 8 bit reading

    	ADMUX |= TEMP_ADMUX; //Set ADC to temp sensor

    	ADCSRA |= (1 << ADFR); // Set ADC to Free-Running Mode
    	ADCSRA |= (1 << ADEN); // Enable ADC
    	ADCSRA |= (1 << ADIE); // Enable ADC Interrupt
    	ADCSRA |= (1 << ADSC); // Start A2D Conversions
	}
#endif

    timeout = 0;
    sei();
    //End initialisation 


    motorBeep(3); //Go Beep Go

    //Main loop
    //Here we manage what mode we are in everything else is done in interrupts
    while (1) {
        //RC Tick runs @ approximately 1kHz on @ 16Mhz or 500hz @ 8Mhz        
        if (rcTick) {
            //Manage State
            if (state != newState) {
            
                //If we are in the brake state then we can go in either direction
                //if (state == brake){

                if (newState == forward && state == brake) {
                    OCR2 = 0;
                    goForwards();
                } else if (newState == backward && state == brake) {
                    OCR2 = 0;
                    goBackwards();
                } else {
                    //If we have ramped down then go to braking mode
                    if (OCR2 <= 10) {
                        braking();
                        OCR2 = 255;
                    }                        //Other wise we want to change our state so first we need to ramp down, then we can change state.
                    else {
                        OCR2 = OCR2 - maxSlew;
                    }
                }
            }                //Manage slew rate
            else if (OCR2 != desiredPWM) {
                if (desiredPWM > (OCR2 + maxSlew)) {
                    OCR2 = OCR2 + maxSlew;
                } else if (desiredPWM < (OCR2 - maxSlew)) {
                    OCR2 = OCR2 - maxSlew;
                } else {
                    OCR2 = desiredPWM;
                } 
            }
            rcTick = 0; 
        }
    }
}


//This interrupt manages failsafe and time0 for upper half of RC timer (lower half is TCNT0)

ISR(TIMER0_OVF_vect) {
    //Check failsafe

    if (timeout == 0) { //If we are out of counts go to braking mode
        newState = brake;
    } else {
        timeout--; //otherwise decrease failsafe counter
    }
    time0++; //time0 is upper bit for RC timer. 
    rcTick++;
}

//This interrupt manages RC pulses

ISR(INT0_vect) {
    //Read in the current time
    uint16_t time = TCNT0; //Read lower
    time = time + time0 * 256; //Add upper
    //Read in current time
    if (RC_PORT & (1 << RC_PIN)) { //If the pin is high (then its just switched on)
        prevTime = time; //then save the current time
    } else {
        uint16_t time_diff = time - prevTime;
        rcTime = time_diff;
        //Update PWM and direction
        uint8_t deadzone = DEADTIME;
        uint8_t buffer = BUFFERTIME;
        uint8_t maxOut = MAXOUTTIME;

 

        if (((time_diff > (rcLow - buffer)) && (time_diff < (rcHigh + buffer))) && (tempState != HOT)) { //Check for valid pulse
            if (failsafe > 10) { //Need 10 good pulses before we start
                failsafe = 15; //Set valid pulse counter (15-10 = 5) therfore we can receive 5 bad pulses before we stop
                timeout = 50; //Set timeout counter

                //Vaild signal
       
                if (time_diff > (rcMid - deadzone) && time_diff < (rcMid + deadzone)) { //If center brake
                    desiredPWM = 255;
                    newState = brake;

                } else if (time_diff < rcMid) { //If low then go backwards

                    if (time_diff < (rcLow + maxOut)) {
                        desiredPWM = 255;
                    } else {
                        desiredPWM = (rcMid - deadzone - time_diff)*256 / (rcMid - deadzone - rcLow);
                        if (EXPO) {
                            desiredPWM = (uint16_t) desiredPWM * desiredPWM / 256;
                        }
                    }
                    newState = backward;

					if(tempLimit){
                    	if (tempState == WARM && OCR2 > 128) {
                    	    desiredPWM = 128;
                    	}
					}

                } else { //Else go forward
                    if (time_diff > (rcHigh - maxOut)) {
                        desiredPWM = 255;
                    } else {
                        desiredPWM = (time_diff - rcMid - deadzone)*256 / (rcHigh - rcMid - deadzone);
                        if (EXPO) {
                            desiredPWM = (uint16_t) desiredPWM * desiredPWM / 256;
                        }
                    }
                    newState = forward;

					if(tempLimit){
		                if (tempState == WARM && OCR2 > 128) {
		                    desiredPWM = 128;
		                }
					}

                }
            } else {
                failsafe++; //If havent got 10 valid pulses yet increase count and keep waiting
            }
        } else { //If there is an invalid pulse
            //Failsafe
            failsafe--; //decrease counter
            if (failsafe < 10) { //If we have had enough invalid pulses
                newState = brake; //go to failsafe breaking mode
            }
        }
    }
}

ISR(TIMER2_COMP_vect) { //When comparator is triggered clear low ports (off part of PWM cycle)
    if (OCR2 < 255) {
        if (state != brake) {
            //Clear high
            clrForwardHigh();
            clrBackwardHigh();
        }
    }
}

ISR(TIMER2_OVF_vect) { //When timer overflows enable low port for current state
    if (OCR2 > 0) {
        if (state == forward) {
            setForwardHigh();
        } else if (state == backward) {
            setBackwardHigh();
        }
    }
    if (OCR2 > 250 && state != brake) {
        lets_get_high++; //Gotta keep the charge pump circuit charged
        if (lets_get_high > 50) { //If it hasn't had a chance to charge in a while
            clrForwardHigh(); //Clear it then reset counter   
            clrBackwardHigh(); //Now pumped up and remaining high so we don't nasty shoot through which ends in magic smoke :)
            lets_get_high = 0;
        }
    }
}

#ifdef TEMP_ADMUX
ISR(ADC_vect) {
    if (ADCH < I_AM_HOT) {
        tempState = HOT;
        newState = brake;
    } else if (ADCH < I_AM_WARM) {
        tempState = WARM;
    } else {
        tempState = COOL;
    }
}
#endif


//Motor State Functions

void goForwards() {
    if (state != forward) {
        cli();

        //Clear unused pins
        clrBackwardHigh();
        clrBackwardLow();
        _delay_us(200); //Fet switch delay
        setForwardLow();
        _delay_us(200); //Fet switch delay
        setForwardHigh();
        state = forward; //Up
        sei();
    }
}

void braking() {

    if (state != brake) {
        cli();

        //Clear unused pins
        clrForwardHigh();
        clrBackwardHigh();
        _delay_us(200); //Fet switch delay
        setForwardLow();
        setBackwardLow();
        state = brake;
        _delay_ms(10);
        sei();
    }
}

void goBackwards() {
    if (state != backward) {
        cli();

        //Clear high
        clrForwardHigh();
        clrForwardLow();
        _delay_us(200); //Fet switch delay
        setBackwardLow();
        _delay_us(200); //Fet switch delay
        setBackwardHigh();
        state = backward;
        sei();
    }
}

void motorBeep(uint8_t length) {
    OCR2 = 10;
    uint8_t count = 0;
    while (count < length) {
        if (count % 2) {
            goForwards();
        } else {
            goBackwards();
        }
        _delay_ms(200);
        count++;
        braking();
        _delay_ms(300);
    }
    braking();
    OCR2 = 0;
}

void fastBeep(uint8_t length) {
    OCR2 = 20;
    uint8_t count = 0;
    while (count < length) {
        if (count % 2) {
            goForwards();
        } else {
            goBackwards();
        }
        _delay_ms(50);
        count++;
        braking();
        _delay_ms(100);
    }
    braking();
    OCR2 = 0;
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
