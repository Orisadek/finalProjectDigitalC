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
unsigned int script[21];

struct ScriptFiles {
    unsigned int addresses[3];

};




//******************************************************************
// write a string of chars to the LCD
//******************************************************************
void lcd_puts(const char * s){
    while(*s)
        lcd_data(*s++);
}


 void set_angle(unsigned int l, unsigned int r, char mode){   /// mode 0 -> Not script, Script is mode 1
     //int i = 0;
   if(mode == 0){
     __bis_SR_register(LPM0_bits + GIE); // waiting for mask_disk
     mask_dist_api = get_mask_dist();
   }
   TBCCR0 = T_Pwm;
   TBCCR1 = l;


     while(TBCCR1 < r){
         start_timer_pwm_engine();   //also starts delay
         __bis_SR_register(LPM0_bits + GIE);
         TBCCTL0 &= ~CCIE; // FOR DELAY COUNTING
        //// __delay_cycles(80000);
         start_ultra_trigger();
         __bis_SR_register(LPM0_bits + GIE);
         ////start_capture_echo();
         distance = calc_dis();

        if(mode == 0){                  ///if not in script mode
         if(distance < mask_dist_api){
             //distances_array[i++] =distance;
             send_dist_and_angle(distance,TBCCR1);
            // stop_ultra_trigger();
           }
        } else                        /// mode = 1 -> script mode
        {
            send_dist_and_angle(distance,TBCCR1);
        }

         TBCCR1 += 0X37;
     }//end while loop
     //////TACCTL2 &= ~CCIE;
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

          address +=2;

     }

     address = Flash_Address;
     erase_segment(address);  // prepare flash
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
void count_to_x(char X){

         char x_counter = 0;
        char str [32]={0};

        TBCTL = TBSSEL_2 + MC_0;
        TBCCR0 = 0X28f0; // FOR 10 ms
        lcd_clear();

        while(x_counter <= X){
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


void count_from_x(char X){

        char x_counter = X;
        char str [32]={0};
        TBCTL = TBSSEL_2 + MC_0;
        TBCCR0 = 0X28f0; // FOR 10 ms
        lcd_clear();

        while(x_counter < 255 ){
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

        erase_segment(address);  // prepare flash



  //  address = Flash_files_address;
    for (i = 0; i<64; i++){
        write_int_flash(address,files_arr[i]);
        address +=2;
    }
}




void get_files(){   ///The function of state4
    char EOF_count = 0, files_index = 0, recv_counter = 0;
    char com;

    while(EOF_count < 6){
    __bis_SR_register(LPM0_bits + GIE);

    com = get_command();

    if(com == 0xff)  /// 0xff means EOF
        EOF_count++;

    files_arr[files_index ] |= com;
    if ((recv_counter % 2) == 0) // reciveng msb
    {
        files_arr[files_index ] = files_arr[files_index ] << 8;
        recv_counter++;
    }
    else                       //reciveng lsb
         {

           files_index++;

           if(EOF_count == 6){
              Set_rcv_data(0);
              files_index = 0;
            }

           recv_counter++;
         }

    } /// End while - recived all files


    store_files();  // Store in Flash Memory

}

void rotate_char(char X){
        TBCTL = TBSSEL_2 + MC_0;
        TBCCR0 = 0X28f0; // FOR 10 ms

        lcd_clear();
        int i = 0;

        while(i < 32 ){

               TBCCTL0 |= CCIE;
               TBCTL |= MC_1;
                __bis_SR_register(LPM0_bits + GIE);
                TBCTL &= ~MC1;
                if(i == 16){
                //  lcd_putchar(" ");
                 lcd_cursor_left();
                  lcd_putchar(32);
                  lcd_new_line;
                }
               lcd_putchar(X);
               lcd_cursor_left();
               lcd_cursor_left();
                lcd_putchar(32);
                lcd_cursor_right();

            //   lcd_cursor_left() ;
            //   lcd_putchar(" ");
               i++;

    }// end while


}




void do_script(){
    unsigned int com;
    int i;
    unsigned int l_before,l,r;

    struct ScriptFiles s;
    s.addresses[0] = 0x1000;
    s.addresses[1] = 0x102a;
    s.addresses[2] = 0x1054;

    __bis_SR_register(LPM0_bits + GIE);
    char script_num = get_Script_Num();


     switch (script_num){
    case 1:
        transferBlock_script( s.addresses[0], script, 42);

        break;

    case 2:
        transferBlock_script( s.addresses[1], script, 42);
        __bis_SR_register(LPM0_bits + GIE);
        break;

    case 3:
        transferBlock_script( s.addresses[2], script, 42);
        __bis_SR_register(LPM0_bits + GIE);
        break;
    }

    for (i = 0; i<21; i+=2){
        if(script[i] != 0xffff)
        {
           com =  script[i] >> 8;
          switch (com){
           case 1:
               count_to_x(script[i]);
           break;

           case 2:
               count_from_x(script[i]);
           break;
           case 3:
               rotate_char(script[i]);
           break;

           case 4:
               set_d(script[i]);
            break;

           case 5:
               lcd_clear();
            break;

           case 7:
               l = 629 + 11*((script[i] << 8) >>8 );
               r = 629 + 11*(script[i+1] >> 8);
               while (!(IFG2 & UTXIFG1));
               TXBUF1 = 7;
               while (!(IFG2 & UTXIFG1));
               state= state1;
               set_angle( l,  r, 1);
               state= state6;
         }
        }

        else // EOF
          break;
      }

    while (!(IFG2 & UTXIFG1));
    TXBUF1 = 255;

}



