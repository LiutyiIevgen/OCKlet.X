#include "Kolibrovka.h"

extern float _distancePerMarks; //mm
extern long _highEdge;
extern long _lowEdge;
extern long EncPositionCounter;
extern int _synchronization;
extern int _overRiseControl;
extern int _kolibrovka;

int oldSynchronizationState;

#define DISTANCE_PER_MARKS 0x2003
char TryMakeKolibrovka(char inputSignals)
{
    if((inputSignals&0b01000000)>0) //kolibrovka input signal
    {
        _kolibrovka = 1;
        return 0;
    }
    if(!_kolibrovka)
        return 0;
    if(inputSignals&0x02)//???
    {
        oldSynchronizationState = _synchronization; // save prev state
        _overRiseControl = 0;
        _synchronization = 0;
        _distancePerMarks = 1;
        EncPositionCounter = 0;
    }
    else if(inputSignals&0x01)//???
    {
        float f1Edge = _highEdge;
        float f2Edge = _lowEdge;
        float enc = -(EncPositionCounter>>2);
        _distancePerMarks = ((f1Edge - f2Edge)/enc);
        EditParameterValue(DISTANCE_PER_MARKS,&_distancePerMarks,4);
        EncPositionCounter = _lowEdge/_distancePerMarks;
        EncPositionCounter<<=2;
        _kolibrovka = 0;
        _synchronization = oldSynchronizationState;
        _overRiseControl = 1;
    }
}
