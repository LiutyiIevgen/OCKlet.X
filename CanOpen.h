/* 
 * File:   CanOpen.h
 * Author: Æåíÿ
 *
 * Created on 11 ??????? 2014 ?., 9:55
 */
typedef struct DeviceInformation //28bytes
{
    unsigned char ObjectCount; 
    unsigned int ParametersCount; 
    char SystemName[10]; //3600 bytes (count = 400)
    char DeviceName[4]; //3048 bytes (count = 762)
    char DeviceVersion[4]; //4
    char SoftVersion[3]; //9
    char CanVersion[4];
    char buf[4];
} DeviceInformation;

/**
 * @autor Kyrylov Andrii
 * @param long - s1
 * @param long - s2
 * @param int - speed(v)
 * @param int - a
 * @param char - input signals
 * @param char - can number
 * @todo send current object state using tpdo over can bus
 */
void CanOpenSendCurrentObjectState(long, long, int, int, int, char*,char);

