#include "OverSpeedControl.h"
#include <xc.h>
extern long _highEdge;
extern long _lowEdge;
extern long overSpeedIS1[120]; extern int overSpeedIV1[120];
extern long overSpeedIS2[120]; extern int overSpeedIV2[120];
extern long overSpeedIS3[120]; extern int overSpeedIV3[120];
extern long overSpeedIS4[120]; extern int overSpeedIV4[120];
extern int _gisterezisNull;
void OverSpeedInterpolation(long* coordinate, int* speed, long* interCoordinate, int* interSpeed)
{
                    int vStep = -100; //0.1 m/sec
                    int k = 0;
                    int i;
                    for (i = 0; coordinate[i] != 2147483647; i++)
                    {
			interCoordinate[k] = coordinate[i];
                        interSpeed[k] = speed[i];
			k++;
                        int speed2 = speed[i + 1];
                        int speed1 = speed[i];
                        int vSub = speed2 - speed1;
                        int intervalsNum = abs(vSub/vStep);
                        if (intervalsNum == 0)
                        {
                            intervalsNum = 0;
                            continue;
                        }
                        long sStep;
                        if(coordinate[i+1] == 2147483647)
                            sStep = abs((_highEdge - coordinate[i])/intervalsNum);
                        else
                            sStep = abs((coordinate[i + 1] - coordinate[i])/intervalsNum);
                        int j=1;
                        for (j = 1; j < intervalsNum; j++)
                        {
                                interCoordinate[k] = coordinate[i] + j*sStep;
                                interSpeed[k] = speed[i] + j*vStep;
								k++;
                        }
                    }

                        interCoordinate[k] = _highEdge;
                        interSpeed[k] = speed[i];
}
int OverSpeedGetMaxV(long s,int speed,char inputSignals)
{
    if(speed == 0)
        return _gisterezisNull;
    int i=0;
    char inputNumber = (inputSignals & 0b00011000)>>3;
    inputNumber = (~inputNumber) & 0x3;

    if(speed<0)//move down
        s = _lowEdge + _highEdge - s;
    if(s>=_highEdge)
        s = _highEdge-1;
    long *overSpeedIS;
    int *overSpeedIV;
    switch(inputNumber) //choose a deffence diagramm
        {
            case 0:
                overSpeedIS = overSpeedIS1;
                overSpeedIV = overSpeedIV1;
                break;
            case 1:
                overSpeedIS = overSpeedIS2;
                overSpeedIV = overSpeedIV2;
                break;
            case 2:
                overSpeedIS = overSpeedIS3;
                overSpeedIV = overSpeedIV3;
                break;
            case 3:
                overSpeedIS = overSpeedIS4;
                overSpeedIV = overSpeedIV4;
                break;
        }
    for(i;;i++)
    {
        if(overSpeedIS[i]>s)
            break;
    }
    return overSpeedIV[i];
}
void TryInitOverSpeedControl(char signals)
{
    if((signals&0b10000000) == 0)
        WriteDigitalOutputs(0b01000000,0b01000000);
}

void MakeOverSpeedControl(long s,int speed,char inputSignals)
{
    int maxV = OverSpeedGetMaxV(s,speed,inputSignals);
    if(abs(speed)>maxV)
        WriteDigitalOutputs(0b01000000,0b00000000);
}