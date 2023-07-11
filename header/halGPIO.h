#ifndef _halGPIO_H_
#define _halGPIO_H_

#include  "../header/app.h"    		// private library - APP layer
#ifdef __MSP430FG4619__
#include "bsp_msp430x4xx.h"
#else
#include "bsp_msp430x2xx.h"
#endif
//#include "bsp_msp430x4xx.h"    		// private library - BSP layer


extern enum FSMstate state;   // global variable
extern enum SYSmode lpm_mode; // global variable
extern char return_char();
extern void set_char();
extern void sysConfig(void);
extern void delay(unsigned int);
extern void delayHalfSecond();
extern void enterLPM(unsigned char);
extern void enable_interrupts();
extern void disable_interrupts();
extern void delayInMs(int); //delay in ms
extern void exitLpm();
extern char charSelect();
extern void sleep_gie();
extern void DMA_start(int *);
extern void DMA_stop();
extern void start_timer();
extern void stop_timer();
extern char return_flag();
extern void transferBlock(char * addr_src, char * adrr_dst, int blk_sz);
extern void DMA_ST4_start(char * addr_src, char * adrr_dst, int blk_sz);
extern __interrupt void TIMER_B_ISR(void);
extern __interrupt void TIMER_A_ISR(void);
extern __interrupt void USART1_rx (void);
extern void start_timer_pwm_engine();
extern void start_timer_4m();
extern void stop_timer_4m();


#endif







