#include <msp430.h>
#include <libemb/serial/serial.h>
#include <libemb/conio/conio.h>
#include <libemb/shell/shell.h>

int v=0;

int main()
{ 
	WDTCTL = WDTPW | WDTHOLD;
    TA1CCTL0 = CCIE;    

    TA1CCR0  = 1000;//500;
    TA1CTL = TASSEL_1 | MC_1 | ID_1;
    serial_init(9600);
    __enable_interrupt();
    while(42 );
}

#pragma vector=TIMER1_A0_VECTOR             // TA1 CCR0 Interrupt
__interrupt void Timer1_A0 (void)
{v=v+1;
         cio_printf("%u\n",v);
}