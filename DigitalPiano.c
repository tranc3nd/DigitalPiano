#include "tm4c123gh6pm.h"
#include "Sound.h"
#include "SwitchLed.h"

// initial values for piano major tones: 
// Assume system clock is 16MHz
const unsigned long Tone_Tab[] =
// Note name: C, D, E, F, G, A, B, C'
// Offset:0, 1, 2, 3, 4, 5, 6, 7
{30534*2,27211*2,24242*2,22923*2,20408*2,18182*2,16194*2,15289*2, // C4 Major notes
15289*2,13621*2,12135*2,11454*2,10204*2,9091*2,8099*2,7645*2, // C5 Major notes
7645*2,6810*2,6067*2,5727*2,5102*2,4545,4050*2,3822*2}; // C6 major notes

// Note name: C, D, E, F
// Offset:    0, 1, 2, 3
/*
{262*2,294*2,330*2,349*2, // Lower Frequency Hz
 5233,587,659,698, // Middle Frequency Hz
 1046,1174,1318,1396}; // Upper Frequency Hz
*/


#define NUM_VALs  64  // Assume 3-bit DAC is used, that will give 16 values for one period.
#define NUM_NOTEs 8   // number of notes to be played repeatedly

// basic functions defined at end of startup.s
extern void DisableInterrupts(void); // Disable interrupts
extern void EnableInterrupts(void);  // Enable interrupts
extern void WaitForInterrupt(void);  // low power mode

int main(void){ 
	unsigned char i,j,input,previous; 
  unsigned char tone_index = 0;	
	
  DisableInterrupts();    
  SwitchLed_Init();       // use Port F for onboard switches and LEDs
  DAC_Init();          // Port B is DAC
	Piano_Init();
  EnableInterrupts(); // SysTick uses interrupts
	// Automatic play for all 8 notes
	/*
  for(i=0;i<4;i++){
		EnableInterrupts();
    Sound_Init(Tone_Tab[i]/NUM_VALs);      // Play 8 major notes
		for (j=0;j<50;j++) 
      Delay10ms();  // delay for tempo
    DisableInterrupts();		
  }*/
  
	// Manual play: push SW1 for one note, release and push again moves to the next note.
  //previous = Switch_In()&0x10;
  while(1){ 
		
    //input = Switch_In()&0x10; // bit 4 means SW1 pressed
		//if(note==1) {
		for(i=0;i<4;i++){
			Sound_Init(Tone_Tab[i]/NUM_VALs);
		}
		
		/*}
		else if(note==2) Sound_Init(Tone_Tab[1]/NUM_VALs);
		else if(note==3) Sound_Init(Tone_Tab[2]/NUM_VALs);
		else if(note==4) Sound_Init(Tone_Tab[3]/NUM_VALs);
		*/
		WaitForInterrupt();
		
    
    //if(previous&&(input==0)){ // just released     
    //  DisableInterrupts();    // stop sound
    //}
		/*
    previous = input; 
		*/
    //Delay10ms();  // remove switch bounce    
  }  
}