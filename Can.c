#include <can.h>
#include "Can.h"

//globals vars
extern int _nodeId;
unsigned char queueSize = 10;
unsigned char bufCount = 3;

//can1
unsigned char isTeilCan1OverCome[3] = {0,0,0};
char can1DataBuf[3][10][8];
int sidCan1Data[3][10];
unsigned char headCan1[3] = {0,0,0};
unsigned char teilCan1[3] = {0,0,0};

//can2
unsigned char isTeilCan2OverCome[3] = {0,0,0};
char can2DataBuf[3][10][8];
int sidCan2Data[3][10];
unsigned char headCan2[3] = {0,0,0};
unsigned char teilCan2[3] = {0,0,0};

void Can1Initialization(void)
{
//Set request for configuration Mode
	CAN1SetOperationMode(CAN_IDLE_CON & CAN_MASTERCLOCK_1 & CAN_REQ_OPERMODE_CONFIG & CAN_CAPTURE_DIS);
	while(C1CTRLbits.OPMODE<=3);

//Load Configuration register

	CAN1Initialize(CAN_SYNC_JUMP_WIDTH1 & CAN_BAUD_PRE_SCALE(BAUD_RATE),CAN_WAKEUP_BY_FILTER_DIS & CAN_PHASE_SEG2_TQ(6) & CAN_PHASE_SEG1_TQ(8) &
					CAN_PROPAGATIONTIME_SEG_TQ(1) & CAN_SEG2_FREE_PROG & CAN_SAMPLE1TIME);

        // Set interrupts
   ConfigIntCAN1(CAN_INDI_INVMESS_DIS &
             CAN_INDI_WAK_DIS &
             CAN_INDI_ERR_DIS &
             CAN_INDI_TXB2_DIS &
             CAN_INDI_TXB1_DIS &
             CAN_INDI_TXB0_DIS &
             CAN_INDI_RXB1_EN &
             CAN_INDI_RXB0_EN ,
             CAN_INT_PRI_2 &
             CAN_INT_ENABLE);
//Set Filters
    long node_id = _nodeId;
    CAN1SetFilter(0, CAN_FILTER_SID(0x600+node_id) & CAN_RX_EID_DIS, CAN_FILTER_EID(00000));
    CAN1SetMask(0, CAN_MASK_SID(0x7FF) & CAN_MATCH_FILTER_TYPE, CAN_MASK_EID(00000));
    CAN1SetFilter(1, CAN_FILTER_SID(0x600+node_id) & CAN_RX_EID_DIS, CAN_FILTER_EID(00000));
    CAN1SetMask(1, CAN_MASK_SID(0x7FF) & CAN_MATCH_FILTER_TYPE, CAN_MASK_EID(00000));
    CAN1SetFilter(2, CAN_FILTER_SID(0x600+node_id) & CAN_RX_EID_DIS, CAN_FILTER_EID(00000));
    CAN1SetMask(2, CAN_MASK_SID(0x7FF) & CAN_MATCH_FILTER_TYPE, CAN_MASK_EID(00000));
//Set transmitter and receiver mode
    CAN1SetTXMode(0, CAN_TX_STOP_REQ & CAN_TX_PRIORITY_HIGH);
    CAN1SetRXMode(0, CAN_RXFUL_CLEAR & CAN_BUF0_DBLBUFFER_EN);
    CAN1SetTXMode(1, CAN_TX_STOP_REQ & CAN_TX_PRIORITY_HIGH);
    CAN1SetRXMode(1, CAN_RXFUL_CLEAR & CAN_BUF0_DBLBUFFER_EN);
    CAN1SetTXMode(2, CAN_TX_STOP_REQ & CAN_TX_PRIORITY_HIGH);
    CAN1SetRXMode(2, CAN_RXFUL_CLEAR & CAN_BUF0_DBLBUFFER_EN);

//Set request for normal mode

	CAN1SetOperationMode(CAN_IDLE_STOP & CAN_CAPTURE_EN & CAN_MASTERCLOCK_1 & CAN_REQ_OPERMODE_NOR);
	while(C1CTRLbits.OPMODE != 0);
}

