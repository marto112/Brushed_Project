#include <math.h>
#include <c8051f330.h>    // SFR declarations

//Setup Delays
#define SYSCLK  24500000  // SYSCLK frequency in Hz
#include "delay.h"
#include "../../pindefs/2in1_plush10.h"
#include "motor.h"
#include "init.h"

//Define RC States
#define unitialised  0
#define forward  1
#define backward  2
#define brake  3
#define spinLeft 4
#define spinRight 5

//Macros
#define ABS(a)	   (((a) < 0) ? -(a) : (a))

//Initialization Functions
void SYSCLK_Init(void);
void Timer0_Init(void);
void Timer1_Init(void);
void PORT_Init(void);
void PCA_Init(void);

//Motor States
void goForwards();
void braking();
void goBackwards();
void goSpinLeft();
void goSpinRight();

//Variables
char state;
int timeCH1;
int timeCH2;

char failsafe = 0;
char timeout = 50;

char newState = 0;
char lets_get_high = 0;

char rcTick = 0;
unsigned int desiredPWM = 0;
char newData1 = 0;
char newData2 = 0;
char chCount = 0;
int CH1 = 0;
int CH2 = 0;

//SiLabs Specific Values
unsigned int Next_Compare_Value = 0;
char pwm_state = 0;
int OCRA_var = 500;
int OCRB_var = 4000;

void OCRA();
void OCRB();
void Overflow();

const unsigned char deadzone = 40;
const unsigned char buffer = 20;   
const unsigned char HYSTERESIS = 50;
 
unsigned int magnitude;
unsigned int angle;
unsigned int temp;

void main(){

	SYSCLK_Init();
	PORT_Init();
    PCA_Init();
	Timer0_Init();
    Timer1_Init();
	EA = 0;

    clrALow();
    clrAHigh();
    clrBLow();
    clrBHigh();
    clrCLow();
    clrCHigh();
	delay_ms(100);
	

	EA = 1;
	PCA0MD |= 0x40; //Start watchdog timer
	while(1){
		PCA0CPH2 = 0x00; //Set watchdog reset
          

        if(newData1 != 0 && newData2 != 0){
            //valid pulse
            if ((timeCH1 > (RC_LOW - buffer)) && (timeCH1 < (RC_HIGH + buffer)) && (timeCH2 > (RC_LOW - buffer)) && (timeCH2 < (RC_HIGH + buffer))){
                failsafe++;               
                if (failsafe > 10){   //Need 10 good pulses before we start
                    failsafe = 15;    //Set valid pulse counter (15-10 = 5) therfore we can receive 5 bad pulses before we stop
                    timeout = 50;     //Set timeout counter
                }
            }
            else {
                failsafe--;
            }
            newData1 = 0; newData2 = 0;
        }

        if(1){//failsafe == 15 && timeout != 0){
            if (rcTick != 0){
                //goBackwards();
                //Need to handle states here 

				//Manage Slew
				if (ABS(timeCH1 - CH1) > MAX_SLEW){
					if (timeCH1 > CH1){
						CH1 = CH1 + MAX_SLEW;
					}
					else {
						CH1 = CH1 - MAX_SLEW;
					}
				} else {
					CH1 = timeCH1;
				}
				if (ABS(timeCH2 - CH2) > MAX_SLEW){
					if (timeCH2 > CH2){
						CH2 = CH2 + MAX_SLEW;
					}
					else {
						CH2 = CH2 - MAX_SLEW;
					}
				} else {
					CH2 = timeCH2;
				}

		        //Work out the magnitude of the RC signal.
		        //this gives a squared distance up to 15 bits. 
		        //SQRT for linear or just scale it to keep exponential rates
		        //magnitude = (CH2)*(CH2) + (CH1)*(CH1)*PCA_TIMEOUT/(RC_MID - RC_LOW); //Scale to 8 bit from 15.
        
				 
				/*if ((CH1 > 0) && (abs(CH1) > abs(CH2)) ){
					goForwards(); 
				} else if ((CH1 > 0) && (abs(CH1) > abs(CH2))) {
					goBackwards();
				} else if (CH2 > 0){
					goSpinLeft();
				} else {
					goSpinRight();
				}*/ 
				goForwards();
                rcTick = 0; 
            }
        }
        else {
           braking();
        }              
	}	
}


