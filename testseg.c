#include "msp430.h"

#define ZERO    0b00111111;
#define ONE     0b00000110;
#define TWO     0b01011011;
#define THREE   0b01001111;
#define FOUR    0b01100110;
#define FIVE    0b01101101;
#define SIX     0b01111101;
#define SEVEN   0b00000111;
#define EIGHT   0b01111111;
#define NINE    0b01100111;

int i, j, k, l;
int display[10] = {0};

void setup(void);
void count(void);
void show(int, int, int, int);

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;           // Stop watchdog timer
  BCSCTL1   = CALBC1_1MHZ;                  // Run @ 1MHz
  DCOCTL    = CALDCO_1MHZ;
  P2SEL &= ~(BIT6 | BIT7);          // clear XIN, XOUT so they can be used as output

  __enable_interrupt();

  P2DIR |= 0xFF;      // Set P2.# to output direction3
  P1DIR |= 0xFF;
  P1OUT &= ~0b11110000;             // Set P1.4-P1.7 to ground

  //setup timer interrupt .
  TA1CCR0  = 1000; //500;
  BCSCTL3 = LFXT1S_2; // LFXT1S_2 sets bits 
                  // in Clock System to 
                  // 'source'VLO for TASSEL_1
  TA1CTL = TASSEL_1 | MC_1 | ID_1;
  serial_init(9600);
  TA1CCTL0 = CCIE;  

  
  //while(1)__delay_cycles(10);
  setup();
  for (;;){
    count();
  }
} // main

//initialize the array
void setup(){
  display[0] = ZERO;
  display[1] = ONE;
  display[2] = TWO;
  display[3] = THREE;
  display[4] = FOUR;
  display[5] = FIVE;
  display[6] = SIX;
  display[7] = SEVEN;
  display[8] = EIGHT;
  display[9] = NINE;
}

// uses for loops to run the timer
// calls show function to update the display
void count(){

  for (i=0; i<10; i++){
    for (j=0; j<10; j++){
      for (k=0; k<10; k++){
        for (l=0; l<10; l++){
        }
      }
    }
  } 
}
//outputs the current timer position to the display
void show(int a, int b, int c, int d){
  int i;
  for (i=0;i<100;i++){
    P1OUT = 0b01110000;
    P2OUT = display[a];
    P2OUT &= ~(display[a]);
    P1OUT = 0b10110000;
    P2OUT = display[b];
    P2OUT &= ~(display[b]);
    P1OUT = 0b11010000;
    P2OUT = display[c];
    P2OUT &= ~(display[c]);
    P1OUT = 0b11100000;
    P2OUT = display[d];
    P2OUT &= ~(display[d]);
  }
}

//timer interrupt to display digits when timer resets
#pragma vector=TIMER1_A0_VECTOR             // TA1 CCR0 Interrupt
__interrupt void Timer1_A0 (void) 
{       show(l,k,j,i);
        //cio_printf("%d | %d | %d | %d\n\r",i,j,k,l);
}

