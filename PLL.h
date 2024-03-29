// PLL.h
// Runs on TM4C123
// Starter file for CECS346 Project 2
// By Dr. Min He

// TODO:" The #define statement SYSDIV2 initializes
// the PLL to the desired frequency. -> Desiring 25MHz 

#define SYSDIV2 15       // 400MHz / (15+1) = 25MHz 
// configure the system to get its clock from the PLL

void PLL_Init(void);