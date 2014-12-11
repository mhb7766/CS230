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
int shell_cmd_play(shell_cmd_args *args);
void setup();
void count();
void show();
void enableTA1();
void enableButInt();
void freeze();
void showResults();
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
int clock0_flag = 0; //** flags for
int clock1_flag = 0; //** main to handle 
int button_flag = 0; //*** interrupts

int w, x, y, z;      // counts digits
int guess[4]  = {0}; // stores user's guess
int result[4] = {0}; // store result of spin


/******
 *
 *    SHELL COMMANDS STRUCT
 *
 ******/
shell_cmds my_shell_cmds = {
  .count = 3,
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
    },
    {
      .cmd  = "play",
      .desc = "enter your four digit guess to play",
      .func =  shell_cmd_play
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

int shell_cmd_play(shell_cmd_args *args)
{
  int k;

  if(args->count > 4){
    cio_printf("error, please reset the board\n\r");
    for(;;) {}
  }

  for(k = 0; k < args->count; k++){
    guess[k] = atoi(args->args[k].val);
  }

  cio_print((char *)"You guessed:   ");
  for(k = 0; k < args->count; k++) {
    cio_printf("%s ", args->args[k].val);
  }
  cio_print("\r\n");

  enableTA1();
  enableButInt();

  for (;;){
    count();
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
  WDTCTL    = WDTPW + WDTHOLD;              // Disable Watchdog
  BCSCTL1   = CALBC1_1MHZ;                  // Run @ 1MHz
  DCOCTL    = CALDCO_1MHZ;
  BCSCTL3   = LFXT1S_2;                     // Source VLO for ACLK
  P2SEL    &= ~(BIT6 | BIT7);               // P2.6 and P2.7 as Outputs
  P1OUT     = 0;
  P2OUT     = 0; // start game with lights off

  P2DIR |= 0xFF;                    // Set P2 to output direction
  P1DIR |= 0xFF;                    // Set P1 to output
  P1OUT &= ~0b11110000;             // Set P1.4-P1.7 to ground

  setup();                          // initialize display array
  serial_init(9600);                        // Initialize Serial Coms
  __enable_interrupt();                     // Enable Global Interrupts

  //small welcome menu
  cio_print("\r\nWelcome to the lotto game\r\n* To get started type 'play' followed by your four guesses (0-9)\r\n");
  cio_print("* Like this: $ play 2 8 9 1\r\n");
  cio_print("* Type help for more info\r\n\r\n");

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

void enableButInt(){
  P1DIR  = BIT0;
  P1REN  = BIT3;
  P1OUT |= BIT3; //set initial state to high - 1
  P1IE  |= BIT3;
  P1IES |= BIT3;
  P1IFG &= ~BIT3;
}

void enableTA1(){
  TA1CCR0  = 1000; //500;
  BCSCTL3 = LFXT1S_2; // LFXT1S_2 sets bits 
                  // in Clock System to 
                  // 'source'VLO for TASSEL_1
  TA1CTL = TASSEL_1 | MC_1 | ID_1;
  TA1CCTL0 = CCIE; 
}

// uses for loops to run the timer
// calls show function to update the display
void count(){

  for (w=0; w<10; w++){
    for (x=0; x<10; x++){
      for (y=0; y<10; y++){
        for (z=0; z<10; z++){
          if(button_flag == 1){
            freeze();
            for(;;){show();}    //display the final lotto numbers
          }
          if(button_flag == 0){
            if (clock1_flag == 1){
              show();      //show the digits on the display if timer1 interrupt fired
              clock1_flag = 0;    //reset clock1 flag
              //enableButInt();
            }
          }
        }
      }
    }
  } 
}

// stops the count, displays what the final digits were
void freeze(){
  int k;
  int nCorrect = 0;

  cio_printf("Lotto Numbers: %u %u %u %u \r\n", w, x, y, z);

  // loop checks for correct guesses... 
  for(k=0; k<4; k++){
    if(w - guess[k] == 0){
      nCorrect++; 
      }
    if(x - guess[k] == 0){
      nCorrect++;
      }
    if(y - guess[k] == 0){
      nCorrect++; 
      }
    if(z - guess[k] == 0){
      nCorrect++; 
      }
  }

  //send a message and flash light based on results
  switch( (unsigned int) nCorrect ){
    case 0 :
      cio_print(">> No matches, you lose... maybe next time\r\n");
      break;
    case 1 :
      cio_print(">> Only 1 match?!?! C'mon!!\r\n");
      flash_red((unsigned int) nCorrect);
      break;
    case 2 :
      cio_print(">> 2 matches... meh\r\n");
      flash_red((unsigned int) nCorrect);
      break;
    case 3 :
      cio_print(">> Good Job, 3 matches\r\n");
      flash_red((unsigned int) nCorrect);
      break;
    case 4 :
      cio_print(">> Jackpot! You matched 4 numbers\r\n");
      flash_red((unsigned int) 10);
      break;
    default :
      cio_print(">> Nice Playing, you matched more than four\r\n");
  } 

}

void flash_red(unsigned int n){
  int i;
  P1DIR |= BIT0;
  __delay_cycles(384000);

  for(i=0; i<n; i++){
    P1OUT |= BIT0;
    __delay_cycles(384000);
    P1OUT ^= BIT0;
    __delay_cycles(384000);
  }
}

//outputs the current timer position to the display
void show(){
  int i;
  P1DIR |= 0xFF;

  for (i=0;i<1000;i++){
    P1OUT = 0b01111000;
    P2OUT = display[z];
    P2OUT &= ~(display[z]);
    P1OUT = 0b10111000;
    P2OUT = display[y];
    P2OUT &= ~(display[y]);
    P1OUT = 0b11011000;
    P2OUT = display[x];
    P2OUT &= ~(display[x]);
    P1OUT = 0b11101000;
    P2OUT = display[w];
    P2OUT &= ~(display[w]);
  }
  enableButInt();       //reset the values for the button interrupt to work
}
/******
 *
 *    INTERRUPTS
 *
 ******/
//timer interrupt to display digits when timer resets
#pragma vector=TIMER1_A0_VECTOR             // TA1 CCR0 Interrupt
__interrupt void Timer1_A0 (void) 
{       
    clock1_flag = 1;
}

#pragma vector =PORT1_VECTOR
    __interrupt void Port_1(void) {
  if(P1IFG & BIT3) {                          //make sure P1.3 did this
   //cio_print("button interrupt fired\r\n");
    button_flag = 1;

    while (!(BIT3 & P1IN)) {}
    __delay_cycles(32000);

    P1IFG &= ~BIT3;     //reset bit
  }

}               
