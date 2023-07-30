#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer
#include  "../header/LCD.h"
#include "stdio.h"
//#define ASCII_OFFSET 48
//#define maxTBR  0xFFFF
//#define subFreq  1048576
#define T_Pwm  0X6666


enum FSMstate state;
enum SYSmode lpm_mode;
unsigned int distance, mask_dist_api;
unsigned int distances_array[500] = {0};
unsigned int config_samples_array[20] = {0};
////////Script mode
unsigned int files_arr[100] = {0};

//******************************************************************
// write a string of chars to the LCD
//******************************************************************
void lcd_puts(const char * s){
    while(*s)
        lcd_data(*s++);
}


 void set_angle(int degree_reg){
     //int i = 0;
     __bis_SR_register(LPM0_bits + GIE); // waiting for mask_disk
     mask_dist_api = get_mask_dist();
     TBCCR0 = T_Pwm;
     TBCCR1 = 0x275;


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
             //distances_array[i++] =distance;
             send_dist_and_angle(distance,TBCCR1);
            // stop_ultra_trigger();
           }
         TBCCR1 += 0X37;
     }
     //TACCTL2 &= ~CCIE;
     TBCTL &= 0xffCf; //set MC0 of timer B
     TBR=0;
     send_dist(0xffff);
 }


 void telemeter (){

     unsigned int wanted_degree;
     __bis_SR_register(LPM0_bits + GIE); // waiting for wanted degree
     wanted_degree = get_mask_dist();    // NOT  mask_distance, but the wanted degree.
     TBCCR0 = T_Pwm;
     TBCCR1 = wanted_degree;
     start_timer_pwm_engine();   //also starts delay
     while(state==state2){
           __delay_cycles(50000);
            start_ultra_trigger();
            __bis_SR_register(LPM0_bits + GIE);
            distance = calc_dis();
            send_dist(distance);
           }
     }


 void environment_config(){
     char i = 0;
     unsigned int sample;
     int address = Flash_Address;
     P1IE |= 0X01;
     for(i = 0; i < 20; i+=2){
         __bis_SR_register(LPM0_bits + GIE); // waiting for user to be ready for sampling
          start_sampling();
          __bis_SR_register(LPM0_bits + GIE); // waiting for ADC to finish
          config_samples_array[i] =  get_LDR1_samp();
          ///write_int_flash(address +=2 , config_samples_array[i] );
          config_samples_array[i + 1] =  get_LDR2_samp();
        ///  write_int_flash(address +=2 , config_samples_array[i+1] );
          erase_segment(address);  // prepare flash
          address +=2;

     }

     address = Flash_Address;
     for(i = 0; i < 20; i+=2){
         write_int_flash(address, config_samples_array[i] );
         address +=2;
         write_int_flash(address, config_samples_array[i+1] );
         address +=2;
     }

 }


void light_sources_detector(){
    send_config_array();
    TBCCR0 = T_Pwm;
    TBCCR1 = 0x275;
    unsigned int sample;
        while(TBCCR1 < 0xa3d){
            start_timer_pwm_engine();   //also starts delay
            __bis_SR_register(LPM0_bits + GIE);
            TBCCTL0 &= ~CCIE; // FOR DELAY COUNTING
            start_sampling();
            __bis_SR_register(LPM0_bits + GIE);
            send_ldr(TBCCR1);
            TBCCR1 += 0X37;
        }
        //TACCTL2 &= ~CCIE;
        TBCTL &= 0xffCf; //set MC0 of timer B
        TBR=0;
        send_dist(0xffff);
}


//////////////////////SCRIPT MODE FUNCTIONS////////////////////////////
void count_to_x(){

        unsigned int x_counter = 0;
        char str [32]={0};
        unsigned X = get_X();
        TBCTL = TBSSEL_2 + MC_0;
        TBCCR0 = 0X28f0; // FOR 10 ms
        lcd_clear();

        while(x_counter < X){
               TBCCTL0 |= CCIE;
               TBCTL |= MC_1;
                __bis_SR_register(LPM0_bits + GIE);
                TBCTL &= ~MC1;
               sprintf(str,"%d", x_counter);
               lcd_home();
               lcd_puts(str);
               x_counter++;

    }// end while

}


void count_from_x(){

         int x_counter = get_X();
        char str [32]={0};
        TBCTL = TBSSEL_2 + MC_0;
        TBCCR0 = 0X28f0; // FOR 10 ms
        lcd_clear();

        while(x_counter >= 0 ){
               TBCCTL0 |= CCIE;
               TBCTL |= MC_1;
                __bis_SR_register(LPM0_bits + GIE);
                TBCTL &= ~MC1;
               sprintf(str,"%d", x_counter);
               lcd_clear();
               lcd_home();
               lcd_puts(str);
               x_counter--;

    }// end while

}





void store_files(){
    volatile int i;
    int address = Flash_files_address;

    for (i = 0; i<64; i++){
        write_int_flash(address, files_arr[i]);
        address +=2;
    }
}




void get_files(){   ///The function of state4
    char EOF_count = 0, files_index = 0, recv_counter = 0;

    while(EOF_count < 3){
    __bis_SR_register(LPM0_bits + GIE);
    files_arr[files_index ] |=  get_command();
    if ((recv_counter % 2) == 0)
    {
        files_arr[files_index ] = files_arr[files_index ] << 8;
        recv_counter++;
    }
    else
         {

           if(files_arr[files_index -1] == 0xffff )
                  EOF_count++;

           files_index++;

           if(EOF_count == 3){
              Set_rcv_data(0);
              files_index = 0;
            }

           recv_counter++;
         }

    } /// End while - recived all files


    store_files();  // Store in Flash Memory

}





