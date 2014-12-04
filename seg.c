#include <msp430.h>

unsigned int counter = 0;                   // Counter variable
unsigned int digitCounter = 0;            // Digit counter
unsigned char digit = 0;               // Single digit to be displayed
unsigned char bcd7digit[10] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F}; // BCD to 7 digit map
unsigned char digitSelector[4] = {0x01, 0x02, 0x04, 0x08}; // Digit selector map

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

  P1OUT |= 0x01;                      // Port P1.0 will be used to latch
  P1DIR |= 0x01;

  CCTL0 = CCIE;                             // CCR0 interrupt enabled
  CCR0 = 500;                        //
  TACTL = TASSEL_2 + MC_1 + ID_3;           // SMCLK, upmode

  _bis_SR_register(LPM0_bits + GIE);        // Enter LPM0 w/ interrupt
}

// Timer A0 interrupt service routine
#pragma vector = TIMERA0_VECTOR
__interrupt void Timer_A (void)
{   
  digitCounter++;                       // Increase digit counter
  digitCounter &= 0x03;                // Mask, counter range is 0-3
  digit = counter>>(4 * digitCounter);       // Shift digits right
  digit &= 0x0F;                      // Mask, we need first digit only
  USISRL = bcd7digit[digit];             // Get segments from the map
  USISRH = digitSelector[digitCounter];    //
  if(digitCounter == 0) {
     counter = _bcd_add_short(counter, 0x01);// Decimally increase counter's when on first digit
  }
  USICNT |= 16;                              // Start USI
}

// USI interrupt service routine
#pragma vector = USI_VECTOR
__interrupt void USI_TXRX (void)
{
  USICTL1 &= ~USIIFG;                     // Clear pending flag
  P1OUT &= ~0x01;                     // Latch data
  P1OUT |= 0x01;
}