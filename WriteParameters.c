#include "CanOpen.h"
unsigned char WriteAllParameters()
{
    Formatting();

    char name[] = "inf";
    DeviceInformation deviceInformation;
    deviceInformation.ObjectCount = 7;
    deviceInformation.ParametersCount = 17;
    deviceInformation.SystemName[0] = 0;
    strcat(deviceInformation.SystemName, "don_avto_1");
    deviceInformation.DeviceName[0] = 0;
    strcat(deviceInformation.DeviceName,"os_1");
    deviceInformation.DeviceVersion[0] = 0;
    strcat(deviceInformation.DeviceVersion,"v1.0");
    deviceInformation.SoftVersion[0] = 0;
    strcat(deviceInformation.SoftVersion,"1.0");
    deviceInformation.CanVersion[0] = 0;
    strcat(deviceInformation.CanVersion,"can1");

    if(!AddParameter(name,0xF,&deviceInformation,32))
        return 0;

    char name1[] = "start_s";
    long value = 400000;
    if(!AddParameter(name1,0x10,&value,3))
        return 0;

    char name2[] = "node_id";
    value = 1;
    if(!AddParameter(name2,0x10,&value,3))
        return 0;

    char name3[] = "mark_distance";
    float fvalue = 0.5;
    if(!AddParameter(name3,0x8,&fvalue,4))
        return 0;

    char name4[] = "high_edge";
    value = 50000;
    if(!AddParameter(name4,0x10,&value,3))
        return 0;

    char name5[] = "low_edge";
    value = -250000;
    if(!AddParameter(name5,0x10,&value,3))
        return 0;

    char name6[] = "synchronization";
    value = 1;
    if(!AddParameter(name6,0x03,&value,2))
        return 0;

    char name7[] = "synch_zone";
    value = 10000;
    if(!AddParameter(name7,0x03,&value,2))
        return 0;

    char name8[] = "over_rise_zone_low";
    value = 500;
    if(!AddParameter(name8,0x03,&value,2))
        return 0;

    char name9[] = "over_rise_zone_high";
    value = 500;
    if(!AddParameter(name9,0x03,&value,2))
        return 0;

    char name10[] = "def_diag_rev";
    char codtValue[120];
    int i=0;
    for(i;i<120;i++)
        codtValue[i] = 0;
    codtValue[0] = 0xF0; codtValue[1] = 0xD8; codtValue[2] = 0xFF; codtValue[3] = 0xFF;
    codtValue[4] = 0xEC; codtValue[5] = 0x2C;
    codtValue[6] = 0xC8; codtValue[7] = 0xAF;
    codtValue[10] = 0xD0; codtValue[11] = 0x07;
    codtValue[12] = 0xFF; codtValue[13] = 0xFF; codtValue[14] = 0xFF; codtValue[15] = 0x7F;
    codtValue[16] = 0xDC; codtValue[17] = 0x05;
    if(!AddParameter(name10,0xF,codtValue,120))
        return 0;

    char name11[] = "def_diag_gruz";
    for(i;i<120;i++)
        codtValue[i] = 0;
    codtValue[0] = 0xF0; codtValue[1] = 0xD8; codtValue[2] = 0xFF; codtValue[3] = 0xFF;
    codtValue[4] = 0xF8; codtValue[5] = 0x2A;
    codtValue[6] = 0xC8; codtValue[7] = 0xAF;
    codtValue[10] = 0xD0; codtValue[11] = 0x07;
    codtValue[12] = 0xFF; codtValue[13] = 0xFF; codtValue[14] = 0xFF; codtValue[15] = 0x7F;
    codtValue[16] = 0xDC; codtValue[17] = 0x05;
    if(!AddParameter(name11,0xF,codtValue,120))
        return 0;

    char name12[] = "def_diag_ludi";
    for(i;i<120;i++)
        codtValue[i] = 0;
    codtValue[0] = 0xF0; codtValue[1] = 0xD8; codtValue[2] = 0xFF; codtValue[3] = 0xFF;
    codtValue[4] = 0x04; codtValue[5] = 0x29;
    codtValue[6] = 0xC8; codtValue[7] = 0xAF;
    codtValue[10] = 0xD0; codtValue[11] = 0x07;
    codtValue[12] = 0xFF; codtValue[13] = 0xFF; codtValue[14] = 0xFF; codtValue[15] = 0x7F;
    codtValue[16] = 0xDC; codtValue[17] = 0x05;
    if(!AddParameter(name12,0xF,codtValue,120))
        return 0;

    char name13[] = "def_diag_negab";
    for(i;i<120;i++)
        codtValue[i] = 0;
    codtValue[0] = 0xF0; codtValue[1] = 0xD8; codtValue[2] = 0xFF; codtValue[3] = 0xFF;
    codtValue[4] = 0x10; codtValue[5] = 0x27;
    codtValue[6] = 0xC8; codtValue[7] = 0xAF;
    codtValue[10] = 0xD0; codtValue[11] = 0x07;
    codtValue[12] = 0xFF; codtValue[13] = 0xFF; codtValue[14] = 0xFF; codtValue[15] = 0x7F;
    codtValue[16] = 0xDC; codtValue[17] = 0x05;
    if(!AddParameter(name13,0xF,codtValue,120))
        return 0;

    char name14[] = "gisterezis_null";
    int ivalue = 100;
    if(!AddParameter(name14,0x03,&ivalue,2))
        return 0;

    char name15[] = "v_coefficient";
    fvalue = 1.0;
    if(!AddParameter(name15,0x08,&fvalue,4))
        return 0;

    char name16[] = "a_coefficient";
    fvalue = 1.0;
    if(!AddParameter(name16,0x08,&fvalue,4))
        return 0;
    
    return 1;
}
