#include  "../header/halGPIO.h"     // private library - HAL layer
#include  "../header/LCD.h"
#include "stdio.h"



int delay_count = 6, count =0;
unsigned int val0=0 , val1=0;
int flag = 0;
int mask_dist;


//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void){ 
	GPIOconfig();
	timer_trigger_and_echo_config ();
    UART_CONFIG();
	lcd_init();
	lcd_clear();


}

void sleep_gie(){
    __bis_SR_register(LPM0_bits + GIE);
}
//--------------------------------------------------------------------
//             Timers functions
//--------------------------------------------------------------------

//---------------------------------------------------------------------
//            Polling based Delay function
//---------------------------------------------------------------------
void delay(unsigned int t){  // t[micro sec]
	volatile unsigned int i;
	for(i=t; i>0; i--);
}

void delayInMs(int t){ // t[m sec]
    volatile unsigned int i;
   for(i=10;i>0;i--) delay(t*10);
}

//---------------------------------------------------------------------
//            Enter from LPM0 mode
//---------------------------------------------------------------------
void enterLPM(unsigned char LPM_level){
	if (LPM_level == 0x00) 
	  _BIS_SR(LPM0_bits);     /* Enter Low Power Mode 0 */
        else if(LPM_level == 0x01) 
	  _BIS_SR(LPM1_bits);     /* Enter Low Power Mode 1 */
        else if(LPM_level == 0x02) 
	  _BIS_SR(LPM2_bits);     /* Enter Low Power Mode 2 */
	else if(LPM_level == 0x03) 
	  _BIS_SR(LPM3_bits);     /* Enter Low Power Mode 3 */
        else if(LPM_level == 0x04) 
	  _BIS_SR(LPM4_bits);     /* Enter Low Power Mode 4 */
}
//---------------------------------------------------------------------
//            Enable interrupts
//---------------------------------------------------------------------
void enable_interrupts(){
    __bis_SR_register(GIE);
}
//---------------------------------------------------------------------
//            Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
    __bic_SR_register(GIE);
}


//----------------------- resrt state 2-------------------------



//---------------------------------------------------------------------
//  LCD
//---------------------------------------------------------------------


