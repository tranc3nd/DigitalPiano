// SwitchLed.c
// This software configures the on-board switches and LEDs.
//
// For use with the LM4F120 or TM4C123
// September 15, 2013
// Port B bits 2-0 have the 3-bit DAC
// Port F is onboard LaunchPad switches and LED
// Port F bit 4 is negative logic switch to play sound, SW1
// SysTick ISR: PF3 ISR heartbeat

#include "tm4c123gh6pm.h"

#define NVIC_EN0_PORTD 	0x00000008     // enable interrupt 3 in NVIC

int note;

//---------------------Switch_Init---------------------
// initialize switch interface
// Input: none
// Output: none 
void SwitchLed_Init(void){ volatile unsigned long  delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R = 0x11;          // 5) PF4,PF0 in, PF3-1 out
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
  GPIO_PORTF_PUR_R = 0x11;          // enable pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R = 0x11;          // 7) enable digital I/O on PF4-0
}

void Piano_Init(void){ volatile unsigned long  delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOD; // activate clock for port F
	delay = SYSCTL_RCGC2_R;
	//GPIO_PORTF_LOCK_R = 0x4C4F434B; 			// unlock GPIO Port F
	GPIO_PORTD_CR_R |= 0x0F;      				// allow changes for PF4,PF0 take effect 
  GPIO_PORTD_AMSEL_R &= ~0x0F;  				// disable analog functionality on PF4,PF0
  GPIO_PORTD_PCTL_R &= ~0x0000FFFF; 		// configure PF4, PF0 as GPIO
	GPIO_PORTD_DIR_R &= ~0x0F;    				// make PF4,0 in (built-in button)
  GPIO_PORTD_DEN_R |= 0x0F;     				// enable digital I/O on PF4, PF0
  GPIO_PORTD_AFSEL_R &= ~0x0F;  				// disable alt funct on PF4, PF0
  GPIO_PORTD_PUR_R |= 0x00;     				// enable weak pull-up on PF4,PF0
  GPIO_PORTD_IS_R &= ~0x0F;     				// PF4,PF0 is edge-sensitive
  GPIO_PORTD_IBE_R &= ~0x0F;    				// PF4,PF0 is not both edges
  GPIO_PORTD_IEV_R &= ~0x0F;    				// PF4,PF0 falling edge event
  GPIO_PORTD_ICR_R = 0x0F;      				// clear flags 4,0
  GPIO_PORTD_IM_R |= 0x0F;      				// arm interrupt on PF4,PF0
	NVIC_PRI0_R = (NVIC_PRI0_R&0x1FFFFFFF)|0xA0000000; // bits:31-29 for PORTD, set priority to 5
  NVIC_EN0_R |= NVIC_EN0_PORTD;      		// enable interrupt 30 in NVIC
}

//---------------------Switch_In---------------------
// read the values of the two switches
// Input: none
// Output: 0x00,0x01,0x10,0x11 from the two switches
//         0 if no switch is pressed
// bit4 PF4 SW1 switch
// bit0 PF0 SW2 switch 
unsigned long Switch_In(void){ 
  return (GPIO_PORTF_DATA_R&0x11)^0x11;
}

unsigned long Piano_Button(void){
	return (GPIO_PORTD_DATA_R&0x0F)^0x0F;
}

// ISR for PORTD
void GPIOPortD_Handler(void){
		if(GPIO_PORTD_RIS_R&0x01) {  // SW2 touched - stops current song and plays next song from the begining
		GPIO_PORTD_ICR_R = 0x01;  // acknowledge flag0
		note = 1;
    }
}
//---------------------Delay10ms---------------------
// wait 10ms for switches to stop bouncing
// Input: none
// Output: none
void Delay10ms(void){unsigned long volatile time;
  time = 14545;  // 10msec
  while(time){
		time--;
  }
}

