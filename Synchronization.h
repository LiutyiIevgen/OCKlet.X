/* 
 * File:   Synchronization.h
 * Author: andrey
 *
 * Created on 25 ?????? 2014, 13:31
 */

/**
 * @author Kyrylov Andrii
 * @param inputSignals - input signals (8 bit)
 * @todo try make synchronization (???/??? sensors)
 * @return 1 - synchronization was done
 * @return 0 - synchronization wasn't done
 */
char TrySynchronization(char inputSignals)
{
    if(!_synchronization)
        return 0;
    long currentDistance = EncGetDistanceLong();
    if(inputSignals&0x01)//???
    {
        if((currentDistance<_lowEdge+_synchZone) && (currentDistance>_lowEdge-_synchZone))
        {
            EncPositionCounter = EncDistanceToCounter(_lowEdge);
            return 1;
        }
    }
    else if(inputSignals&0x02)
    {
        if((currentDistance>_highEdge-_synchZone) && (currentDistance<_highEdge+_synchZone))
        {
            EncPositionCounter = EncDistanceToCounter(_highEdge);
            return 1;
        }
    }
    return 0;
}

