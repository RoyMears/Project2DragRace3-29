// SysTick.h
// Runs on TM4C123
// Starter file for CECS346 Project 2
// By Dr. Min He


#include <stdint.h> // C99 data types

#define HALF_SEC						12500000   // Constant used to for clock decrement generating 0.5s time interval. U follows a constant indicate this is an unsigned number

// Function Prototypes (external functions from startup.s)
void SysTick_Init(void);      
void SysTick_Start(uint32_t period);
void SysTick_Stop(void);