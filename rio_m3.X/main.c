

#include <xc.h>
#include "main.h"

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = ON        // Watchdog Timer Enable (WDT enabled)
#pragma config PWRTE = ON       // Power-up Timer Enable (PWRT enabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = ON          // Flash Program Memory Code Protection (Program memory code protection is enabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF      // PLL Enable (4x PLL disabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage 
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

UINT8 input_state[4] = {0};
UINT8 flag_up_front[4] = {0};
UINT8 flag_low_front[4] = {0};
UINT8 Relay_Target[3] = {0};
UINT8 Relay_State[3] = {0};
UINT8 Saved_Relay_Target[3] = {0};

void main(void)
{
    __EEPROM_DATA('M', 'e', 'a', 'n', 'd', 'r', ' ', ' ');
    __EEPROM_DATA("R", "I", "O", "M", "3", "1", ".", "0");

    ADCON1bits.ADFM = 1;
    ADCON1bits.ADCS = 0b101;
    ADCON1bits.ADPREF = 0b00;
    ADCON0bits.ADON = 1;
    OSCCONbits.SCS = 0;
    OSCCONbits.IRCF = 0b1111;
    OSCCONbits.SPLLEN = 0;
    OPTION_REGbits.nWPUEN = 0;
    OPTION_REGbits.TMR0CS = 0;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS = 0;
    WPUA = 0;
    WPUC = 0;
    WPUA3 = 1;

    ANSA1 = DIG;
    ANSA2 = DIG;
    ANSA4 = DIG;
    ANSC0 = DIG;
    LATA = 0;
    LATC = 0;
    TRISA0 = OUT;
    TRISA5 = OUT;
    TRISC5 = OUT;
    TRISC4 = OUT;
    TRISC1 = OUT;
    TRISC2 = OUT;

    GIE = 1;
    PEIE = 1;
    __delay_ms(100);
    for (UINT8 index = 0; index < 3; index++)
    {
        Relay_Target[index] = Relay_State[index] = Saved_Relay_Target[index] = eeprom_read(RELAY_STATE_ADD + index);
        //Relay_Target[index] = Relay_State[index] = Saved_Relay_Target[index] = 0;
        //Relay_Target[index] = 1;
    }
    T0IE = 1;
#if 0
    TRIS_INPUT_Y = IN;
    while (1)
    {
        if (INPUT_Y)
        {
            eeprom_write(40, 1);
        }
        __delay_ms(1000);
        CLRWDT();
    }

#endif
    while (1)
    {
        if (input_state[3])
        {
            if (flag_up_front[3])
            {
                Relay_Target[0] = Relay_Target[1] = Relay_Target[2] = 0;
                eeprom_write(RELAY_STATE_ADD, 0);
                eeprom_write(RELAY_STATE_ADD + 1, 0);
                eeprom_write(RELAY_STATE_ADD + 2, 0);
                flag_up_front[3] = 0;
            }
            flag_low_front[0] = flag_low_front[1] = flag_low_front[2] = 0;
        } else
        {
            for (UINT8 index = 0; index < 3; index++)
            {
                if (flag_low_front[index])
                {
                    Relay_Target[index] = !Relay_Target[index];
                    eeprom_write(RELAY_STATE_ADD + index, Relay_Target[index]);
                    flag_low_front[index] = 0;
                }
            }
        }
    }
}

void interrupt func_interrupt(void)
{
    static UINT8 index_input = 0;
    static UINT8 cnt_filt[4] = {0};
    static UINT8 time_filt = (3000/(INTERRUPT_PERIOD*4));
    UINT8 temp = INPUT_(index_input);
    if (input_state[index_input] != temp)
    {
        if (++cnt_filt[index_input] > time_filt)
        {
            time_filt = (temp) ?(20000/(INTERRUPT_PERIOD*4)) : (3000/(INTERRUPT_PERIOD*4));
            input_state[index_input] = temp;
            flag_up_front[index_input] = temp;
            flag_low_front[index_input] = !temp;
            cnt_filt[index_input] = 0;
        }
    } else
        cnt_filt[index_input] = 0;
    if (++index_input >= 4)
        index_input = 0;


#if 1
    static UINT8 index_relay = 0;
    static UINT16 timer_relay[3] = {0};

    if (Relay_Target[index_relay] != Relay_State[index_relay] || Saved_Relay_Target[index_relay] != Relay_State[index_relay])
    {
        switch (timer_relay[index_relay])
        {
            case 0:
                if (Relay_Target[index_relay])
                    RELAY_ON_(index_relay, 1);
                else
                    RELAY_OFF_(index_relay, 1);
                Saved_Relay_Target[index_relay] = Relay_Target[index_relay];
                timer_relay[index_relay]++;
                break;
            case TIME_PULSE_RELAY / (INTERRUPT_PERIOD * 3):
                if (Saved_Relay_Target[index_relay])
                    RELAY_ON_(index_relay, 0);
                else
                    RELAY_OFF_(index_relay, 0);
                Relay_State[index_relay] = Saved_Relay_Target[index_relay];
                timer_relay[index_relay] = 0;
                break;
            default:
                timer_relay[index_relay]++;
                break;
        }
    }
    if (++index_relay >= 3)
        index_relay = 0;
#endif
    T0IF = 0;
}