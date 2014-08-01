/* 
 * File:   Display.h
 * Author: andrey
 *
 * Created on 6 ????????? 2013, 14:43
 * contains methods for working with lcd
 */

/**
 * @author Kyrylov Andrii
 * @todo clear display
 */
void clrscr()
{
TRISGbits.TRISG2 = 0;//RS
TRISGbits.TRISG3 = 0;//RW
TRISFbits.TRISF6 = 0;//D0
TRISD &= 0x00FF;//D1-D3, E
//display clear
LATGbits.LATG2 = 0;//RS=0 - command
LATGbits.LATG3 = 0;//RW=0 - write
LATDbits.LATD11 = 1;//E=1 - ready
//data
LATDbits.LATD10 = 0; LATDbits.LATD9 = 0; LATDbits.LATD8 = 0; LATFbits.LATF6 = 0;
delay(100);
LATDbits.LATD11 = 0;//E=0 - go
delay(100);
LATDbits.LATD11 = 1;//E=1 - ready
//data
LATDbits.LATD10 = 0; LATDbits.LATD9 = 0; LATDbits.LATD8 = 0; LATFbits.LATF6 = 1;
delay(100);
LATDbits.LATD11 = 0;//E=0 - go
delay(10000);
//----------
}

/**
 * @author Kyrylov Andrii
 * @param x - address of lcd memory
 * @todo set position pointer
 */
void setDDRAM(char x)
{
TRISGbits.TRISG2 = 0;//RS
TRISGbits.TRISG3 = 0;//RW
TRISFbits.TRISF6 = 0;//D0
TRISD &= 0x00FF;//D1-D3, E
//set DDRAM adr
LATGbits.LATG2 = 0;//RS=0 - command
LATGbits.LATG3 = 0;//RW=0 - write
LATDbits.LATD11 = 1;//E=1 - ready
//data
LATDbits.LATD10 = (x&128)>0?1:0;
LATDbits.LATD9 = (x&64)>0?1:0;
LATDbits.LATD8 = (x&32)>0?1:0;
LATFbits.LATF6 = (x&16)>0?1:0;
delay(100);
LATDbits.LATD11 = 0;//E=0 - go
delay(100);
LATDbits.LATD11 = 1;//E=1 - ready
x=x<<4;
//data
LATDbits.LATD10 = (x&128)>0?1:0;
LATDbits.LATD9 = (x&64)>0?1:0;
LATDbits.LATD8 = (x&32)>0?1:0;
LATFbits.LATF6 = (x&16)>0?1:0;
delay(100);
LATDbits.LATD11 = 0;//E=0 - go
//-----
delay(100);
}

/**
 * @author Kyrylov Andrii
 * @todo init display (set 4-bit interface, display on, clrscr,
 * set enty mode, set ddram)
 */
void DisplayInitialization()//4-bit interface
{
TRISGbits.TRISG2 = 0;//RS
TRISGbits.TRISG3 = 0;//RW
TRISFbits.TRISF6 = 0;//D0
TRISD &= 0x00FF;//D1-D3, E
//set string
LATGbits.LATG2 = 0;//RS=0 - command
LATGbits.LATG3 = 0;//RW=0 - write
LATDbits.LATD11 = 1;//E=1 - ready
//data
LATDbits.LATD10 = 0; LATDbits.LATD9 = 0; LATDbits.LATD8 = 1; LATFbits.LATF6 = 0;
delay(100);
LATDbits.LATD11 = 0;//E=0 - go
delay(100);
LATDbits.LATD11 = 1;//E=1 - ready
//data
LATDbits.LATD10 = 0; LATDbits.LATD9 = 0; LATDbits.LATD8 = 1; LATFbits.LATF6 = 0;
delay(100);
LATDbits.LATD11 = 0;//E=0 - go
delay(100);
LATDbits.LATD11 = 1;//E=1 - ready
//data
LATDbits.LATD10 = 1; LATDbits.LATD9 = 1; LATDbits.LATD8 = 1; LATFbits.LATF6 = 1;
delay(100);
LATDbits.LATD11 = 0;//E=0 - go
delay(100);
//------------
//display on
LATGbits.LATG2 = 0;//RS=0 - command
LATGbits.LATG3 = 0;//RW=0 - write
LATDbits.LATD11 = 1;//E=1 - ready
//data
LATDbits.LATD10 = 0; LATDbits.LATD9 = 0; LATDbits.LATD8 = 0; LATFbits.LATF6 = 0;
delay(100);
LATDbits.LATD11 = 0;//E=0 - go
delay(100);
LATDbits.LATD11 = 1;//E=1 - ready
//data
LATDbits.LATD10 = 1; LATDbits.LATD9 = 1; LATDbits.LATD8 = 0; LATFbits.LATF6 = 0;
delay(100);
LATDbits.LATD11 = 0;//E=0 - go
delay(100);
//------

clrscr();

//ENTRY MODE SET
LATGbits.LATG2 = 0;//RS=0 - command
LATGbits.LATG3 = 0;//RW=0 - write
LATDbits.LATD11 = 1;//E=1 - ready
//data
LATDbits.LATD10 = 0; LATDbits.LATD9 = 0; LATDbits.LATD8 = 0; LATFbits.LATF6 = 0;
delay(100);
LATDbits.LATD11 = 0;//E=0 - go
delay(100);
LATDbits.LATD11 = 1;//E=1 - ready
//data
LATDbits.LATD10 = 0; LATDbits.LATD9 = 1; LATDbits.LATD8 = 1; LATFbits.LATF6 = 0;
delay(100);
LATDbits.LATD11 = 0;//E=0 - go
delay(100);
//------
setDDRAM(0x80);
delay(3000);
}

/**
 * @author Kyrylov Andrii
 * @param *buf - pointer to output data
 * @todo show information on lcd
 */
void DisplayView(char *buf)
{
    int position = 0;
    int point = 1;
    int i;
setDDRAM(0x80);
 for(i=position+1;i<position+strlen(buf)+1;i++)//65
{
//data
LATGbits.LATG2 = 1;//RS=1 - data
LATGbits.LATG3 = 0;//RW=0 - write
LATDbits.LATD11 = 1;//E=1 - ready
//data
LATDbits.LATD10 = (buf[i-1]&128)>0?1:0;
LATDbits.LATD9 = (buf[i-1]&64)>0?1:0;
LATDbits.LATD8 = (buf[i-1]&32)>0?1:0;
LATFbits.LATF6 = (buf[i-1]&16)>0?1:0;
delay(30);
LATDbits.LATD11 = 0;//E=0 - go
delay(30);
LATDbits.LATD11 = 1;//E=1 - ready
//data
LATDbits.LATD10 = (buf[i-1]&8)>0?1:0;
LATDbits.LATD9 = (buf[i-1]&4)>0?1:0;
LATDbits.LATD8 = (buf[i-1]&2)>0?1:0;
LATFbits.LATF6 = (buf[i-1]&1)>0?1:0;
delay(30);
LATDbits.LATD11 = 0;//E=0 - go
delay(30);
 if(i%16==0)
 {
   switch(point)
   {case 1:
     { setDDRAM(0xC0);
       point=2;
       break;
     }
    case 2:
     { setDDRAM(0x90);
       point=3;
       break;
     }
    case 3:
     { setDDRAM(0xD0);
       point=4;
       break;
     }

   }
 }
}
}