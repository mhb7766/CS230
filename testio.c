#include <msp430.h>
#include <libemb/serial/serial.h>
#include <libemb/conio/conio.h>
#include <libemb/shell/shell.h>
int v=0;
int main()
{     WDTCTL = WDTPW | WDTHOLD;

    TA1CCR0  = 1000; //500;
    BCSCTL3 = LFXT1S_2; // LFXT1S_2 sets bits 
                    // in Clock System to 
                    // 'source'VLO for TASSEL_1
    TA1CTL = TASSEL_1 | MC_1 | ID_1;
    serial_init(9600);
    TA1CCTL0 = CCIE;  

    
    
    __enable_interrupt();
    while(1)__delay_cycles(10);
}

#pragma vector=TIMER1_A0_VECTOR             // TA1 CCR0 Interrupt
__interrupt void Timer1_A0 (void) 
{        v=v+1;
        cio_printf("%u\n\r",v);
}