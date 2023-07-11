#ifndef _bsp_H_4xx_
#define _bsp_H_4xx_

#include  <msp430xG46x.h>  // MSP430x4xx    (MSP430FG4619)


#define   debounceVal      250

// PushButtons abstraction
#define PBsArrPort     P1IN
#define PBsArrIntPend      P1IFG
#define PBsArrIntEn    P1IE
#define PBsArrIntEdgeSel   P1IES
#define PBsArrPortSel      P1SEL
#define PBsArrPortDir      P1DIR
#define PB0                0x01
#define PB1                0x02
#define PB2                0x04
#define PB3                0x08




//LCD
#define LCD_DATA_WRITE  P1OUT
#define LCD_DATA_DIR    P1DIR
#define LCD_DATA_READ   P1IN
#define LcdCtrl             P2OUT
#define LcdCtrlDir          P2DIR
#define LcdCtrl             P2OUT
#define LCD_CTL_SEL         P2SEL
#define LCD_DATA_SEL      P1SEL


//kKEYPAD
#define KEYPAD_OUT  P10OUT
#define KEYPAD_IN   P10IN
#define KEYPAD_DIR   P10DIR
#define KEYPAD_SEL   P10SEL
#define KEYPAD_IRQ  P2IN
#define KEYPAD_IRQ_DIR  P2DIR
#define KEYPAD_IRQ_SEL  P2SEL
#define KEYPAD_IRQ_IFG  P2IFG
#define KEYPAD_IRQ_IE  P2IE
#define KEYPAD_IRQ_IES  P2IES
#define KEYPAD_IRQ_INDEX 0x02

extern void Row4_search();
extern void Row3_search();
extern void Row2_search();
extern void Row1_search();
extern void Reset_Keypad();
// timers

#define TIMER_A_VEC TIMERA1_VECTOR;

///LED
#define LEDSOUT P9OUT
#define LEDSSEL P9SEL
#define LEDSDIR P9DIR
///



extern void GPIOconfig(void);
extern void TIMERconfig(void);
extern void pwm_engine_init(void);
extern void timer_4m_config (void);
extern void UART_CONFIG();



#endif





