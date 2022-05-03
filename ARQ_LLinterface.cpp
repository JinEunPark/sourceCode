#include "mbed.h"
#include "PHYMAC_layer.h"
#include "ARQ_FSMevent.h"
#include "ARQ_msg.h"
#include "ARQ_parameters.h"

#define ARQLLI_MAX_PDUSIZE      50

static uint8_t txType;
static uint8_t rcvdData[ARQLLI_MAX_PDUSIZE];
static uint8_t rcvdSrc;
static uint8_t rcvdSize;

//interface event : DATA_CNF, TX done event
void arqLLI_dataCnfFunc(int err) 
{
    if (txType == ARQMSG_TYPE_DATA)//송신하는 측의 데이터 타입이 데이터일 경우에
    {
        arqEvent_setEventFlag(arqEvent_dataTxDone);//arqEvent_dataTxDone을 현제 이벤트로 설정함
    }
    else if (txType == ARQMSG_TYPE_ACK)//송신하는 측의 데이터가 아크일 경우에
    {
        arqEvent_setEventFlag(arqEvent_ackTxDone);//에크를 수신하면 에크를 수신한 이벤트로 전환한다.
    }
}

//interface event : DATA_IND, RX data has arrived
void arqLLI_dataIndFunc(uint8_t srcId, uint8_t* dataPtr, uint8_t size)
{
    debug_if(DBGMSG_ARQ, "\n --> DATA IND : src:%i, size:%i\n", srcId, size);

    memcpy(rcvdData, dataPtr, size*sizeof(uint8_t));
    rcvdSrc = srcId;
    rcvdSize = size;

    //ready for ACK TX
    if (arqMsg_checkIfData(dataPtr))
    {
        arqEvent_setEventFlag(arqEvent_dataRcvd);
    }
    else if (arqMsg_checkIfAck(dataPtr))
    {
        arqEvent_setEventFlag(arqEvent_ackRcvd);
    }
}


void arqLLI_initLowLayer(uint8_t srcId)
{
    phymac_init(srcId, arqLLI_dataCnfFunc, arqLLI_dataIndFunc);//와 솔직히 이 함수가 뭐하는 건지
    //모르겠습니다 교수님
}




//TX function
void arqLLI_sendData(uint8_t* msg, uint8_t size, uint8_t dest)
{
    phymac_dataReq(msg, size, dest);
    txType = msg[ARQMSG_OFFSET_TYPE];//전송하는 데이터 타입을 지정해줌
}


//GET functions
uint8_t arqLLI_getSrcId()//수신한 데이터의 아이디를 반환하는 함수
{
    return rcvdSrc;
}

uint8_t* arqLLI_getRcvdDataPtr()//수신한 pdu에서 데이터를 반환하는 함수 입니다.
{
    return rcvdData;
}

uint8_t arqLLI_getSize()
{
    return rcvdSize;//반환한 pdu의 사이즈를 반환하는 함수
}