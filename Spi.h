/* 
 * File:   Spi.h
 * Author: andrey
 *
 * Created on 13 ????????? 2013, 13:52
 * library for work with spi
 */

#include <spi.h>

/**
 * @author Kyrylov Andrii
 * @todo ini spi modul (8-bit interface)
 */
void ConfigurateSPI2()
{
   unsigned int SPICONValue = 0;
   unsigned int SPISTATValue = 0;
   TRISGbits.TRISG6 = 0; // SCK out
   TRISGbits.TRISG7 = 0; // SDO out
   TRISGbits.TRISG8 = 1; // SDI in
   delay(2000);

   SPICONValue =     FRAME_ENABLE_OFF &
                            FRAME_SYNC_OUTPUT &
                            ENABLE_SDO_PIN &
                            SPI_MODE16_OFF &     //  16 / 8 bit
                            SPI_SMP_OFF &
                            SPI_CKE_OFF &
                            SLAVE_ENABLE_OFF  &
                            CLK_POL_ACTIVE_HIGH &
                            MASTER_ENABLE_ON &
                            SEC_PRESCAL_1_1 &
                            PRI_PRESCAL_64_1;

   SPISTATValue =    SPI_ENABLE &
                            SPI_IDLE_CON &
                            SPI_RX_OVFLOW_CLR;

   OpenSPI2(SPICONValue, SPISTATValue);
    /*/IFS1bits.SPI2IF = 0;
   IEC1bits.SPI2IE = 1;
   SPI2STATbits.SPITBF = 0;*/
}

/**
 * @author Kyrylov Andrii
 * @param data_out - output value
 * @todo write over spi
 * @return 1 - success
 */
char spi2_write(unsigned int data_out)
{
    IFS1bits.SPI2IF = 0;
     if (SPI2CONbits.MODE16)          /* word write */
         SPI2BUF = data_out;
     else
         SPI2BUF = data_out & 0xff;    /*  byte write  */
     while(SPI2STATbits.SPITBF);
     delay(40);
     data_out = SPI2BUF;               //Avoiding overflow when reading
     return 1;
}

/**
 * @author Kyrylov Andrii
 * @todo read byte over spi module
 * @return byte is received from spi
 */
unsigned char spi2_read(void)
{
  SPI2STATbits.SPIROV = 0;
  SPI2BUF = 0x00;                  // initiate bus cycle
  while(!SPI2STATbits.SPIRBF);
  // Check for Receive buffer full status bit of status register
  if (SPI2STATbits.SPIRBF)
  {
      SPI2STATbits.SPIROV = 0;

      if (SPI2CONbits.MODE16)
          return (SPI2BUF);           // return word read
      else
          return (SPI2BUF & 0xff);    // return byte read
  }
  return 1;                          // RBF bit is not set return error
}