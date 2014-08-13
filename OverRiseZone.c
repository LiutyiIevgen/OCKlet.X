#include "OverRiseZone.h"
#include <xc.h>

extern long _highEdge;
extern long _lowEdge;
extern long _zeroPlatform;
extern int _overRizeZoneHigh;
extern int _overRizeZoneLow;
extern int _overRiseControl;
long hEdge;
char OverRiseZoneControl(long currentS)
{
    if(!_overRiseControl)
        return;
    char inputSignals = ReadDigitalInputs();
    char regim = (inputSignals & 0b00011000)>>3;
    regim = (~regim) & 0x3;
    if(regim == 2)//liudi
        hEdge = _zeroPlatform;
    else
        hEdge = _highEdge;
    if(currentS < _lowEdge - _overRizeZoneLow) //overise low
        WriteDigitalOutputs(0x3,0x2);
    else if(currentS > hEdge + _overRizeZoneHigh)
        WriteDigitalOutputs(0x3,0x1);
    else
        WriteDigitalOutputs(0x3,0x3);
}
void OnOveriseRele(char data)
{
    TRISDbits.TRISD1 = 0;//output OE
    TRISDbits.TRISD2 = 0;//output LE
    //write output digit (RB8-RB15)
    TRISB |= 0xFF00;//input

    LATDbits.LATD2 = 0; //clr LE
    LATDbits.LATD1 = 1; //set OE
    TRISB &= 0x00FF;//output
    PORTB = (PORTB&0xFCFF)|data;
    LATDbits.LATD2 = 1;//set LE (D17 save data)
    delay(25);
    LATDbits.LATD2 = 0;//set LE (D17 save data)
    delay(25);
}