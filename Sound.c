// Sound.c, derived from SysTickInts.c

// Runs on LM4F120 or TM4C123, 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano
// September 15, 2013
// Port B bits 2-0 have the 3-bit DAC
// Port F is onboard LaunchPad switches and LED
// Port F bit 4 is negative logic switch to play sound, SW1
// SysTick ISR: PF3 ISR heartbeat
#include "tm4c123gh6pm.h"

unsigned char Index;  

// 6-bit 64-element sine wave
const unsigned short SineWave[64] = {  
  32,35,38,41,44,47,49,52,54,56,58,
  59,61,62,62,63,63,63,62,62,61,59,
  58,56,54,52,49,47,44,41,38,35,32,
  29,26,23,20,17,15,12,10,8,6,5,
  3,2,2,1,1,1,2,2,3,5,6,
  8,10,12,15,17,20,23,26,29
};  


	
// **************DAC_Init*********************
// Initialize 6-bit DAC 
// Input: none
// Output: none
void DAC_Init(void){unsigned long volatile delay;
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // activate port B
  delay = SYSCTL_RCGC2_R;    // allow time to finish activating
  GPIO_PORTB_AMSEL_R &= ~0x3F;      // no analog 
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // regular function
  GPIO_PORTB_DIR_R |= 0x00;      // make PB5-0 out
  GPIO_PORTB_AFSEL_R &= ~0x3F;   // disable alt funct on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;      // enable digital I/O on PB5-0
  GPIO_PORTB_DR8R_R |= 0x3F;        // enable 8 mA drive on PB5-0
}

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Input: interrupt period
//        Units of period are 12.5ns
//        Maximum is 2^24-1
//        Minimum is determined by lenght of ISR
// Output: none
void Sound_Init(unsigned long period){
  Index = 0;
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1      
  NVIC_ST_CTRL_R = 0x0007;  // enable SysTick with core clock and interrupts
}

void Sound_stop(void)
{
	  NVIC_ST_CTRL_R = 0;
}

// **************DAC_Out*********************
// output to DAC
// Input: 6-bit data, 0 to 7 
// Output: none
void DAC_Out(unsigned long data){
  GPIO_PORTB_DATA_R = data;
}

// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){
  GPIO_PORTF_DATA_R ^= 0x08;     // toggle PF3, debugging
  //Index = (Index+1)&0x0F;        // 16 samples for each period
	Index = (Index+1)%64;
	GPIO_PORTB_DATA_R = SineWave[Index]; // output to DAC: 3-bit data  //4 10k 2 5k
}
