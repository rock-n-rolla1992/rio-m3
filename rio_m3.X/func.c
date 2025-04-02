

#include <xc.h>
#include "main.h"



UINT8 getPotPos(UINT8 val)
{
    UINT8 cnt;
    const UINT8 adc2Pos [] = {15, 44, 73, 101, 128, 154, 182, 211, 241, 255};
    cnt = 0;
    while (cnt < 10)
    {
        if (val <= adc2Pos[cnt])
            return cnt;
        cnt++;
    }
    return 9;
}

UINT16 adc_convert(UINT8 ADCpin)
{
    ADCON0bits.CHS = ADCpin; // выбираем данный канал 
    _delay(5); // задержка для зарядки конденсатора
    ADCON0bits.GO = 1; // начать преобразование 
    while (ADCON0bits.GO) // ждем, пока преобразование завершит 
        continue;
    return (ADRES);
} // вернуть результат }