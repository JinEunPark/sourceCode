#include "mbed.h"
#include "ARQ_msg.h"

int arqMsg_checkIfData(uint8_t* msg)//수신한 메세지를 확인하는 메소드
{
    return (msg[ARQMSG_OFFSET_TYPE] == ARQMSG_TYPE_DATA);
}

int arqMsg_checkIfAck(uint8_t* msg)//수신한 데이터가 아크 인지불리언 타입을 반환하는 메소드
{
    return (msg[ARQMSG_OFFSET_TYPE] == ARQMSG_TYPE_ACK);//
}

uint8_t arqMsg_encodeAck(uint8_t* msg_ack, uint8_t seq)
{
    msg_ack[ARQMSG_OFFSET_TYPE] = ARQMSG_TYPE_ACK;//전송하는 배열의 첫번째 element에 에크를 확인하는 상수를 집어 넣는다
    msg_ack[ARQMSG_OFFSET_SEQ] = seq;//전송하는 배열의 두번째 요소에 시퀀스 넘버를 삽입해준다.
    msg_ack[ARQMSG_OFFSET_DATA] = 1;

    return ARQMSG_ACKSIZE;
}

uint8_t arqMsg_encodeData(uint8_t* msg_data, uint8_t* data, int seq, int len)
{
    msg_data[ARQMSG_OFFSET_TYPE] = ARQMSG_TYPE_DATA;
    msg_data[ARQMSG_OFFSET_SEQ] = seq;
    memcpy(&msg_data[ARQMSG_OFFSET_DATA], data, len*sizeof(uint8_t));

    return len+ARQMSG_OFFSET_DATA;
}
                    

uint8_t arqMsg_getSeq(uint8_t* msg)
{
    return msg[ARQMSG_OFFSET_SEQ];
}

uint8_t* arqMsg_getWord(uint8_t* msg)
{
    return &msg[ARQMSG_OFFSET_DATA];
}