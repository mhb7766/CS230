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
#define FOUR    0b1100110;
#define FIVE    0b1101101;
#define SIX     0b1111101;
#define SEVEN   0b0000111;
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
    cio_printf("%s: %s\n\r", my_shell_cmds.cmds[k].cmd, my_shell_cmds.cmds[k].desc);
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

void digitCtrl(){

}

/******
 *
 *    INITIALIZATION
 *
 ******/
int main(void)
{

  WDTCTL    = WDTPW + WDTHOLD;              // Disable Watchdog
  BCSCTL1   = CALBC1_1MHZ;                  // Run @ 1MHz
  DCOCTL    = CALDCO_1MHZ;
  BCSCTL3   = LFXT1S_2;                     // Source VLO for ACLK
  P2SEL    &= ~(BIT6 | BIT7);               // P2.6 and P2.7 as Outputs

  P2DIR |= 0xFF;                    // Set P2 to output direction
  P1DIR |= 0xFF;                    // Set P1 to output
  P1OUT &= ~0b11110000;             // Set P1.4-P1.7 to ground

  setup();
  for(;;){
    count();
  }

  serial_init(9600);                        // Initialize Serial Coms
  __enable_interrupt();                     // Enable Global Interrupts

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
  int i, j, k, l;

  for (i=0; i<10; i++){
    for (j=0; j<10; j++){
      for (k=0; k<10; k++){
        for (l=0; l<10; l++){
          if(l==5){
            show(l,k,j,i);
          }
        }
        show(0, k, j, i);
      }
      show(0,0,j,i);
    }
    show(0,0,0,i);
  } 
  
}
//outputs the current timer position to the display
void show(int a, int b, int c, int d){
  int i;
  for (i=0;i<10;i++){
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
/******
 *
 *    INTERRUPTS
 *
 ******/
#pragma vector=TIMER0_A0_VECTOR             // TA0 CCR0 Interrupt
  __interrupt void Timer0_A0 (void) {
    cio_print("timer fired");               // debugging
  // First Timer Interrupt
}

#pragma vector=TIMER1_A0_VECTOR             // TA1 CCR0 Interrupt
  __interrupt void Timer0_A1 (void) {
  // Second Timer Interrupt
}

#pragma vector=PORT1_VECTOR
__interrupt void Port1_Interrupt (void) {
  // Butter Interrupt
  P1IFG &= ~BIT3;                           // Don't Forget to Clear Flag!
}
