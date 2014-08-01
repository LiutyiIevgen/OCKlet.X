/* 
 * File:   DigitalInputs.h
 * Author: andrey
 *
 * Created on 31 ?????? 2013, 9:15
 * Contains methods for input/output digital signals processing
 */

/**
 * @author Kyrylov Andrii
 * @todo save data from input port
 * to input register; read data form input register to ram;
 * write data from ram to output register
 * @return input data from port
 */
char outputBuffer = 0;
char ReadDigitalInputs()
{
    TRISDbits.TRISD1 = 0;//output OE
    TRISDbits.TRISD2 = 0;//output LE

    //read input digit (RB8-RB15)
    TRISB |= 0xFF00;//input
    LATDbits.LATD2 = 0;
    LATDbits.LATD1 = 0; //clr OE (D16 save data)
    delay(25);
    int raData = PORTB;
    LATDbits.LATD1 = 1; //set OE
    return (raData>>8);
}
void WriteDigitalOutputs(char maska, char data)
{
    maska = ~maska;
    outputBuffer &= maska;
    outputBuffer |= data;
    char outputData = ~outputBuffer;
    TRISDbits.TRISD1 = 0;//output OE
    TRISDbits.TRISD2 = 0;//output LE
    //write output digit (RB8-RB15)
    TRISB |= 0xFF00;//input

    LATDbits.LATD2 = 0; //clr LE
    LATDbits.LATD1 = 1; //set OE
    TRISB &= 0x00FF;//output
    PORTB = outputData<<8;
    LATDbits.LATD2 = 1;//set LE (D17 save data)
    delay(25);
    LATDbits.LATD2 = 0;//set LE (D17 save data)
    delay(25);
}
char GetDigitalOutputs()
{
    return ~outputBuffer;
}
/**
 * @author Kyrylov Andrii
 * @param str - pointer to output string
 * @param signals - byte of input signals
 * @todo convert signals values to string ('1'/'0')
 */
void DigitalInputsToString(char *str,char signals)
{
    str[8]=0;
    int i=0;
    for(i;i<8;i++)
    {
        str[i]=signals&1;
        str[i]+=0x30;
        signals=signals>>1;
    }
}

