#include "mbed.h"

#define ARQMSG_TYPE_DATA    0
#define ARQMSG_TYPE_ACK     1 //에크인지 데이터 인지 구분하기 위한 상수

#define ARQMSG_OFFSET_TYPE  0
#define ARQMSG_OFFSET_SEQ   1
#define ARQMSG_OFFSET_DATA  2  //

#define ARQMSG_ACKSIZE      3   //에크 사이즈

#define ARQMSG_MAXDATASIZE  26   //맥스 데이터 사이즈
#define ARQMSSG_MAX_SEQNUM  1024 //맥스 시퀀스


int arqMsg_checkIfData(uint8_t* msg);
int arqMsg_checkIfAck(uint8_t* msg);
uint8_t arqMsg_encodeAck(uint8_t* msg_ack, uint8_t seq);
uint8_t arqMsg_encodeData(uint8_t* msg_data, uint8_t* data, int seq, int len);
uint8_t arqMsg_getSeq(uint8_t* msg);
uint8_t* arqMsg_getWord(uint8_t* msg);