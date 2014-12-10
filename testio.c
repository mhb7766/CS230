#include <msp430.h>
#include <libemb/serial/serial.h>
#include <libemb/conio/conio.h>
#include <libemb/shell/shell.h>

/******
  *
  *    PROTOTYPES
  *
  ******/
int shell_cmd_help(shell_cmd_args *args);
int shell_cmd_argt(shell_cmd_args *args);
void setup(void);
void digitCtrl(void);
void count(void);
void show(int, int, int, int);

/******
  *
  *    CONSTANTS
  *
  ******/
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


/******
  *
  *    GLOBALS
  *
  ******/
int display[10] = {0};


/******
  *
  *    SHELL COMMANDS STRUCT
  *
  ******/
shell_cmds my_shell_cmds = {
   .count = 2,
   .cmds  = {
     {
       .cmd  = "help",
       .desc = "list available commands",
       .func = shell_cmd_help
     },
     {
       .cmd  = "args",
       .desc = "print back given arguments",
       .func = shell_cmd_argt
     }
   }
};

/******
  *
  *    CALLBACK HANDLERS
  *
  ******/
int shell_cmd_help(shell_cmd_args *args)
{
   int k;

   for(k = 0; k < my_shell_cmds.count; k++) {
     cio_printf("%s: %s\n\r", my_shell_cmds.cmds[k].cmd, 
my_shell_cmds.cmds[k].desc);
   }

   return 0;
}

int shell_cmd_argt(shell_cmd_args *args)
{
   int k;

   cio_print((char *)"args given:\n\r");

   for(k = 0; k < args->count; k++) {
     cio_printf(" - %s\n\r", args->args[k].val);
   }

   return 0;
}

int shell_process(char *cmd_line)
{
   return shell_process_cmds(&my_shell_cmds, cmd_line);
}

/******
  *
  *    INITIALIZATION
  *
  ******/
int main(void)
{

  WDTCTL  = WDTPW | WDTHOLD;
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL  = CALDCO_1MHZ;

  P1DIR = BIT0;
  P1REN = BIT3;
  P1OUT  |= BIT3;
  P1IE   |= BIT3;
  P1IES  |= BIT3;
  P1IFG  &= ~BIT3;  

   serial_init(9600);                        // Initialize Serial Coms
   __enable_interrupt();                     // Enable Global Interrupts

  P1REN  = BIT3;    //enable pullup resistor for p1.3
  P1OUT  = BIT3;    //set output for p1.3 high
  P1IE  |= BIT3;    //register an interrupt enabler for p1.3
  P1IES |= BIT3;    //edge select to high.. indicates high to low edge
  P1IFG &= ~BIT3;   //clear interrupt flag

  TA1CCR0  = 1000; //500;
  BCSCTL3 = LFXT1S_2; // LFXT1S_2 sets bits 
                  // in Clock System to 
                  // 'source'VLO for TASSEL_1
  TA1CTL = TASSEL_1 | MC_1 | ID_1;
  TA1CCTL0 = CCIE; 
/******
  *
  *    PROGRAM LOOP
  *
  ******/
   for (;;) {
     // __delay_cycles(4000);
     int j = 0;                              // Char array counter
     char cmd_line[90] = {0};                // Init empty array

     cio_print((char *) "$ ");               // Display prompt
     char c = cio_getc();                    // Wait for a character
     while(c != '\r') {                      // until return sent then ...
       if(c == 0x08) {                       //  was it the delete key?
         if(j != 0) {                        //  cursor NOT at start?
           cmd_line[--j] = 0;                //  delete key logic
           cio_printc(0x08); cio_printc(' '); cio_printc(0x08);
         }
       } else {                              // otherwise ...
         cmd_line[j++] = c; cio_printc(c);   //  echo received char
       }
       c = cio_getc();                       // Wait for another
     }

     cio_print((char *) "\n\n\r");           // Delimit command result

     switch(shell_process(cmd_line))         // Execute specified shell command
     {                                       // and handle any errors
       case SHELL_PROCESS_ERR_CMD_UNKN:
         cio_print((char *) "ERROR, unknown command given\n\r");
         break;
       case SHELL_PROCESS_ERR_ARGS_LEN:
         cio_print((char *) "ERROR, an arguement is too lengthy\n\r");
         break;
       case SHELL_PROCESS_ERR_ARGS_MAX:
         cio_print((char *) "ERROR, too many arguements given\n\r");
         break;
       default:
         break;
     }

     cio_print((char *) "\n");               // Delimit before prompt
   }

   return 0;
}

/******
  *
  * FUNCTIONS
  *
  ******/


/******
  *
  *    INTERRUPTS
  *
  ******/
//timer interrupt to display digits when timer resets
#pragma vector=TIMER1_A0_VECTOR             // TA1 CCR0 Interrupt
__interrupt void Timer1_A0 (void) 
{       
    cio_print("timer fired\n\r");
    cio_printf("%s\n\r", TAIV);
}

#pragma vector =PORT1_VECTOR
    __interrupt void Port_1(void) {

    cio_print("interrupt fired");
    cio_printf("%s\n\r", TAIV);
    P1OUT ^= BIT0;
    while (!(BIT3 & P1IN)) {}
    __delay_cycles(32000);
    P1IFG &= ~BIT3;
}
