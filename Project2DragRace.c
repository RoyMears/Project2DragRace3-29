// Course number: CECS 346
// Assignment number: LAB 6 Interrupts
// Authors: Roy Mears, Ivan Martinez, James Henry
// Date: 3/15/2024
// System Requirements:
// The embedded system will start with the onboard red LED on and blink at a speed of 0.5s on/0.5s off.
// Every time the onboard switch 2 (PF0) is pressed and released, the LED color will be changed to the
// next color in a round robin order of red, blue, and Green. The new LED will blink at the same speed.

// Documentation Section
// main.c
// Runs on TM4C123
// Lab6_Interrupts, Input from PF0, output to PF3,PF2,PF1 (LEDS)
// Authors: Roy Mears, Ivan Martinez, James Henry
// Date: 3/15/2024
// LaunchPad built-in hardware
// SW2 right switch is negative logic PF0 on the Launchpad
// red LED connected to PF1 on the Launchpad
// blue LED connected to PF2 on the Launchpad
// Green LED connected to PF3 on the Launchpad
// 
// DraGraceStarter.c
// Starter file for CECS346 Project 2

/*
Hardware Design
	PortA controls sensors 
		left sensor 	- PA2
		right sensor 	- PA3
		
	PortB LEDs 
		rightRed 		- PB0
		leftRed 		- PB1
		rightGreen 	- PB2
		leftGreen 	- PB3
	
 	PortC LEDs
		rightYellow1 - PC4
		leftYellow1  - PC5
		rightYellow2 - PC6
		rightYellow2 - PC7
		
 	PortE is for reset 
		reset - PE1
	
*/




#include "tm4c123gh6pm.h"
#include <stdint.h> // C99 data types
#include <stdbool.h> // provides boolean data type
#include "PLL.h"
#include "SysTick.h"
#include "Sensors_Lights.h"


// Function Prototypes (external functions from startup.s)
extern void DisableInterrupts(void); // Disable interrupts
extern void EnableInterrupts(void);  // Enable interrupts
extern void WaitForInterrupt(void);  // go to low power mode while waiting for the next interrupt

// Function Prototypes
void Sensors_Init(void); // Initialize the left and right push buttons
void Reset_Init(void);   // Initialize reset button

void LightsB_Init(void);  // Initialize portB race lights 
void LightsC_Init(void);  // Initialize portC race lights 


void SysTick_Init(void); // Initialize SysTick timer with interrupt enabled 
void SysTick_Start(uint32_t reload); // Start systick timer with specified reload value.
void System_Init(void);
void debounce(void);						// debounce module for button

//TODO: Bit addresses for two sensors, 8 color lights, and one reset button 

#define PUSHBUTTONS_PORTA 						(*((volatile unsigned long *)0x40004030))//PA2, PA3
#define LIGHT_PORTB					 					(*((volatile unsigned long *)0x4000503C))//PB0, PB1, PB2, PB3
#define LIGHT_PORTC										(*((volatile unsigned long *)0x400063C0))//PC4, PC5, PC6, PC7
#define RESET 												(*((volatile unsigned long *)0x40024008))//reset - PE1
#define PUSHBUTTON_LEFT_MASK     0x04      
#define PUSHBUTTON_RIGHT_MASK    0x08      

// FSM                                                                           
struct State { 
	uint8_t Out;
	uint8_t Time;     // multiple of 0.5 second
	uint8_t Next[4];
};

typedef const struct State STyp;

// Constant definitions


// assign a value to all states in Drag Race FSM
enum DraGrace_states {
init, 
wait, 
yellow1, 
yellow2, 
go, 
falseStartLeft, 
falseStartRight, 
falseStartBoth, 
winLeft, 
winRight, 
bothWin 
};


// Drag Race FSM
// Out  Time	Next States
STyp DraGrace_FSM[11] = { // input 11, 10, 01, 00
	{0xFF, 2, {wait, wait, wait, wait}},	  																		// initialize state, all LEDs on, SysTick Timer restarts      		
	{0x00, 1, {wait, wait, wait, yellow1}},																			// waiting state, all LEDs off									
	{0xC0, 1, {falseStartBoth, falseStartLeft, falseStartRight, yellow2}},			// countdown yellow1 state, yellow1L & yellow1R LEDs on																			
	{0x30, 1, {falseStartBoth, falseStartLeft, falseStartRight, go}},			// countdown yellow2 state, yellow2L & yellow2R LEDs on
	{0x0C, 1, {bothWin, winLeft, winRight, go}},													// go state, GreenLeft & Gr LEDs on
	{0x02, 2, {wait, wait, wait, wait}},																	// false start left, redLeft LED on
	{0x01, 2, {wait, wait, wait, wait}},																	// false start right, redRight LED on
	{0x03, 2, {wait, wait, wait, wait}},																	// false start both, redleft and redRight LEDs on
	{0x08, 2, {wait, wait, wait, wait}},																	// win left, GreenLeft LED on
	{0x04, 2, {wait, wait, wait, wait}},																	// win right, GreenRight LED on
	{0x0C, 2, {wait, wait, wait, wait}}																		// win both, GreenLeft & Gr LED on
	};

uint8_t S;  // current state index
uint8_t timesup;
uint8_t Input;
uint8_t reset;  // flag to reset the system, set by the reset button located at breadboard, not the launchpad reset button.
	
	
int main(void){
	System_Init();
		
  while(1){
		while (!reset) {
			
			  LIGHT_PORTB = (DraGrace_FSM[S].Out & LIGHT_PORTB_MASK); // was LIGHT_PORTB = (DraGrace_FSM[S].Out);
			  LIGHT_PORTC = (DraGrace_FSM[S].Out & LIGHT_PORTC_MASK); // was LIGHT_PORTC = (DraGrace_FSM[S].Out);
				SysTick_Start(DraGrace_FSM[S].Time * HALF_SEC);
			
			while((!timesup)&&(!reset)){
			  WaitForInterrupt();
			}
			
			SysTick_Stop();
			timesup=false;
			S = DraGrace_FSM[S].Next[Input];
		}
		
		reset = false;
		S = init;
  }
}

void System_Init(void) {
	DisableInterrupts();
	PLL_Init();
	Sensors_Init(); 
	Reset_Init(); 
	LightsB_Init();
	LightsC_Init();
	SysTick_Init();
	 // TODO: assign Initial state, reset global variables: timesup and reset 
	S = init;
	timesup = false;   
	reset = false;
  Input = PUSHBUTTONS_PORTA >> 2;		
	EnableInterrupts();
}
	


// Interrupt handler for the two sensors: take care of Input here 
void GPIOPortA_Handler(void){
	
	unsigned long i;
	for (i = 0; i < 182828; i++) {
	}
	
	if ((GPIO_PORTA_RIS_R & PUSHBUTTON_LEFT_MASK)||(GPIO_PORTA_RIS_R & PUSHBUTTON_RIGHT_MASK)){ // if left or right or both button is pressed
					GPIO_PORTA_ICR_R = PUSHBUTTON_LEFT_MASK;  // clear flag
					Input = PUSHBUTTONS_PORTA >> 2;		//take 
	}
}



// Interrupt handler for reset button: Resets the board to the initalization state: take care of global variable reset here 
void GPIOPortE_Handler(void) {
	unsigned long i;
	for (i = 0; i < 182828; i++) {
	}
	if(RESET & RESET_MASK){
		reset = true;
		GPIO_PORTE_ICR_R 	|= RESET_MASK;	// clear flag	
	}		
}

// Take care of timesup here
void SysTick_Handler(void) {
	timesup = true;
}


