  // private library - BSP layer
#ifdef __MSP430FG4619__
#include "../header/bsp_msp430x4xx.h"
#define LabKit
#else
#include "../header/bsp_msp430x2xx.h"
#define PersonalEvalKit
#endif
//-----------------------------------------------------------------------------  
//           GPIO congiguration
//-----------------------------------------------------------------------------



void GPIOconfig(void){
 // volatile unsigned int i; // in case of while loop usage
  
  WDTCTL = WDTHOLD | WDTPW;		// Stop WDT
  pwm_engine_init();
  trigger_and_echo_legs_config();
  ///
}


void timer_trigger_and_echo_config (void) //  4 ms timer
{
   TACTL = TASSEL_2 + MC_0;
   TBCCTL3 |= OUTMOD_7;   // FOR TRIGGER
   TACCTL2 |= CAP  + CM_3; // FOR ECHO CAPTURE

}

void trigger_and_echo_legs_config(void){
    P3SEL |= 0X10;  // P3.4 FOR TRIGGER
    P3DIR |= 0X10;
    P2SEL |= 0X01; // P2.0 FOR ECHO
    P2DIR &= ~0X01;

}

void pwm_engine_init(void){
    P2DIR |= 0X04;  // P2.2 CONFIG TO  output PWM
    P2SEL |= 0X04;
    TBCCTL1 = OUTMOD_7; // MODE 7
    TBCTL |= TBSSEL_2;
    TBCTL  &= 0Xffcf;
}

void UART_CONFIG(){
      volatile unsigned int i;

      WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
      FLL_CTL0 |= XCAP14PF;                     // Configure load caps

      do
      {
      IFG1 &= ~OFIFG;                           // Clear OSCFault flag
      for (i = 0x47FF; i > 0; i--);             // Time for flag to set
      }
      while ((IFG1 & OFIFG));                   // OSCFault flag still set?

      P4SEL |= 0x03;                            // P4.1,0 = USART1 TXD/RXD
      ME2 |= UTXE1 + URXE1;                     // Enable USART1 TXD/RXD
      U1CTL |= CHAR;                            // 8-bit character
      U1TCTL |= SSEL0;                          // UCLK = ACLK
      U1BR0 = 0x03;                             // 32k/9600 - 3.41
      U1BR1 = 0x00;                             //
      U1MCTL = 0x4A;                            // Modulation
      U1CTL &= ~SWRST;                          // Initialize USART state machine
      IE2 |= URXIE1;                            // Enable USART1 RX interrupt
}





