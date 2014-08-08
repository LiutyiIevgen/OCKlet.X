#include "OverSpeedControl.h"
#include <xc.h>
extern long _highEdge;
extern long _lowEdge;
extern long overSpeedIS1[120]; extern int overSpeedIV1[120];
extern long overSpeedIS2[120]; extern int overSpeedIV2[120];
extern long overSpeedIS3[120]; extern int overSpeedIV3[120];
extern long overSpeedIS4[120]; extern int overSpeedIV4[120];
extern int _gisterezisNull;
extern long _horizont1;
extern long _horizont2;
extern long _horizont3;
extern int _maxVhorizont;
extern long _zeroPlatform;
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
    int betweenHorizonts = 0;
    char inputNumber = (inputSignals & 0b00011000)>>3;
    inputNumber = (~inputNumber) & 0x3;
    char horizontNumber0 = (inputSignals & 0b00000100)>>2;
    horizontNumber0 = (~horizontNumber0) & 0x1;
    char horizontNumber1 = (inputSignals & 0b00100000)>>5;
    horizontNumber1 = (~horizontNumber1) & 0x1;
    char horizontNumber = horizontNumber1 + horizontNumber0;
    long horizontDif;

    if(speed<0)//move down
    {
        if(s < _horizont3)
            betweenHorizonts = 1;
        if(s >= _horizont3)
            horizontDif = _horizont3 - _lowEdge;
        else if(s >= _horizont2)
            horizontDif = _horizont2 - _lowEdge;
        else if(s >= _horizont1)
            horizontDif = _horizont1 - _lowEdge;
        else if(s >= _lowEdge)
            horizontDif = _lowEdge - _lowEdge;
        s = _lowEdge + _highEdge - s;
    }
    else if (speed > 0)
    {
        if(s < _horizont3)
            betweenHorizonts = 1;
        if(s >= _horizont3)
        {
            if(inputNumber == 2)//liudi
                horizontDif = _highEdge - _zeroPlatform;
            else
                horizontDif = _highEdge - _highEdge;
        }
        else if(s >= _horizont2)
            horizontDif = _highEdge - _horizont3;
        else if(s >= _horizont1)
            horizontDif = _highEdge - _horizont2;
        else if(s >= _lowEdge)
            horizontDif = _highEdge - _horizont1;
    }
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
        if(i > 119)
        {
            int j = 119;
            for(j; j>=0; j--)
            {
                if(overSpeedIV[j] > 0)
                    return overSpeedIV[j];
            }
        }
        if((overSpeedIS[i] - horizontDif) > s)
            break;
    }
    if(betweenHorizonts == 1)
    {
        if(_maxVhorizont <= overSpeedIV[i])
           return _maxVhorizont;
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