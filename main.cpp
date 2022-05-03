#include "mbed.h"
#include "string.h"

#include "ARQ_FSMevent.h"
#include "ARQ_msg.h"
#include "ARQ_timer.h"
#include "ARQ_LLinterface.h"
#include "ARQ_parameters.h"

//FSM state -------------------------------------------------
#define MAINSTATE_IDLE              0
#define MAINSTATE_TX                1

//GLOBAL variables (DO NOT TOUCH!) ------------------------------------------
//serial port interface
Serial pc(USBTX, USBRX);//시리얼 객체를 생성해서 아두이노 통신을 셋업 시작함.

//state variables
uint8_t main_state = MAINSTATE_IDLE; //protocol state

//source/destination ID
uint8_t endNode_ID=1;//초기화되어있는 아이디
uint8_t dest_ID=0;

//PDU context/size
uint8_t arqPdu[200];
uint8_t pduSize;

//SDU (input)
uint8_t originalWord[200];
uint8_t wordLen=0;



//ARQ parameters -------------------------------------------------------------
uint8_t seqNum = 0;     //ARQ sequence number
uint8_t retxCnt = 0;    //ARQ retransmission counter
uint8_t arqAck[5];      //ARQ ACK PDU


//application event handler : generating SDU from keyboard input
void arqMain_processInputWord(void)//2
{
    char c = pc.getc();//스트림에서 문자를 읽어옴.
    if (main_state == MAINSTATE_IDLE &&
        !arqEvent_checkEventFlag(arqEvent_dataToSend))//메인상태가 수신이 가능한 idel 이고, 보낼게 있다는 이벤트 플레그가 보낼게 있다는 것과 동일하지 않다면,
    {
        if (c == '\n' || c == '\r')//입력문자중에 엔터나 케리지 리턴 문자가 포함되어 있다면
        {
            originalWord[wordLen++] = '\0';//저장된 문자열 끝에 널문자를 삽입해서 문자열을 생성한다.
            arqEvent_setEventFlag(arqEvent_dataToSend);//에벤트 플래그에 저장된 값을 바꾼다.#####3
            pc.printf("word is ready! ::: %s\n", originalWord);
        }
        else//엔터나 케리지 리턴 문자가 아니라면,
        {
            originalWord[wordLen++] = c;//문자를 오리지널 문자배열에 추가함,
            if (wordLen >= ARQMSG_MAXDATASIZE-1)//전송하는 문자열의 길이가 최대 전송길이 보다 길다면,
            {
                originalWord[wordLen++] = '\0';//끝에 문자열을 널문자를 삽압해서 문자열을 완성하고
                arqEvent_setEventFlag(arqEvent_dataToSend);//이벤트 플래그를 전송준비가 완료된것으로 변경한다.
                pc.printf("\n max reached! word forced to be ready :::: %s\n", originalWord);//유저에게 길이를 넘어서 강제로 생성된 문자를 출력해서 알린다.
            }
        }
    }
}