//******************************************************************
// send a command to the LCD
//******************************************************************
void lcd_cmd(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    if (LCD_MODE == FOURBIT_MODE)
    {
        LCD_DATA_WRITE &= ~OUTPUT_DATA;// clear bits before new write
                LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();
                LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= (c & (0x0F)) << LCD_DATA_OFFSET;
        lcd_strobe();
    }
    else
    {
        LCD_DATA_WRITE = c;
        lcd_strobe();
    }
}
//******************************************************************
// send data to the LCD
//******************************************************************
void lcd_data(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_RS(1);
    if (LCD_MODE == FOURBIT_MODE)
    {
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
                LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();
                LCD_DATA_WRITE &= (0xF0 << LCD_DATA_OFFSET) | (0xF0 >> 8 - LCD_DATA_OFFSET);
                LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= (c & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();
    }
    else
    {
        LCD_DATA_WRITE = c;
        lcd_strobe();
    }

    LCD_RS(0);
}

//******************************************************************
// initialize the LCD
//******************************************************************
void lcd_init(){

    char init_value;

    if (LCD_MODE == FOURBIT_MODE) init_value = 0x3 << LCD_DATA_OFFSET;
        else init_value = 0x3F;

    LCD_RS_DIR(OUTPUT_PIN);
    LCD_EN_DIR(OUTPUT_PIN);
    LCD_RW_DIR(OUTPUT_PIN);
        LCD_DATA_DIR |= OUTPUT_DATA;
        LCD_RS(0);
    LCD_EN(0);
    LCD_RW(0);

    DelayMs(15);
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayMs(5);
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayUs(200);
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();

    if (LCD_MODE == FOURBIT_MODE){
        LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h
                LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= 0x2 << LCD_DATA_OFFSET; // Set 4-bit mode
        lcd_strobe();
        lcd_cmd(0x28); // Function Set
    }
        else lcd_cmd(0x3C); // 8bit,two lines,5x10 dots

    lcd_cmd(0x0F); //Display On, Cursor On, Cursor Blink
    lcd_cmd(0x01); //Display Clear
    lcd_cmd(0x6); //Entry Mode
    lcd_cmd(0x80); //Initialize DDRAM address to zero
    lcd_clear();
}
//******************************************************************
// Delay usec functions
//******************************************************************
void DelayUs(unsigned int cnt){

    unsigned char i;
        for(i=cnt ; i>0 ; i--) asm("nop"); // tha command asm("nop") takes raphly 1usec

}
//******************************************************************
// Delay msec functions
//******************************************************************
void DelayMs(unsigned int cnt){

    unsigned char i;
        for(i=cnt ; i>0 ; i--) DelayUs(1000); // tha command asm("nop") takes raphly 1usec

}
//******************************************************************
// lcd strobe functions
//******************************************************************
void lcd_strobe(){
  LCD_EN(1);
  asm("NOP");
  asm("Nope");    // DIDNT COMPILE
  LCD_EN(0);
}



//******************************************************************
//timer functions
//******************************************************************
void start_timer(){
    TACTL = TASSEL_2 + MC_2 +ID_3+TAIE;
}

void stop_timer(){
    TACTL = TASSEL_2 + MC_0 +ID_3+TACLR;
    TACTL &=~TAIE;
}
//******************************************************************
//DMA functions
//******************************************************************
void DMA_start(int *arr){
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    DMACTL0 = DMA0TSEL_2;                     // CCR2 trigger
    DMA0SA = (void (*)())arr;                  // Source block address
    DMA0DA = (void (*)())&P9OUT;                     // Destination single address
    DMA0SZ = 0x10;                            // Block size
    DMA0CTL = DMADT_4 + DMASRCINCR_3 + DMASBDB + DMAEN+DMAIE; // Rpt, inc src
    TBCTL = TBSSEL_2 + MC_2 +ID_2;                  // SMCLK/4, contmode, half a second
}

void DMA_stop(){
    DMA0CTL &= ~DMAEN + ~DMAIE;
    P9OUT &= ~0xFF;
    TBCTL &=MC_0;
}

void transferBlock(char * addr_src, char * adrr_dst, int blk_sz){
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    DMA1SA = (void (*)( ))addr_src;            // Start block address
    DMA1DA = (void (*)( ))adrr_dst;            // Destination block address
    DMA1SZ = blk_sz;                          // Block size
    DMA1CTL = DMADT_1 + DMASRCINCR_3 + DMADSTINCR_3 + DMASRCBYTE + DMADSTBYTE; // Rpt, inc
    DMA1CTL |= DMAEN;                         // Enable DMA0
    DMA1CTL |= DMAREQ;                        // triger DMA
}

void DMA_ST4_start(char * addr_src, char * adrr_dst, int blk_sz){
       WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
       DMA2SA = (void (*)( ))addr_src;            // Start block address
       DMA2DA = (void (*)( ))adrr_dst;            // Destination block address
       DMA2SZ = blk_sz;                          // Block size
       DMA2CTL = DMADT_1 + DMASRCINCR_2 + DMADSTINCR_3 + DMASRCBYTE + DMADSTBYTE; // Rpt, inc
       DMA2CTL |= DMAEN;                         // Enable DMA0
       DMA2CTL |= DMAREQ;                        // triger DMA
}



//*********************************************************************
//            Port1 Interrupt Service Rotine
//*********************************************************************
#pragma vector=PORT1_VECTOR
  __interrupt void PBs_handler(void){
   
      delay(debounceVal);
//---------------------------------------------------------------------
//            selector of transition between states
//---------------------------------------------------------------------

	if(PBsArrIntPend & PB0){
	  state = state1;
	  PBsArrIntPend &= ~PB0;
        }
      else if(PBsArrIntPend & PB1){

      state = state2;
      PBsArrIntPend &= ~PB1;
      }
	else if(PBsArrIntPend & PB2){ 
	  state = state3;
	  PBsArrIntPend &= ~PB2;
        }
    else if(PBsArrIntPend & PB3){
    state = state4;
      PBsArrIntPend &= ~PB3;
        }



//---------------------------------------------------------------------
//            Exit from a given LPM
//---------------------------------------------------------------------
	            switch(lpm_mode){
	            case mode0:
	             LPM0_EXIT; // must be called from ISR only
	             break;

	            case mode1:
	             LPM1_EXIT; // must be called from ISR only
	             break;

	            case mode2:
	             LPM2_EXIT; // must be called from ISR only
	             break;

	                    case mode3:
	             LPM3_EXIT; // must be called from ISR only
	             break;

	                    case mode4:
	             LPM4_EXIT; // must be called from ISR only
	             break;
	        }
}
 







#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=DMA_VECTOR
  __interrupt void DMA0_handler(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(DMA_VECTOR))) DMA0_handler (void)
