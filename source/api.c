#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer
#include  "../header/LCD.h"
#include "stdio.h"
#define ASCII_OFFSET 48
#define maxTBR  0xFFFF
#define subFreq  1048576


enum FSMstate state;
enum SYSmode lpm_mode;
int distance;
//******************************************************************
// write a string of chars to the LCD
//******************************************************************
void lcd_puts(const char * s){

    while(*s)
        lcd_data(*s++);
}


 void set_angle(int degree_reg){

     __bis_SR_register(LPM0_bits + GIE); // waiting for mask_disk

     TBCCR0 = 0X6666;
     TBCCR1 = degree_reg;
     while(TBCCR1 < 0xa3d){
     start_timer_pwm_engine();   //also starts delay
     __bis_SR_register(LPM0_bits + GIE);
     TBCCR1 += 0X14;
     __delay_cycles(50000);
     TBCCR3 =0X3333;
     start_ultra_trigger();
     __delay_cycles(10);
     start_capture_echo();
     __bis_SR_register(LPM0_bits + GIE);
     distance = calc_dis();

     //send_dist_and_angle(distance);
 //    stop_ultra_trigger();
     }
 }

