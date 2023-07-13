#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer
//#include  <string.h>
#define ascii_offset 48



enum FSMstate state;
enum SYSmode lpm_mode;


void main(void){
  state = state5;  // start in idle state on RESET
  lpm_mode = mode0;     // start in idle state on RESET
  sysConfig();
  
  while(1){
	switch(state){
	  case state0:
	      sleep_gie();
		break;
		 
	  case state1:
	     set_angle(0x275);
	     state = state0;
		break;
		 
	    case state2:
	    telemeter();
		break;
		
	  case state3:

	    break;

	  case state4:

	    break;
	  case state5:
	      environment_config();
	      state = state0;
	  break;

	}
  }
}
  
  
  
  
  
  
