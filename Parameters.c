#include <math.h>
#include <string.h>
#include "Fat.h"
#include "Parameters.h"
#include "CanOpen.h"



char AddParameter(char* name, char type, char* data, unsigned int dataLength)
{
    Boot boot;
    ReadBootSector(&boot);
    ParamDescriptor paramDescriptor;
    paramDescriptor.parameterType = type;
    paramDescriptor.index = FindMaxIndex()+1;
    double clustersCount = (double)dataLength/boot.ClusterSize;
    clustersCount = ceil(clustersCount);
    //fat (clusters sequence)
    unsigned int clusterNum = AllocateClustersSequence(clustersCount);
    if(clusterNum==0xFFFF)
        return 0;
    paramDescriptor.startCluster = clusterNum;
    //data area
    int i=0;
    for(i;i<clustersCount;i++)
    {
        WriteDataToCluster(clusterNum,data+i*boot.ClusterSize);
        clusterNum = ReadNextFat(clusterNum);
    }
    //find free descriptor adr
    unsigned int freeDescriptorAdr = FindFreeDescriptorAdr();
    if(freeDescriptorAdr == 0xFFFF)
        return 0;
    //name + write descriptor
    if(strlen(name)<4)//simple descriptor
    {
        paramDescriptor.paramName[0] = 0;
        strcat(paramDescriptor.paramName,name);
        WriteDescriptorByAddress(freeDescriptorAdr,&paramDescriptor);
        return 1;
    }
    //extended descriptors
    paramDescriptor.paramName[0] = 0x0F;//exteded descripto sign
    //count extended descriptors num
    double buf = (double)(strlen(name) + 1)/boot.DescriptorSize;
    unsigned int extendedDescriptorsCount = (int)ceil(buf);
    char *ch = &extendedDescriptorsCount;
    paramDescriptor.paramName[1] = ch[0];
    paramDescriptor.paramName[2] = ch[1];
    paramDescriptor.paramName[3] = 0;
    WriteDescriptorByAddress(freeDescriptorAdr,&paramDescriptor);
    i=0;
    ExtendedParamDescriptor extendedDescriptor;
    for(i;i<extendedDescriptorsCount;i++)
    {
        freeDescriptorAdr += boot.DescriptorSize;
        int j=0;
        for(j;j<boot.DescriptorSize;j++)
            extendedDescriptor.paramName[j] = name[i*boot.DescriptorSize + j];
        WriteDescriptorByAddress(freeDescriptorAdr,&extendedDescriptor);
    }
    return 1;
}

char DeleteParameter(unsigned int paramIndex)
{
    char zero = 0;
    Boot boot;
    ReadBootSector(&boot);
    unsigned int descriptorAdr = FindDescriptorAdrByIndex(paramIndex);
    ParamDescriptor paramDescriptor;
    ReadDescriptorByAddress(descriptorAdr, &paramDescriptor);
    //clear data
    ClearClustersSequence(paramDescriptor.startCluster);
    //clear head descriptor
    if(paramDescriptor.paramName[0] == 0x0F)
    {
        int extendedDescriptorCount = paramDescriptor.paramName[1] + paramDescriptor.paramName[2]<<8;
        int i = 1;
        for(i; i < (extendedDescriptorCount + 1); i++)
        {
            FramWrite(descriptorAdr + i*boot.DescriptorSize, &zero, sizeof(char));
        }
    }
    FramWrite(descriptorAdr, &zero, sizeof(char));
    DescriptorsDefragmentation();
}

unsigned int ReadParameterValue(unsigned int paramIndex, char* data)
{
    Boot boot;
    ReadBootSector(&boot);
    ParamDescriptor paramDescriptor;
    unsigned int descriptorAdr = FindDescriptorAdrByIndex(paramIndex);
    if(descriptorAdr==0xFFFF)//there are no parameter with such index
        return 0;
    ReadDescriptorByAddress(descriptorAdr, &paramDescriptor);
    unsigned int currentCluster = paramDescriptor.startCluster;
    int i = 0;
    while(1)
    {
        ReadDataFromCluster(currentCluster, data+i*boot.ClusterSize);
        currentCluster = ReadNextFat(currentCluster);
        if(currentCluster == 0xFFFF)
            break;
        i++;
    }
    return (i+1)*boot.ClusterSize;
}

unsigned char ReadParameterName(unsigned int paramIndex, char* name)
{
    Boot boot;
    ReadBootSector(&boot);
    ParamDescriptor paramDescriptor;
    ExtendedParamDescriptor extendedParamDescriptor;
    unsigned int descriptorAdr = FindDescriptorAdrByIndex(paramIndex);
    if(descriptorAdr == 0xFFFF)//there are no parameter with such index
        return 0;
    ReadDescriptorByAddress(descriptorAdr, &paramDescriptor);
    if(paramDescriptor.paramName[0] == 0x0F)
    {
        unsigned int extendedDescriptorCount = paramDescriptor.paramName[1] + (paramDescriptor.paramName[2]<<8);
        unsigned int i = 1;
        for(i; i < (extendedDescriptorCount + 1); i++)
        {
            ReadDescriptorByAddress(descriptorAdr + boot.DescriptorSize*i, &extendedParamDescriptor);
            int j=0;
            for(j;j<boot.DescriptorSize;j++)
                name[(i-1)*boot.DescriptorSize + j] = extendedParamDescriptor.paramName[j];
        }
    }
    else
    {
        name[0] = 0;
        strcat(name,paramDescriptor.paramName);
    }
    return strlen(name);
}