void Can2Initialization(void)
{
//Set request for configuration Mode
	CAN2SetOperationMode(CAN_IDLE_CON & CAN_MASTERCLOCK_1 & CAN_REQ_OPERMODE_CONFIG & CAN_CAPTURE_DIS);
	while(C2CTRLbits.OPMODE<=3);

//Load Configuration register

	CAN2Initialize(CAN_SYNC_JUMP_WIDTH1 & CAN_BAUD_PRE_SCALE(BAUD_RATE),CAN_WAKEUP_BY_FILTER_DIS & CAN_PHASE_SEG2_TQ(6) & CAN_PHASE_SEG1_TQ(8) &
					CAN_PROPAGATIONTIME_SEG_TQ(1) & CAN_SEG2_FREE_PROG & CAN_SAMPLE1TIME);

        // Set interrupts
   ConfigIntCAN2(CAN_INDI_INVMESS_DIS &
             CAN_INDI_WAK_DIS &
             CAN_INDI_ERR_DIS &
             CAN_INDI_TXB2_DIS &
             CAN_INDI_TXB1_DIS &
             CAN_INDI_TXB0_DIS &
             CAN_INDI_RXB1_EN &
             CAN_INDI_RXB0_EN ,
             CAN_INT_PRI_2 &
             CAN_INT_ENABLE);
//Set Filters
    long node_id = _nodeId;
    CAN2SetFilter(0, CAN_FILTER_SID(0x600+node_id) & CAN_RX_EID_DIS, CAN_FILTER_EID(00000));
    CAN2SetMask(0, CAN_MASK_SID(0x7FF) & CAN_MATCH_FILTER_TYPE, CAN_MASK_EID(00000));
    CAN2SetFilter(1, CAN_FILTER_SID(0x600+node_id) & CAN_RX_EID_DIS, CAN_FILTER_EID(00000));
    CAN2SetMask(1, CAN_MASK_SID(0x7FF) & CAN_MATCH_FILTER_TYPE, CAN_MASK_EID(00000));
    CAN2SetFilter(2, CAN_FILTER_SID(0x600+node_id) & CAN_RX_EID_DIS, CAN_FILTER_EID(00000));
    CAN2SetMask(2, CAN_MASK_SID(0x7FF) & CAN_MATCH_FILTER_TYPE, CAN_MASK_EID(00000));
//Set transmitter and receiver mode
    CAN2SetTXMode(0, CAN_TX_STOP_REQ & CAN_TX_PRIORITY_HIGH);
    CAN2SetRXMode(0, CAN_RXFUL_CLEAR & CAN_BUF0_DBLBUFFER_EN);
    CAN2SetTXMode(1, CAN_TX_STOP_REQ & CAN_TX_PRIORITY_HIGH);
    CAN2SetRXMode(1, CAN_RXFUL_CLEAR & CAN_BUF0_DBLBUFFER_EN);
    CAN2SetTXMode(2, CAN_TX_STOP_REQ & CAN_TX_PRIORITY_HIGH);
    CAN2SetRXMode(2, CAN_RXFUL_CLEAR & CAN_BUF0_DBLBUFFER_EN);

//Set request for normal mode

	CAN2SetOperationMode(CAN_IDLE_STOP & CAN_CAPTURE_EN & CAN_MASTERCLOCK_1 & CAN_REQ_OPERMODE_NOR);
	while(C2CTRLbits.OPMODE != 0);
}

void Can1SendData(unsigned int sid, char* data, unsigned char bufNumber)
{
    unsigned char teil = teilCan1[bufNumber];
    IncTeilCan1(bufNumber);
    sidCan1Data[bufNumber][teil] = sid;
    int i = 0;
    for(i;i<8;i++)
        can1DataBuf[bufNumber][teil][i] = data[i];
}

