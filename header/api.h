#ifndef _api_H_
#define _api_H_

#include  "../header/halGPIO.h"     // private library - HAL layer


extern void lcd_puts(const char *);
extern void set_angle(unsigned int l, unsigned int r, char mode);
extern void telemeter (void);
extern void environment_config(void);
extern void light_sources_detector(void);
#endif