//Motor State Functions
void goForwards(){
    EA = 0; //Disable interrupts
    if (state != forward){
        //Clear unused pins
        clrALow();
        clrBHigh();
        clrCLow();

        uSecDelay(uSecToInstCycles(200)); //Fet switch delay

        setBLow(); //Enable Common pin
	
	    uSecDelay(uSecToInstCycles(200)); //Fet switch delay
	    setAHigh(); //Enable one motor	
    	//Both motors are enabled at slightly different times
    	//to hopefully be a little bit nicer to the fets
    	//200us should have no noticible effect on driving straight
        uSecDelay(uSecToInstCycles(200)); //Fet switch delay
        setCHigh(); //Enable 2nd motor

        state = forward; //Up
    }
    EA = 1; //Enable interrupts
}

void braking(){
    EA = 0; //Disable interrupts
    if (state != brake){
        //Clear unused pins
        clrAHigh();
        clrBHigh();
        clrCHigh();

        uSecDelay(uSecToInstCycles(200)); //Fet switch delay
	    setALow();
        setBLow();
        setCLow();

        state = brake;
    }
    EA = 1; //Enable interrupts
}

void goBackwards(){
    EA = 0; //Disable interrupts
    if (state != backward){

        //Clear unused pins
        clrAHigh();
        clrBLow();
        clrCHigh();

        uSecDelay(uSecToInstCycles(200)); //Fet switch delay
        setBHigh(); //Enable Common pin
	
	    uSecDelay(uSecToInstCycles(200)); //Fet switch delay
	    setALow(); //Enable one motor
	
	    //Both motors are enabled at slightly different times
	    //to hopefully be a little bit nicer to the fets
	    //200us should have no noticible effect on driving straight
        uSecDelay(uSecToInstCycles(200)); //Fet switch delay
        setCLow(); //Enable 2nd motor
	
        state = backward;
    }
    EA = 1; //Enable interrupts
}

void goSpinLeft(){
    EA = 0; //Disable interrupts
    if (state != spinLeft){

        //Clear unused pins
        clrAHigh();
        clrBLow();
        clrBHigh();
        clrCLow();

        uSecDelay(uSecToInstCycles(200)); //Fet switch delay
        setALow(); //Enable Common pin
	
	    uSecDelay(uSecToInstCycles(200)); //Fet switch delay
	    setCHigh(); //Enable one motor
	
        state = spinLeft;
    }
    EA = 1; //Enable interrupts
}

void goSpinRight(){
    EA = 0; //Disable interrupts
    if (state != spinRight){

        //Clear unused pins
        clrALow();
        clrBLow();
        clrBHigh();
        clrCHigh();

        uSecDelay(uSecToInstCycles(200)); //Fet switch delay
        setAHigh(); //Enable Common pin
	
	    uSecDelay(uSecToInstCycles(200)); //Fet switch delay
	    setCLow(); //Enable one motor
	
        state = spinRight;
    }
    EA = 1; //Enable interrupts
}


void Timer1_ISR(void) interrupt 3 {
    //Check failsafe
    if (timeout == 0) { //If we are out of counts go to braking mode
        newState = brake;
    } else {
        timeout--; //otherwise decrease failsafe counter
    }
    rcTick++;
}

unsigned int maxOut = RC_MAXOUT;
unsigned int time;

void INT0_ISR(void) interrupt 0 {
    EA = 0;
    time = TL0 + 256 * TH0;
    
    if (time > 10000){
        chCount = 0;
    } else {
        chCount++;
        if (chCount == 1){
            timeCH1 = time - RC_MID;
            newData1++;
        } else if (chCount == 2){
            timeCH2 = time - RC_MID;
            newData2++;
        }
    }
    
    TH0 = 0x00; // Reinit Timer0 High register
    TL0 = 0x00; // Reinit Timer0 Low register
	
	EA = 1;
	
}