void Can2SendData(unsigned int sid, char* data, unsigned char bufNumber)
{
    unsigned char teil = teilCan2[bufNumber];
    IncTeilCan2(bufNumber);
    sidCan2Data[bufNumber][teil] = sid;
    int i = 0;
    for(i;i<8;i++)
        can2DataBuf[bufNumber][teil][i] = data[i];
}

void Can1Execute()
{
    int bufNumber = 0;
    unsigned int sid;
    char data[8];
    char head;
    for(bufNumber; bufNumber<bufCount; bufNumber++)
    {
        if(IsQueueCan1Empty(bufNumber))
            continue;
        while(!CAN1IsTXReady(bufNumber));
        head = headCan1[bufNumber];
        sid = sidCan1Data[bufNumber][head];
        int j = 0;
        for(j;j<8;j++)
            data[j] = can1DataBuf[bufNumber][head][j];
        //Load message ID, Data into transmit buffer and set transmit request bit
	CAN1SendMessage((CAN_TX_SID(sid)) & (CAN_TX_EID_DIS) & (CAN_SUB_NOR_TX_REQ), (CAN_TX_EID(1)) & (CAN_NOR_TX_REQ), data, 8, bufNumber);
        IncHeadCan1(bufNumber);
    }
}

void Can2Execute()
{
    int bufNumber = 0;
    unsigned int sid;
    char data[8];
    char head;
    for(bufNumber; bufNumber<bufCount; bufNumber++)
    {
        if(IsQueueCan2Empty(bufNumber))
            continue;
        while(!CAN2IsTXReady(bufNumber));
        head = headCan2[bufNumber];
        sid = sidCan2Data[bufNumber][head];
        int j = 0;
        for(j;j<8;j++)
            data[j] = can2DataBuf[bufNumber][head][j];
        //Load message ID, Data into transmit buffer and set transmit request bit
	CAN2SendMessage((CAN_TX_SID(sid)) & (CAN_TX_EID_DIS) & (CAN_SUB_NOR_TX_REQ), (CAN_TX_EID(1)) & (CAN_NOR_TX_REQ), data, 8, bufNumber);
        IncHeadCan2(bufNumber);
    }
}

void Can1ReceiveData(char* data)
{
    CAN1ReceiveMessage(data, 8, 0);
}

void Can2ReceiveData(char* data)
{
    CAN2ReceiveMessage(data, 8, 0);
}

void IncTeilCan1(unsigned char bufNumber)
{
    teilCan1[bufNumber]++;
    if(teilCan1[bufNumber]>=queueSize)
    {
        teilCan1[bufNumber] = 0;
        isTeilCan1OverCome[bufNumber] = 1;
    }
}
void IncHeadCan1(unsigned char bufNumber)
{
    headCan1[bufNumber]++;
    if(headCan1[bufNumber]>=queueSize)
    {
        headCan1[bufNumber]=0;
        isTeilCan1OverCome[bufNumber] = 0;
    }
}
char IsQueueCan1Empty(unsigned char bufNumber)
{
    if(isTeilCan1OverCome[bufNumber] == 1)
        return 0;
    if(headCan1[bufNumber] < teilCan1[bufNumber])
        return 0;
    return 1;
}

void IncTeilCan2(unsigned char bufNumber)
{
    teilCan2[bufNumber]++;
    if(teilCan2[bufNumber]>=queueSize)
    {
        teilCan2[bufNumber] = 0;
        isTeilCan2OverCome[bufNumber] = 1;
    }
}
void IncHeadCan2(unsigned char bufNumber)
{
    headCan2[bufNumber]++;
    if(headCan2[bufNumber]>=queueSize)
    {
        headCan2[bufNumber]=0;
        isTeilCan2OverCome[bufNumber] = 0;
    }
}
char IsQueueCan2Empty(unsigned char bufNumber)
{
    if(isTeilCan2OverCome[bufNumber] == 1)
        return 0;
    if(headCan2[bufNumber] < teilCan2[bufNumber])
        return 0;
    return 1;
}