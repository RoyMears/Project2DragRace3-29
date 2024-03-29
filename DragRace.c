// DragRace.c
// Starter file for CECS346 Project 2
// By Dr. Min He
// 3/21/2024

#include "tm4c123gh6pm.h"
#include <stdint.h> // C99 data types
#include <stdbool.h> // provides boolean data type
#include "PLL.h"
#include "SysTick.h"
#include "Sensors_Lights.h"

// Function Prototypes
// External functions from startup.s
extern void DisableInterrupts(void); // Disable interrupts
extern void EnableInterrupts(void);  // Enable interrupts
extern void WaitForInterrupt(void);  // Go to low power mode while waiting for the next interrupt

// Functions defined in this file.
void System_Init(void);

//TODO: Bit addresses for two sensors, 8 color lights, and one reset button 
//#define SENSORS 								(*((volatile unsigned long *)          )) // bit addresses for  2 sensors 
//#define LIGHTS              		(*((volatile unsigned long *)          )) // bit addresses for 8 Race Lights 
//#define RESET                   (*((volatile unsigned long *)          )) // bit address for one reset button
 
// FSM 
struct State { 
	uint8_t Out;
	uint8_t Time;     // multiple of 0.5 second
	uint8_t Next[4];
};

typedef const struct State STyp;

// Constant definitions for FSM outputs

// TODO: define reload value for half second
//#define HALF_SEC   ?

//TODO: assign a value to all states in Drag Race FSM
//enum DragRace_states { };

//TODO: Define Drag Race FSM
STyp DragRace_FSM[] = {};

uint8_t S;  // current state index
uint8_t Input;
bool timesup;
bool reset;  // flag to reset the system, set by the reset button located at breadboard, not the launchpad reset button.
	
	
int main(void){
	System_Init();
		
  while(1){
		while (!reset) {
			// TO Do: take care of FSM outputs and time in state.
//			LIGHTS = DragRace_FSM[S].Out;
//			SysTick_Start(DragRace_FSM[S].Time*HALF_SEC);
			while((!timesup)&&(!reset)){
			  WaitForInterrupt();
			}
			timesup=false;
			S = DragRace_FSM[S].Next[Input];
		}
  }
}

// Initialize the following hardware modules: PLL, Sensors, Reset button, Lights, SysTick timer
// Initialize the following golbal variables: timesup for FSM state time, reset for reset the whole system
void System_Init(void) {
	DisableInterrupts();
	PLL_Init();
  Sensors_Init(); 
	Reset_Init(); 
	Lights_Init();
	SysTick_Init(); 
  // TODO: assign Initial state, reset global variables: timesup and reset 
  // S = ? ; 
	timesup = false;
	// reset = ?
	EnableInterrupts();
}

// Interrupt handler for the two sensors: take care of Input here 
//void GPIOPortx_Handler(void){
//}

// Interrupt handler for reset button: Resets the board to the initalization state: take care of global variable reset, timesup and S here 
//void GPIOPortx_Handler(void) {
//}

// Systick interrupt handler: stop systick timer and take care of timesup 
void SysTick_Handler(void) {
}