unsigned int duty_cycle = 0;
void PCA0_ISR(void) interrupt 11 {
    EA = 0;
    if (CCF0) // If Module 0 caused the interrupt
    {
        CCF0 = 0; // Clear module 0 interrupt flag.
        // Set up the variable for the following edge
	
		if (pwm_state == 0){

			if ( OCRA_var < OCRB_var){
				OCRA();
				duty_cycle = OCRB_var - OCRA_var;
			} else {
				OCRB();
				duty_cycle = OCRA_var - OCRB_var;
			}
			if (duty_cycle < 10) {
				pwm_state++;
			}
		
		}

		if (pwm_state == 1){

			if ( OCRA_var < OCRB_var){
				OCRB();
				duty_cycle = PCA_TIMEOUT - OCRB_var;
			} else {
				OCRA();
				duty_cycle = PCA_TIMEOUT - OCRA_var;
			}
			if ( duty_cycle < 10){
				pwm_state++;
			}
		
		}

		if (pwm_state == 2){
			Overflow();
			if ( OCRA_var < OCRB_var){
				duty_cycle = OCRA_var;
			} else {
				duty_cycle = OCRB_var;
			}
			if (duty_cycle < 10){
				duty_cycle = 10;
			};
			pwm_state =0;
		} else {
			pwm_state++;
		}

		Next_Compare_Value = PCA0CP0 +  duty_cycle;
		PCA0CPL0 = (Next_Compare_Value & 0x00FF);
	    PCA0CPH0 = (Next_Compare_Value & 0xFF00) >> 8;
    }
	else // Interrupt was caused by other bits.
	{
    	PCA0CN &= ~0x86; // Clear other interrupt flags for PCA
	}
	EA = 1;
}


void OCRA(){
    if (state == forward){
        //A trigger
        clrCHigh();
    }
    else if (state == backward){
        //A trigger
        clrCLow();
    }
    else if (state == spinLeft){
        clrBHigh();
        uSecDelay(uSecToInstCycles(10)); //Fet switch delay
        setBLow();
    }
    else if (state == spinRight){
        clrBLow();
        uSecDelay(uSecToInstCycles(10)); //Fet switch delay
        setBHigh();
    } 
}

void OCRB(){
	if (state == forward){
        //B trigger
        clrAHigh();
    }
    else if (state == backward){
        //B trigger
        clrALow();
    }
    else if (state == spinLeft){
        clrCHigh();
    }
    else if (state == spinRight){
        clrCLow();
    }
}

void Overflow(){
    if (state == backward){
        setALow();
        setBHigh();
        setCLow();

        if ((OCRA_var > PCA_TIMEOUT*0.90 ) || (OCRB_var > PCA_TIMEOUT*0.90 )){    
	        lets_get_high++; //Gotta keep the charge pump circuit charged
	        if (lets_get_high > 100) { //If it hasn't had a chance to charge in a while           
	            clrBHigh();
	            lets_get_high = 0;
	        }
		}
    }
    else if (state == forward){
        setAHigh();
        setBLow();
        setCHigh();
        
        if ((OCRA_var > PCA_TIMEOUT*0.90 ) || (OCRB_var > PCA_TIMEOUT*0.90 )){    
            lets_get_high++; //Gotta keep the charge pump circuit charged
            if (lets_get_high > 100) { //If it hasn't had a chance to charge in a while           
                clrAHigh();
                clrCHigh();
                lets_get_high = 0;
            }
        }
    }

    else if (state == spinLeft){
        setALow();
        clrBLow();
        uSecDelay(uSecToInstCycles(10)); //Fet switch delay
        setBHigh();
        setCHigh();
        if ((OCRA_var > PCA_TIMEOUT*0.90 ) || (OCRB_var > PCA_TIMEOUT*0.90 )){   
            lets_get_high++; //Gotta keep the charge pump circuit charged
            if (lets_get_high > 100) { //If it hasn't had a chance to charge in a while           
                clrBHigh();
                clrCHigh();
                lets_get_high = 0;
            }
        }
   }
   else if (state == spinRight){
        setAHigh();
        clrBHigh();
        uSecDelay(uSecToInstCycles(10)); //Fet switch delayssss
        setBLow();
        setCLow();
        if ((OCRA_var > PCA_TIMEOUT*0.90 ) || (OCRB_var > PCA_TIMEOUT*0.90 )){   
            lets_get_high++; //Gotta keep the charge pump circuit charged
            if (lets_get_high > 100) { //If it hasn't had a chance to charge in a while           
                clrBHigh();
                clrCHigh();
                lets_get_high = 0;
            }
        }
    }
}