//FSM operation implementation ------------------------------------------------
int main(void){
    uint8_t flag_needPrint=1;//flag_needPrint라는 변수에 1을 할당한다
    uint8_t prev_state = 0;// prev_state라는 변수에 0을 할당한다

    //initialization
    pc.printf("------------------ ARQ protocol starts! --------------------------\n");//쁘띠에 출력pc는 자세하게 알필요가 없어용
    arqEvent_clearAllEventFlag();//함수를 호출해서 eventflag라는 변수에 0을 할당한다

    //source & destination ID setting
    pc.printf(":: ID for this node : ");//터널에서 node 아이디를 받는다
    pc.scanf("%d", &endNode_ID);//ende node 아이디를 유저로부터 입력받는데 이번 경우는 1을 입력받는다.
    pc.printf(":: ID for the destination : ");
    pc.scanf("%d", &dest_ID);//통신상대의 아이디인 2를 입력받는다.
    pc.getc();//표준입력버퍼에서 자하나를 가져온다.

    pc.printf("endnode : %i, dest : %i\n", endNode_ID, dest_ID);//설정된 node아이디와 dest아이디를 출력한다.

    arqLLI_initLowLayer(endNode_ID);//STRX레이어 에서 LOWER실제로 데이터를 전송하는 레이어로 ennodeID를 전달함.
    pc.attach(&arqMain_processInputWord, Serial::RxIrq);//콜백함수이기 때문에 키보드에 입력이 들어오면 실행된다.





    while(1)
    {
        //debug message
        if (prev_state != main_state)//위에서 메인함수 처음으로 실행할할 때 prev_state를 0으로 초기화했고 위에서 main state도 0으로 초기화되어있음
        {//prev_state와 mainstate가 다르다면
            debug_if(DBGMSG_ARQ, "[ARQ] State transition from %i to %i\n", prev_state, main_state);
            prev_state = main_state;//둘의 상태를 같게 만들어줌
        }


        //FSM should be implemented here! ---->>>>
        switch (main_state)//메인 상태에 따라서 다르게 동작한다.
        {
            case MAINSTATE_IDLE: //IDLE state description// main state가 IDLE 상태라면이고 수신대기중일때
                
                if (arqEvent_checkEventFlag(arqEvent_dataRcvd)) //if data reception event happens 데이터 수신이 일어나면
                {
                    //Retrieving data info.
                    uint8_t srcId = arqLLI_getSrcId();//수신한 src의 아이디를 반환하는 함수를 이용해서 scrId 변수에 저장한다.
                    uint8_t* dataPtr = arqLLI_getRcvdDataPtr();//수신한 data의 출력할 부분을 받는다.
                    uint8_t size = arqLLI_getSize();//입력받은 데이터의 크기를 반환한다.

                    pc.printf("\n -------------------------------------------------\nRCVD from %i : %s (length:%i, seq:%i)\n -------------------------------------------------\n", 
                                srcId, arqMsg_getWord(dataPtr), size, arqMsg_getSeq(dataPtr));
4
                    main_state = MAINSTATE_IDLE;//다시 수신대기 상태로 만든다.
                    flag_needPrint = 1;//출력될 데이터가 있다는 의미인 변수를 1로 만들어 준다.

                    arqEvent_clearEventFlag(arqEvent_dataRcvd);//arqEvent_dataRcvd =3 인데 이때 arqEvent_clearEventFlag 메서드 안에 들어있는
                    //비트 연산의 결과로 16진수 을 비트연산을 수행한과를 반환하는데
                    //eventFlag &= ~(0x01 << event);   근데 초기에 0으로 설정했기 때문에 다시 eventFlag변수에 0이 할당된다.
                }
                else if (arqEvent_checkEventFlag(arqEvent_dataToSend)) //if data needs to be sent (keyboard input)
                {//    arqEvent_dataToSend = 4,

                    //msg header setting
                    pduSize = arqMsg_encodeData(arqPdu, originalWord, seqNum, wordLen);//생성되는 pdu의 사이즈를 반환한다.
                    arqLLI_sendData(arqPdu, pduSize, dest_ID);

                    pc.printf("[MAIN] sending to %i (seq:%i)\n", dest_ID, (seqNum-1)%ARQMSSG_MAX_SEQNUM);//ARQMSSG_MAX_SEQNUM 로 모듈레이션 연산을 수행해준다.

                    main_state = MAINSTATE_TX;//메인상태를 송신으로 전환하는 것으 ㄹ실행한다
                    flag_needPrint = 1;//출력데이터를 표시하기 위해서 변수에 할당된 정수를 변환해 준다.

                    wordLen = 0;//단어의 길이를 0으로 젼환한다.메세지를 전성한 후에 배열을 초기화 하는 연산을 수행함
                    arqEvent_clearEventFlag(arqEvent_dataToSend);//
                }
                else if (flag_needPrint == 1)//1
                {
                    pc.printf("Give a word to send : ");
                    flag_needPrint = 0;
                }     

                break;

            case MAINSTATE_TX: //IDLE state description

                if (arqEvent_checkEventFlag(arqEvent_dataTxDone)) //data TX finished
                {
                    main_state = MAINSTATE_IDLE;
                    arqEvent_clearEventFlag(arqEvent_dataTxDone);
                }

                break;

            default :
                break;
        }
    }
}