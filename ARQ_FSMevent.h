typedef enum arqEvent
{
    arqEvent_dataTxDone = 0,//데이터를 보냈다
    arqEvent_ackTxDone = 1,//데이터를 보내고 아크를 받는 과정이 끝났다
    arqEvent_ackRcvd = 2,//에크의 수신이 끝났다
    arqEvent_dataRcvd = 3,//데이터를 받았다
    arqEvent_dataToSend = 4,//데이터를 보낼것이 발생했을 때 발생하는 함수
    arqEvent_arqTimeout = 5//아크를 받는 타이머가 시간이 지났을 때 발생하는 이벤트.
} arqEvent_e;


void arqEvent_setEventFlag(arqEvent_e event);
void arqEvent_clearEventFlag(arqEvent_e event);
void arqEvent_clearAllEventFlag(void);
int arqEvent_checkEventFlag(arqEvent_e event);