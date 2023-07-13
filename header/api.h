#ifndef _api_H_
#define _api_H_

#include  "../header/halGPIO.h"     // private library - HAL layer


extern void lcd_puts(const char *);
extern void set_angle(int degree_reg);
extern void telemeter (void);
extern void environment_config(void);

#endif







