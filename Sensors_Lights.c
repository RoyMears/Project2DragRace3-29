// Sensors_Lights.c
// Starter file for CECS346 Project 2
// By Dr. Min He
#include "tm4c123gh6pm.h"
#include <stdint.h> // C99 data types
#include "Sensors_Lights.h"
const uint8_t  PUSHBUTTONS_MASK = 0x0C;
const uint8_t  LIGHT_PORTB_MASK = 0x0F;
const uint8_t  LIGHT_PORTC_MASK = 0xF0;
const uint8_t  RESET_MASK = 0x02;

// Initialize the two sensors, enable both edge edge-triggered interrupt for both sensors
// Utilizing port a for sensor input
void Sensors_Init(void){
		if ((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOA) != SYSCTL_RCGC2_GPIOA) { 
		SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;      // Activate Port A clocks FEDCBA
		}
		while ((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOA) != SYSCTL_RCGC2_GPIOA) {}	
			
		GPIO_PORTA_DIR_R 		&= ~PUSHBUTTONS_MASK;										// inputs on PA3 - PA2 
	  GPIO_PORTA_DEN_R 		|= PUSHBUTTONS_MASK;										// enable digital PA3 - PA2
	  GPIO_PORTA_AMSEL_R 	= 0;																		// disable analog  PA3 - PA2
	  GPIO_PORTA_PCTL_R 	&= ~0x0000FF00;												  // clear PCTL PA3 - PA2
	  GPIO_PORTA_AFSEL_R 	&= ~PUSHBUTTONS_MASK;										// no alternate function PA3 - PA2
	  
	  // Interrupt portA
	  GPIO_PORTA_IS_R 	&= ~PUSHBUTTONS_MASK;											// edge sensitive PA3 - PA2
	  GPIO_PORTA_IBE_R 	|= PUSHBUTTONS_MASK;											// both edges PA3 - PA2
	  GPIO_PORTA_ICR_R 	|= PUSHBUTTONS_MASK;											// clear flag
	  GPIO_PORTA_IM_R 	|= PUSHBUTTONS_MASK;											// arm interrupt 
	  NVIC_PRI0_R 			= (NVIC_PRI0_R & 0xFFFFFF00) | 0x00000040;// priority 2
	  NVIC_EN0_R 				= NVIC_EN0_PORTA;										// NVIC enable interrupt 0
		
		
}





// Initialize the reset button: use level triggered interrupt
void Reset_Init(void){
	if ((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOE) != SYSCTL_RCGC2_GPIOE) { 
		SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;      // Activate Port E clocks FEDCBA
	}
	while ((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOE) != SYSCTL_RCGC2_GPIOE) {}	
		
	GPIO_PORTE_CR_R 		|= RESET_MASK;
	GPIO_PORTE_DIR_R 		&= ~RESET_MASK;														// input on PE1
	GPIO_PORTE_DEN_R 		|= RESET_MASK;														// enable digital PE1
	GPIO_PORTE_AMSEL_R 	= 0;																			// disable analog PE1
	GPIO_PORTE_PCTL_R 	&= ~0x000000F0;														// GPIO clear bit PCTL for PE1
	GPIO_PORTE_AFSEL_R 	&= ~RESET_MASK;														// no alternate function for PE1
	
	// Port E Interrupt
	GPIO_PORTE_IS_R 	|= RESET_MASK;															// PE1 is level sensitive
	GPIO_PORTE_IEV_R 	&= ~RESET_MASK;															// active low trigger
	GPIO_PORTE_ICR_R 	|= RESET_MASK;															// clear flag
	GPIO_PORTE_IM_R 	|= RESET_MASK;															// arm interrupt on PE1
	NVIC_PRI1_R 			= (NVIC_PRI1_R & 0xFFFFFF00) | 0x00000020;	// priority 1
	NVIC_EN0_R 				= NVIC_EN0_PORTE;														// NVIC enable interrupt 4 
	
}




// Initialize 8 LEDs
void LightsB_Init(void){
	if ((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOB) != SYSCTL_RCGC2_GPIOB) { 
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;      // Activate Port B clocks FEDCBA
	}
	while ((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOB) != SYSCTL_RCGC2_GPIOB) {}	
	GPIO_PORTB_DIR_R 		|= LIGHT_PORTB_MASK;														// output on PB3 - PB0
	GPIO_PORTB_AMSEL_R 	&= ~LIGHT_PORTB_MASK;														// disable analog PB3 - PB0
	GPIO_PORTB_AFSEL_R 	&= ~LIGHT_PORTB_MASK;														// no alternate function PB3 - PB0
	GPIO_PORTB_PCTL_R 	&= ~0x0000FFFF;															    // clear PCTL for PB3 - PB0
	GPIO_PORTB_DEN_R 		|= LIGHT_PORTB_MASK;														// enable digital PB3 - PB0
			
			
}


void LightsC_Init(void){
				
	if ((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOC) != SYSCTL_RCGC2_GPIOC) { 
		SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOC;      // Activate Port C clocks FEDCBA
	}
	while ((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOC) != SYSCTL_RCGC2_GPIOC) {}			
	GPIO_PORTC_AMSEL_R 	&= ~LIGHT_PORTC_MASK;														// disable analog PC7 - PC4
	GPIO_PORTC_PCTL_R 	&= ~0xFFFF0000;																	// GPIO clear PCTL PC7 - PC4
	GPIO_PORTC_DIR_R 		|= LIGHT_PORTC_MASK;														// output on PC7 - PC4
	GPIO_PORTC_AFSEL_R 	&= ~LIGHT_PORTC_MASK;														// no alternate PC7 - PC4
	GPIO_PORTC_DEN_R 		|= LIGHT_PORTC_MASK;														// enable digital PC7 - PC4
	
}


