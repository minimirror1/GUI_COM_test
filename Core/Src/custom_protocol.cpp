#include "main.h"
#include "custom_protocol.h"
#include <cstdlib>  // rand() 함수를 위해 추가

Custom_Protocol::Custom_Protocol(ISerialInterface* serial, ITick* tick)
    : Com_Protocol(serial, tick)
    , lastPingTime_(0)
    , totalRunTime_(0)
    , currentCount_(0)
    , totalCount_(0)
    , voltage_(0)
    , current_(0)
    , mainPowerState_(false)
{
}

Custom_Protocol::~Custom_Protocol()
{
    // 필요한 정리 작업이 있다면 여기에 구현
}

void Custom_Protocol::handlePing(uint16_t senderId, uint8_t* payload, size_t length)
{
    lastPingTime_ = tick_->getTickCount();
    
    // 응답 패킷 준비
    uint8_t response[4];
    uint32_t currentTime = tick_->getTickCount();
    memcpy(response, &currentTime, sizeof(currentTime));
    
    // 응답 전송
    sendData(senderId, 0, CMD_PONG, response, sizeof(response));
}

void Custom_Protocol::handleStatusSync(uint16_t senderId, uint8_t* payload, size_t length)
{
   // 페이로드 길이 체크

	/* 상태 동기화 로직 구현  재정의 하여 측정된 값을 추가*/
    // 메인파워 상태
    uint8_t mainPowerStatus = mainPowerState_; // 1: ON, 0: OFF
    // 모션 재생 상태
    uint8_t motionPlayStatus = motionPlayState_; // 1:1회 재생, 2:반복 재생, 3:일시 정지 , 4:정지
	// 연속 구동시간
	uint32_t totalRunTime = tick_->getTickCount(); // ms 단위의 전체 구동시간 획득
	// 동작 회차
	uint16_t currentCount = 1000; // 현재 동작 회차
	uint16_t totalCount = 2000; // 총 동작 회차
	// 전압
	uint16_t voltage = rand() % 5000; // 0~50.00V 사이의 랜덤값
	// 전류
	uint16_t current = rand() % 4000; // 0~40.00A 사이의 랜덤값
	// 모션 시간
	uint32_t motionCurrentTime = tick_->getTickCount();//rand() % 20000; // 10.00s
    uint32_t motionEndTime = 20000; // 20.00s
    
    // 마지막 에러 정보 추가
    uint8_t f_error = 0;            // 0: 정상, 1: 에러
    uint8_t can_id = 0;             // CAN ID
    uint8_t can_sub_id = 0;         // CAN SUB ID
    uint8_t motor_type = 0;         // 모터 타입
    char error_code_str[8] = {0};   // 8바이트 제한의 에러 코드 문자열
	/************************************************* */



    /*패킷 생성*/
    // 응답 패킷 준비 (29byte로 확장)
    uint8_t responsePayload[29] = {0,};


    // 시간 변환 (ms -> 시/분/초)
    uint8_t hours = totalRunTime / (1000 * 60 * 60);
    uint8_t minutes = (totalRunTime % (1000 * 60 * 60)) / (1000 * 60);
    uint8_t seconds = ((totalRunTime % (1000 * 60 * 60)) % (1000 * 60)) / 1000;

    // 상태 플레그
    responsePayload[0] = mainPowerStatus;
    responsePayload[1] = motionPlayStatus;

    // 시간 정보 입력
    responsePayload[2] = hours;
    responsePayload[3] = minutes;
    responsePayload[4] = seconds;

    // 동작 회차 정보 입력 (현재/총)
    responsePayload[5] = (currentCount >> 8) & 0xFF;
    responsePayload[6] = currentCount & 0xFF;
    responsePayload[7] = (totalCount >> 8) & 0xFF;
    responsePayload[8] = totalCount & 0xFF;

    // 에너지 정보 입력 (전압/전류)
    responsePayload[9] = (voltage >> 8) & 0xFF;
    responsePayload[10] = voltage & 0xFF;
    responsePayload[11] = (current >> 8) & 0xFF;
    responsePayload[12] = current & 0xFF;

    // 모션 시간 입력    
    responsePayload[13] = (motionCurrentTime >> 8) & 0xFF;
    responsePayload[14] = motionCurrentTime & 0xFF;
    responsePayload[15] = (motionEndTime >> 8) & 0xFF;
    responsePayload[16] = motionEndTime & 0xFF;
    
    // 마지막 에러 입력
    responsePayload[17] = f_error;
    responsePayload[18] = can_id;
    responsePayload[19] = can_sub_id;
    responsePayload[20] = motor_type;

    // error_code_str을 responsePayload에 복사 (최대 8바이트)
    for (int i = 0; i < 8 && i < strlen(error_code_str); i++) {
        responsePayload[21 + i] = static_cast<uint8_t>(error_code_str[i]);
    }

    // 응답 전송 (크기를 28바이트로 변경)
    sendData(senderId, receiverId_, CMD_STATUS_SYNC_ACK, responsePayload, 29);
}

void Custom_Protocol::handleMainPowerControl(uint16_t senderId, uint8_t* payload, size_t length)
{
    if (length >= 1) {
        setMainPower(payload[0]);
        
        // 응답 패킷 준비
        uint8_t response[1] = {mainPowerState_};
        
        // 응답 전송
        sendData(senderId, 0, CMD_MAIN_POWER_CONTROL_ACK, response, sizeof(response));
    }
}

void Custom_Protocol::setMainPower(uint8_t powerFlag)
{
    mainPowerState_ = (powerFlag != 0);
    // 실제 하드웨어 전원 제어 로직은 여기에 구현

    if (mainPowerState_) {
        HAL_GPIO_WritePin(LD8_GPIO_Port, LD8_Pin, GPIO_PIN_SET);
        
    } else {
        HAL_GPIO_WritePin(LD8_GPIO_Port, LD8_Pin, GPIO_PIN_RESET);
    }
}

void Custom_Protocol::handlePlayControl(uint16_t senderId, uint8_t* payload, size_t length)
{
    // 페이로드 길이 체크
    if (length < 1) return;
    // todo : 각 switch 내부 처리 추가, currentPlayState_ 에 상태 반환
    // 페이로드에서 PlayControl 상태 추출
    PlayControlState playState = static_cast<PlayControlState>(payload[0]);    
    // 상태에 따른 처리
    switch (playState) {
        case PlayControlState::PLAY_ONE:
            // 1회 재생
            break;
            
        case PlayControlState::PLAY_REPEAT:
            // 반복 재생
            break;
            
        case PlayControlState::PAUSE:
            // 일시정지
            break;            

        case PlayControlState::STOP:
            // 정지
            break;

        default:
            // 잘못된 상태값 수신
            return;
    }
    
    // 응답 전송
    uint8_t response[1];
    uint8_t currentPlayState_ = static_cast<uint8_t>(playState);

	motionPlayState_ = currentPlayState_;

    response[0] = static_cast<uint8_t>(currentPlayState_);    
    
    sendData(senderId, receiverId_, CMD_PLAY_CONTROL_ACK, response, 1);
}

