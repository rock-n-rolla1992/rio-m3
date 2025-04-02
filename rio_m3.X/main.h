

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_MAIN_H
#define XC_MAIN_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "Data_types.h"

#define RELAY_STATE_ADD 32
#define TIME_PULSE_RELAY 10000//мкс
#define TIME_FILT 5000 //мкс
#define INTERRUPT_PERIOD 128 //мкс
#define INPUT_(x) ((x == 0)?RA1:(x == 1)?RA2:(x == 2)?RA4:RC0) 
#define RELAY_ON_(x, state) ((x == 0)?LATA0 = state:(x == 1)?LATC5 = state:LATC1 = state)
#define RELAY_OFF_(x, state) ((x == 0)?LATA5 = state:(x == 1)?LATC4 = state:LATC2 = state)


#define _XTAL_FREQ 16000000 //для правильной работы задержки __delay_us()

typedef struct {
    UINT8 sw_on_by_sensor : 1;
    UINT8 Memory : 1;
} flags_t;

typedef struct {
    UINT8 Y_UP : 1;
    UINT8 Y1_UP : 1;
    UINT8 Y2_UP : 1;
    UINT8 Y_Down : 1;
    UINT8 Y1_Down : 1;
    UINT8 Y2_Down : 1;
    UINT8 Long_Y : 1;
    UINT8 Long_Y1 : 1;
    UINT8 Long_Y2 : 1;
    UINT8 Short_Y : 1;
    UINT8 Short_Y1 : 1;
    UINT8 Short_Y2 : 1;
} change_t;

typedef struct {
    UINT8 Y : 1;
    UINT8 Y1 : 1;
    UINT8 Y2 : 1;
} input_state_t;
typedef void (*current_mode_t)(void);
typedef void (*current_diagram_t)(change_t change);

UINT16 adc_convert(UINT8 ADCpin);
UINT8 getPotPos(UINT8 val);


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_MAIN_H */

