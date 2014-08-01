/* 
 * File:   Uart.h
 * Author: andrey
 *
 * Created on 31 ?????? 2013, 14:23
 */

/**
 * @author Kyrylov Andrii
 * @todo init uart
 */
void OpenUART2()
{
    TRISF = 0x00;
    //LATF=0;
    U2MODEbits.STSEL = 0;
    U2MODEbits.PDSEL = 0;
    U2MODEbits.ABAUD = 0;
    U2BRG = 155;
    U2MODEbits.UARTEN = 1;
    U2STAbits.UTXEN = 1;
}

/**
 * @author Kyrylov Andrii
 * @todo convert long digit to char array (4 byte)
 * @return busy flag 1 - busy 0 - ready
 */
char BusyUART2(void)
{
    if(!IFS1bits.U2TXIF)
        return 1;
    else
    {
        IFS1bits.U2TXIF = 0;
        return 0;
    }
}

/**
 * @author Kyrylov Andrii
 * @param Data - value for sending
 * @todo send data over uart
 */
void WriteCharToUART2(unsigned char Data)
{
    U2TXREG = Data;
    while(BusyUART2());
}

/**
 * @author Kyrylov Andrii
 * @param *String - pointer to data for sending
 * @todo send string over uart
 */
void PrintStringUART2(unsigned char *String)
{
    while(*String)
    {
        WriteCharToUART2(*String++);
    }
}

/**
 * @author Kyrylov Andrii
 * @param *Data - pointer to data for sending
 * @param num - number of bytes
 * @todo send array over uart
 */
void PrintDigitUART2(unsigned char *Data,int num)
{
    int j=0;
    for (j=0;j<num; j++)
        WriteCharToUART2(*Data++);
}