char ReadParameterType(unsigned int paramIndex)
{
    Boot boot;
    ReadBootSector(&boot);
    unsigned int descriptorAdr = FindDescriptorAdrByIndex(paramIndex);
    if(descriptorAdr == 0xFFFF)//there are no descriptors with such index
        return -1;
    ParamDescriptor paramDescriptor;
    ReadDescriptorByAddress(descriptorAdr, &paramDescriptor);
    return paramDescriptor.parameterType;
}

void EditParameterValue(unsigned int paramIndex, char* value, unsigned int dataLength)
{
    Boot boot;
    ReadBootSector(&boot);
    ParamDescriptor paramDescriptor;
    unsigned int descriptorAdr = FindDescriptorAdrByIndex(paramIndex);
    ReadDescriptorByAddress(descriptorAdr, &paramDescriptor);
    unsigned int currentCluster = paramDescriptor.startCluster;
    double clustersCount = (double)dataLength/boot.ClusterSize;
    clustersCount = ceil(clustersCount);
    int i = 0;
    for(i;i<clustersCount;i++)
    {
        WriteDataToCluster(currentCluster, value+i*boot.ClusterSize);
        currentCluster = ReadNextFat(currentCluster);
    }
}

char EditParameterName(unsigned int paramIndex, char* name)
{
    Boot boot;
    ReadBootSector(&boot);
    char zero = 0;
    ParamDescriptor paramDescriptor;
    unsigned int descriptorAdr = FindDescriptorAdrByIndex(paramIndex);
    ReadDescriptorByAddress(descriptorAdr, &paramDescriptor);
    //deleting of descriptors
    if(paramDescriptor.paramName[0] == 0x0F)
    {
        int extendedDescriptorCount = paramDescriptor.paramName[1] + paramDescriptor.paramName[2]<<8;
        int i = 1;
        for(i; i < (extendedDescriptorCount + 1); i++)
        {
            FramWrite(descriptorAdr + i*boot.DescriptorSize, &zero, sizeof(char));
        }
    }
    FramWrite(descriptorAdr, &zero, sizeof(char));
    DescriptorsDefragmentation();
    unsigned int freeDescriptorAdr = FindFreeDescriptorAdr();
    if(freeDescriptorAdr == 0xFFFF)
        return 0;
    //name + write descriptor
    if(strlen(name)<4)//simple descriptor
    {
        paramDescriptor.paramName[0] = 0;
        strcat(paramDescriptor.paramName,name);
        WriteDescriptorByAddress(freeDescriptorAdr,&paramDescriptor);
        return 1;
    }
    //extended descriptors
    paramDescriptor.paramName[0] = 0x0F;//exteded descripto sign
    //count extended descriptors num
    double buf = (double)(strlen(name) + 1)/boot.DescriptorSize;
    unsigned int extendedDescriptorsCount = (int)ceil(buf);
    char *ch = &extendedDescriptorsCount;
    paramDescriptor.paramName[1] = ch[0];
    paramDescriptor.paramName[2] = ch[1];
    paramDescriptor.paramName[3] = 0;
    WriteDescriptorByAddress(freeDescriptorAdr,&paramDescriptor);
    int ii=0;
    ExtendedParamDescriptor extendedDescriptor;
    for(ii;ii<extendedDescriptorsCount;ii++)
    {
        freeDescriptorAdr += boot.DescriptorSize;
        int j=0;
        for(j;j<boot.DescriptorSize;j++)
            extendedDescriptor.paramName[j] = name[ii*boot.DescriptorSize + j];
        WriteDescriptorByAddress(freeDescriptorAdr,&extendedDescriptor);
    }
    return 1;
}

unsigned int FindMaxIndex()
{
    Boot boot;
    ReadBootSector(&boot);
    unsigned int maxIndex = 0;
    ParamDescriptor descriptor;
    int adr = boot.BootSectorSize + boot.FatSectorSize;
    for(adr; adr < boot.BootSectorSize + boot.FatSectorSize + boot.DescriptorSectorSize;
            adr += boot.DescriptorSize)
    {
        ReadDescriptorByAddress(adr, &descriptor);
        if(descriptor.paramName[0] == 0)
            break;
        if(maxIndex < descriptor.index)
            maxIndex = descriptor.index;
        if(descriptor.paramName[0] == 0x0F)//extended
        {
            unsigned int extCount = descriptor.paramName[1] + (descriptor.paramName[2]<<8);
            adr+=extCount*boot.DescriptorSize;
            continue;
        }                 
    }
    if(maxIndex == 0)
        return 0x1FFF;
    return maxIndex;
}

unsigned int FindDescriptorAdrByIndex(unsigned int index)
{
    Boot boot;
    ReadBootSector(&boot);
    unsigned int descriptorAdr = 0xFFFF;//there are no descriptor with such index
    ParamDescriptor descriptor;
    int adr = boot.BootSectorSize + boot.FatSectorSize;
    for(adr; adr < boot.BootSectorSize + boot.FatSectorSize + boot.DescriptorSectorSize; adr += boot.DescriptorSize)
    {
        ReadDescriptorByAddress(adr, &descriptor);
        if(descriptor.index == index)
        {
            descriptorAdr = adr;
            break;
        } 
        if(descriptor.paramName[0] == 0x0F)//extended
        {
            unsigned int extCount = descriptor.paramName[1] + (descriptor.paramName[2]<<8);
            adr+=extCount*boot.DescriptorSize;
            continue;
        }
        if(descriptor.paramName[0] == 0)
            break;
        
    }
    return descriptorAdr;
}