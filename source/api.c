#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer
#include  "../header/LCD.h"
#include "stdio.h"
#define ASCII_OFFSET 48
#define maxTBR  0xFFFF
#define subFreq  1048576


enum FSMstate state;
enum SYSmode lpm_mode;
unsigned int distance, mask_dist_api;
unsigned int distances_array [500] = {0};
unsigned int config_samples_array [20] = {0};

//******************************************************************
// write a string of chars to the LCD
//******************************************************************
void lcd_puts(const char * s){

    while(*s)
        lcd_data(*s++);
}


 void set_angle(int degree_reg){
     int i = 0;
     __bis_SR_register(LPM0_bits + GIE); // waiting for mask_disk
     mask_dist_api = get_mask_dist();
     TBCCR0 = 0X6666;
     TBCCR1 = degree_reg;


     while(TBCCR1 < 0xa3d){
         start_timer_pwm_engine();   //also starts delay
         __bis_SR_register(LPM0_bits + GIE);
         TBCCTL0 &= ~CCIE; // FOR DELAY COUNTING

        //// __delay_cycles(80000);
         start_ultra_trigger();
         __bis_SR_register(LPM0_bits + GIE);
         ////start_capture_echo();
         distance = calc_dis();

         if(distance < mask_dist_api){
             distances_array[i++] =distance;
             send_dist_and_angle(distance,TBCCR1);
            // stop_ultra_trigger();
           }
         TBCCR1 += 0X37;
     }

     //TACCTL2 &= ~CCIE;
     TBCTL &= 0xffCf; //set MC0 of timer B
     TBR=0;
     send_dist_and_angle(0xffff,TBCCR1);

 }


 void telemeter (){

     unsigned int wanted_degree;
     __bis_SR_register(LPM0_bits + GIE); // waiting for wanted degree
     wanted_degree = get_mask_dist();    // NOT  mask_distance, but the wanted degree.
     TBCCR0 = 0X6666;
     TBCCR1 = wanted_degree;
     start_timer_pwm_engine();   //also starts delay
     while(state==state2){
           __delay_cycles(50000);
            start_ultra_trigger();
            __bis_SR_register(LPM0_bits + GIE);
            distance = calc_dis();
            send_dist_and_angle(distance,TBCCR1);

             }
     }


 void environment_config(){
     char i = 0;
     P1IE |= 0X01;
     unsigned int sample;
     int address = 0x3000;
     for(i = 0; i < 20; i+=2){
         __bis_SR_register(LPM0_bits + GIE); // waiting for user to be ready for sampling
          start_sampling();
          __bis_SR_register(LPM0_bits + GIE); // waiting for user to be ready for sampling
          config_samples_array[i] =  get_LDR1_samp();
          write_int_flash(address +=2 , config_samples_array[i] );
          config_samples_array[i + 1] =  get_LDR2_samp();
          write_int_flash(address +=2 , config_samples_array[i+1] );
     }


 }



