/* 
 * File:   main.c
 * Author: andrey
 *
 * Created on 30 ?????? 2013, 9:31
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xc.h>


#include "globals.h"
#include "LowVoltageDetect.h"
#include "MainLibrary.h"
#include "DigitalInputs.h"
#include "Uart.h"
#include "Timer.h"
#include "Encoder.h"
#include "Display.h"
#include "Spi.h"
#include "RealTimer.h"
#include "FRAM.h"
#include "Synchronization.h"
#include "OverSpeedControl.h"

// FOSC
#pragma config FOSFPR = XT_PLL16             // Oscillator (XT)
#pragma config FCKSMEN = CSW_FSCM_OFF   // Clock Switching and Monitor (Sw Disabled, Mon Disabled)

// FWDT
#pragma config FWPSB = WDTPSB_16        // WDT Prescaler B (1:16)
#pragma config FWPSA = WDTPSA_512       // WDT Prescaler A (1:512)
#pragma config WDT = WDT_OFF            // Watchdog Timer (Disabled)

// FBORPOR
#pragma config FPWRT = PWRT_64          // POR Timer Value (64ms)
#pragma config BODENV = BORV_45            // Brown Out Voltage (4.5)
#pragma config BOREN = PBOR_OFF         // PBOR Enable (Disable)
#pragma config MCLRE = MCLR_EN         // Master Clear Enable (Enable)

// FBS
#pragma config BWRP = WR_PROTECT_BOOT_OFF// Boot Segment Program Memory Write Protect (Boot Segment Program Memory may be written)
#pragma config BSS = NO_BOOT_CODE       // Boot Segment Program Flash Memory Code Protection (No Boot Segment)
#pragma config EBS = NO_BOOT_EEPROM     // Boot Segment Data EEPROM Protection (No Boot EEPROM)
#pragma config RBS = NO_BOOT_RAM        // Boot Segment Data RAM Protection (No Boot RAM)

// FSS
#pragma config SWRP = WR_PROT_SEC_OFF   // Secure Segment Program Write Protect (Disabled)
#pragma config SSS = NO_SEC_CODE        // Secure Segment Program Flash Memory Code Protection (No Secure Segment)
#pragma config ESS = NO_SEC_EEPROM      // Secure Segment Data EEPROM Protection (No Segment Data EEPROM)
#pragma config RSS = NO_SEC_RAM         // Secure Segment Data RAM Protection (No Secure RAM)

// FGS
#pragma config GWRP = GWRP_OFF          // General Code Segment Write Protect (Disabled)
#pragma config GCP = GSS_OFF            // General Segment Code Protection (Disabled)

// FICD
#pragma config ICS = ICS_PGD            // Comm Channel Select (Use PGC/EMUC and PGD/EMUD)


// Function prototype for timer 1 ISR
void __attribute__((__interrupt__, __auto_psv__)) _T1Interrupt(void);
// Function prototype for timer 2 ISR
void __attribute__((__interrupt__, __auto_psv__)) _T2Interrupt(void);
// Function prototype for timer 3 ISR
void __attribute__((__interrupt__, __auto_psv__)) _T3Interrupt(void);
// Function prototype for timer 4 ISR
void __attribute__((__interrupt__, __auto_psv__)) _T4Interrupt(void);
// Function prototype for timer 5 ISR
void __attribute__((__interrupt__, __auto_psv__)) _T5Interrupt(void);
// Can1 Receive Parameter
void __attribute__ ((__interrupt__, __auto_psv__)) _C1Interrupt (void);
// Can2 Receive Parameter
void __attribute__ ((__interrupt__, __auto_psv__)) _C1Interrupt (void);
// LVD interrupt
void __attribute__((__interrupt__, __auto_psv__)) _LVDInterrupt(void);

int repeatCount = 0;

int main(int argc, char** argv) {
    ADPCFG = 0xFFFF;//RA only digit
    DisplayInitialization(); //lcd display init
    RtcInitialization(); //realtime counter init
    //RtcSetTime();
    FramInitialization(); //fram init
    //read config
    WriteAllParameters();

    ReadParameterValue(ENCODER_COUNTER,&EncPositionCounter); //read position counter id = 0x2000

    ReadConfig();
    
    LVDinitialization(); //voltage detect interrupt

    WriteDigitalOutputs(0b01000011,0b00000011);

    Can1Initialization();
    Can2Initialization();
    //OpenUART2();
    StartTimer1();
    StartTimer2();
    StartTimer3();
    StartTimer4();
    StartTimer5();
    DisplayView("start"); //lcd display write
    
    while(1);
    return (EXIT_SUCCESS);
}
// Timer 1 interrupt service write data to uart
void __attribute__((__interrupt__, __auto_psv__)) _T1Interrupt(void)
{
    // Clear Timer 1 interrupt flag
    _T1IF = 0;
    TRISCbits.TRISC13 = 0;
    // Toggle LED on RD1
    LATCbits.LATC13 = 1 - LATCbits.LATC13;

    char signals = ReadDigitalInputs();

    // write to uart encoder counter
    char str[60]="";
    char sDistance[12];
    char sSignals[9];
    
    RtcWriteDateInString(str); //time and date
    while(strlen(str)<32)
      strcat(str," ");

    char offset = EncGetDistanceStr(sDistance);//distance in mm
    strcat(str, sDistance+offset);
    while(strlen(str)<48)
      strcat(str," ");

    DigitalInputsToString(sSignals,~signals);//8 analog signals
    strcat(str,sSignals);
    while(strlen(str)<64)
      strcat(str," ");

    DisplayView(str);

    //LongToCharArray(140,sDistance);
    //PrintDigitUART2(sDistance,4);
}
// Timer 2 interrupt service count encoder signal f1 f2
void __attribute__((__interrupt__, __auto_psv__)) _T2Interrupt(void)
{
    // Clear Timer 2 interrupt flag
    _T2IF = 0;
    EncoderScan();

    repeatCount++;

    if(repeatCount==10000)
    {
        repeatCount=0;
        EncCountV();
    }
}
void __attribute__((__interrupt__, __auto_psv__)) _T3Interrupt(void)
{
    // Clear Timer 3 interrupt flag
    _T3IF = 0;
    TRISDbits.TRISD0 = 0;//out WDT
    LATDbits.LATD0 = 1 -  LATDbits.LATD0;

    TRISCbits.TRISC14 = 0;
    // Toggle LED on RD1
    LATCbits.LATC14 = 1 - LATCbits.LATC14;

    char signals = ReadDigitalInputs();
    //sync encoder counter
    TrySynchronization(signals);
    //over rise zone control
    OverRiseZoneControl(EncGetDistanceLong());
    //try make kolibrovka
    TryMakeKolibrovka(signals);
    //try on overspeed  relay
    TryInitOverSpeedControl(signals);
    //over speed
    int speed = EncGetV();
    
    long lDistance = EncGetDistanceLong();

    MakeOverSpeedControl(lDistance,speed,signals);
}
void __attribute__((__interrupt__, __auto_psv__)) _T4Interrupt(void)
{
    // Clear Timer 4 interrupt flag
    // Write to can bus
    _T4IF = 0;
    int speed = EncGetV();
    long lDistance = EncGetDistanceLong();
    long highEdge = _highEdge;
    long lowEdge = _lowEdge;
    long rDistance = lowEdge + highEdge - lDistance;
    int a = 0;
    char inputSignals = ReadDigitalInputs();
    int maxV = OverSpeedGetMaxV(lDistance,speed,inputSignals);
    char externSignals[8] = {inputSignals,0,0,0,GetDigitalOutputs(),0,0,0};
    CanOpenSendCurrentObjectState(rDistance,lDistance,speed,maxV,a,externSignals,1);
    CanOpenSendCurrentObjectState(rDistance,lDistance,speed,maxV,a,externSignals,2);
}
void __attribute__((__interrupt__, __auto_psv__)) _T5Interrupt(void)
{
    // Clear Timer 5 interrupt flag
    // Write to can bus
    _T5IF = 0;
    Can1Execute();
    Can2Execute();
}
void __attribute__ ((__interrupt__, __auto_psv__)) _C1Interrupt (void){
    IFS1bits.C1IF = 0; //Clear CAN1 interrupt flag
    C1INTFbits.RX0IF = 0; //Clear CAN1 RX interrupt flag
    C1INTFbits.RX1IF = 0; //Clear CAN1 RX interrupt flag
    char rxData[8];
    if(C1CTRLbits.ICODE == 7) //check filters
    {
        C1INTFbits.WAKIF = 0;
        return;
    }
    FramWrite(5172,&EncPositionCounter,4); //write current counter

    unsigned int sid = C1RX0SIDbits.SID;
    Can1ReceiveData(rxData);
    CanOpenParseRSDO(sid,rxData,1); //parse message and send response
    C1RX0CONbits.RXFUL = 0;
    C1RX1CONbits.RXFUL = 0;
  }
void __attribute__ ((__interrupt__, __auto_psv__)) _C2Interrupt (void){
    IFS2bits.C2IF = 0; //Clear CAN1 interrupt flag
    C2INTFbits.RX0IF = 0; //Clear CAN1 RX interrupt flag
    C2INTFbits.RX1IF = 0; //Clear CAN1 RX interrupt flag
    char rxData[8];
    if(C2CTRLbits.ICODE == 7) //check filters
    {
        C2INTFbits.WAKIF = 0;
        return;
    }
    
    FramWrite(5172,&EncPositionCounter,4); //write current counter

    unsigned int sid = C2RX0SIDbits.SID;
    Can2ReceiveData(rxData);
    CanOpenParseRSDO(sid,rxData,2); //parse message and send response
    C2RX0CONbits.RXFUL = 0;
    C2RX1CONbits.RXFUL = 0;
  }

void __attribute__((__interrupt__, __auto_psv__)) _LVDInterrupt(void) //low voltage detcetion
//save data in fram
{
    _LVDIF = 0; //clear interrupt flag
    //fram write position counter
    FramWrite(5172,&EncPositionCounter,4); //adr=0 : data sector
}