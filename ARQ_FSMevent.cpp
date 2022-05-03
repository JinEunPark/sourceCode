#include "mbed.h"
#include "ARQ_FSMevent.h"

static uint32_t eventFlag;


void arqEvent_setEventFlag(arqEvent_e event)// 이벤트 플래그를 설정하는 함수
{
    eventFlag |= (0x01 << event);//초기에 이벤트 플래그
}

void arqEvent_clearEventFlag(arqEvent_e event)//이벤트 플래그를 초기화하는 함수
{
    eventFlag &= ~(0x01 << event);//eventFlag에 비트연산을 적용한 결과를 저장한다.
}
void arqEvent_clearAllEventFlag(void)//모든 이벤트 플레그를 0 으로 초기화하는 함수
{
    eventFlag = 0;
}

int arqEvent_checkEventFlag(arqEvent_e event)//현재발생하는 이벤트를 확인하는 함수
{
    return (eventFlag & (0x01 << event));
}