#else
#error Compiler not supported!
#endif
  {
      switch(DMAIV){
      case 0x02:

      }

  }


  void start_timer_4m(void) {
      TACTL |= MC_1 ;
      TACCTL0 |= CCIE;
  }

  void stop_timer_4m(void) {
      TACTL &= MC_0 + TASSEL_2;
      TACCTL0 &= ~CCIE;
  }


  void start_timer_pwm_engine(){
      TBCCTL1  = OUTMOD_7;
      TBCTL |= MC_1; // START UP MODE
      TBCCTL0 |= CCIE; // FOR DELAY COUNTING

  }

/*
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_B1_VECTOR
__interrupt void TIMER_B_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_B1_VECTOR))) TIMER_B_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(TBIV, 0x0A))
  {
      case  TBIV_TBCCR1:                   // Vector  2:  TACCR1 CCIFG

        break;
      case TBIV_TBCCR2:                    //   TACCR2 CCIFG

      if(TBCCR1 <= 0xa3d){
          TBCCR1 += 0X04;   // NEXT 9 DEGREE
      //    TBCCR2 += 0X1060; // UPDATE 4 MS COMPARE
      }
      else
      {  LPM0_EXIT;
          TBCCR2 = 0X1060;
      }

      break;

      default:  break;
  }
}
*/



#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMER1_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A1_VECTOR))) TIMER1_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(TAIV, 0x0A))
  {
      case  TAIV_TACCR1:                   // Vector  2:  TACCR1 CCIFG

        break;
      case TAIV_TACCR2:                    // Vector  4:  TACCR2 CCIFG

                     if(flag){
                          val1 = TACCR2;
                          flag ^= 1;
                         TACCTL2 &= ~CCIE;
                         TAR = 0X00;
                         TACTL &= MC0;
                         LPM0_EXIT;

                      }

                      else
                      {
                          val0 = TACCR2;
                          flag ^= 1;
                      }

          break;
      default:  break;
  }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector =TIMER0_B0_VECTOR
__interrupt void TIMER_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_B0_VECTOR))) TIMER_B0_ISR(void)
#else
#error Compiler not supported!
#endif
{
    if(count == delay_count){
        LPM0_EXIT;
        count = 0;
    }

    else
    {
        count ++;
    }
}


void start_ultra_trigger() {
    TACTL |= MC_2;
}

void stop_ultra_trigger(){
    TACTL &= ~MC_0;
}

void start_capture_echo(){
    TACTL |= MC_2;
    TACCTL2 |= CCIE;
}


int calc_dis(){
    return (val1 - val0);
}

void send_dist_and_angle(int distance){

}


#pragma vector=USART1RX_VECTOR
__interrupt void USART1_rx (void)
{
  static char rcv_data = 0;
  static char state1_LSB_Byte = 0;

  if(rcv_data == 0){
   switch(RXBUF1){
   case '1':
       state = state1;
       rcv_data =1;
       LPM0_EXIT;
   break;
   case '2':
       state = state2;
       rcv_data =1;
       LPM0_EXIT;
   break;
   case '3':
       state = state3;
       rcv_data =1;
       LPM0_EXIT;
   break;
   case '4':
       state = state4;
       rcv_data = 1;
       LPM0_EXIT;

   }
  }

  else{
         switch(state){
         case state1:
             if (state1_LSB_Byte == 0){    // Reciving MSB of mask distance
                 mask_dist = RXBUF1;
                 mask_dist << 8;
                 state1_LSB_Byte = 1;
             }

             else {
                 mask_dist |= RXBUF1; // Reciving LSB of mask distance
                 state1_LSB_Byte = 0;
                 rcv_data = 0;
                 LPM0_EXIT;
             }
      }

  }

}











