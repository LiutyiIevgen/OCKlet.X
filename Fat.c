#include "Fat.h"
void WriteBootSector()
{
    Boot boot;
    boot.BootSectorSize = 16;
    boot.FatSectorSize = 1524;
    boot.DescriptorSectorSize = 3600;
    boot.DataSectorSize = 3048;
    boot.ClusterSize = 4;
    boot.DescriptorSize = 9;
    FramWrite(0,&boot, boot.BootSectorSize);
}
void ReadBootSector(Boot* boot)
{
     FramRead(0, boot, 16);
}
void Formatting()
{
    Boot boot;
    int dzero = 0;
    char zero = 0;
    WriteBootSector();
    ReadBootSector(&boot);
    int adr = boot.BootSectorSize;
    for(adr; adr < boot.BootSectorSize + boot.FatSectorSize; adr += 2)
    {
        FramWrite(adr,&dzero, sizeof(int));
    }
    adr = boot.BootSectorSize + boot.FatSectorSize;
    for(adr; adr < boot.BootSectorSize + boot.FatSectorSize + boot.DescriptorSectorSize; adr += boot.DescriptorSize)
    {
        FramWrite(adr,&zero, 1);
    }
}
unsigned int ReadNextFat(unsigned int currentFat)
{
    Boot boot;
    ReadBootSector(&boot);
    unsigned int nextFat;
    unsigned int address = boot.BootSectorSize + currentFat*2 + 2;
    FramRead(address, &nextFat, 2);
    return nextFat;
}
void WriteToFat(unsigned int clusterNum, unsigned int data)
{
    Boot boot;
    ReadBootSector(&boot);
    unsigned int address = boot.BootSectorSize + clusterNum*2 + 2;
    FramWrite(address,&data, 2);
}
unsigned int FindFreeClusterNum()
{
    Boot boot;
    ReadBootSector(&boot);
    unsigned int freeClusterNum = 0xFFFF;//there are no free clusters
    unsigned int clusterMeaning;
    int adr = boot.BootSectorSize + 2;
    for(adr; adr < boot.BootSectorSize + boot.FatSectorSize; adr += 2)
    {
        FramRead(adr, &clusterMeaning, 2);
        if(clusterMeaning == 0)
        {
            freeClusterNum = (adr - boot.BootSectorSize)>>1;
            freeClusterNum--;
            break;
        }
    }
    return freeClusterNum;
}
unsigned int AllocateClustersSequence(unsigned char clustersCount)
{
    unsigned int freeClusterNum;
    unsigned int prevFreeClusterNum;
    unsigned int firstFreeClusterNum;
    int i = 0;
    for(i; i < clustersCount; i++)
    {
        freeClusterNum = FindFreeClusterNum();
        if(freeClusterNum == 0xFFFF) //all clusters are busy
            return freeClusterNum;
        if(i!=0)
            WriteToFat(prevFreeClusterNum, freeClusterNum);
        else
            firstFreeClusterNum = freeClusterNum;
        WriteToFat(freeClusterNum, 0xFFFF); // end of sequence
        prevFreeClusterNum = freeClusterNum;
    }
   return firstFreeClusterNum;
}
void ClearClustersSequence(unsigned int firstClusterNum)
{
    Boot boot;
    ReadBootSector(&boot);
    unsigned int currentFat = firstClusterNum;
    unsigned int nextFat;
    while(1)
    {
       nextFat = ReadNextFat(currentFat);
       WriteToFat(currentFat, 0);
       currentFat = nextFat;
       if(nextFat == 0xFFFF)
           break;
    }
}
unsigned int FindFreeDescriptorAdr()
{
    Boot boot;
    ReadBootSector(&boot);
    unsigned int freeDescriptorAdr = 0xFFFF;//there are no free descriptors
    char descriptorFirstByte[3];
    int adr = boot.BootSectorSize + boot.FatSectorSize;
    for(adr; adr < boot.BootSectorSize + boot.FatSectorSize + boot.DescriptorSectorSize; adr += boot.DescriptorSize)
    {
        FramRead(adr, &descriptorFirstByte, 3);
        if(descriptorFirstByte[0] == 0x0F)//extended
        {
            unsigned int extCount = descriptorFirstByte[1] + (descriptorFirstByte[2]<<8);
            adr+=extCount*boot.DescriptorSize;
        }
        else if(descriptorFirstByte[0] == 0)
        {
            freeDescriptorAdr = adr;
            break;
        }
    }
    return freeDescriptorAdr;
}
void WriteDescriptorByAddress(unsigned int address, char* data)
{
    Boot boot;
    ReadBootSector(&boot);
    FramWrite(address, data, boot.DescriptorSize);
}
void ReadDescriptorByAddress(unsigned int address, char* data)
{
    Boot boot;
    ReadBootSector(&boot);
    FramRead(address, data, boot.DescriptorSize);
}
unsigned char DescriptorsDefragmentation()
{
    Boot boot;
    ReadBootSector(&boot);
    unsigned int freeDescriptorAdr = 0xFFFF;
    unsigned int busyDescriptorAdr = 0xFFFF;
    char descriptorFirstByte;
    char zero = 0;
    unsigned int adr = boot.BootSectorSize + boot.FatSectorSize;
    char  bufDescriptor[9];// 9 - descriptor size
    for(adr; adr < boot.BootSectorSize + boot.FatSectorSize + boot.DescriptorSectorSize; adr += boot.DescriptorSize)
    {
        FramRead(adr, &descriptorFirstByte, 1);
        if(descriptorFirstByte == 0 && freeDescriptorAdr == 0xFFFF)
        {
            freeDescriptorAdr = adr;
        }
        if(descriptorFirstByte != 0 && freeDescriptorAdr != 0xFFFF)
        {
            busyDescriptorAdr = adr;
            break;
        }
    }
    if(busyDescriptorAdr == 0xFFFF)//if all descriptors, finded after fist free descriptor, are free
        return 0;
    while(busyDescriptorAdr < (boot.BootSectorSize + boot.FatSectorSize + boot.DescriptorSectorSize))
    {
        ReadDescriptorByAddress(busyDescriptorAdr, bufDescriptor);
        if(bufDescriptor[0] == 0)
            return 1;
        WriteDescriptorByAddress(freeDescriptorAdr, bufDescriptor);
        FramWrite(busyDescriptorAdr, &zero, 1); // 0
        busyDescriptorAdr += boot.DescriptorSize;
        freeDescriptorAdr += boot.DescriptorSize;
    }
    return 1;
}
void WriteDataToCluster(unsigned int clusterNum, char* data)
{
    Boot boot;
    ReadBootSector(&boot);
    unsigned int adr = boot.BootSectorSize + boot.FatSectorSize + boot.DescriptorSectorSize + clusterNum * boot.ClusterSize;
    FramWrite(adr, data, boot.ClusterSize);
}
void ReadDataFromCluster(unsigned int clusterNum, char* data)
{
    Boot boot;
    ReadBootSector(&boot);
    unsigned int adr = boot.BootSectorSize + boot.FatSectorSize + boot.DescriptorSectorSize + clusterNum * boot.ClusterSize;
    FramRead(adr, data, boot.ClusterSize);
}