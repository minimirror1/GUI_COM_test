#include "main.h"
#include "custom_protocol.h"

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
    // 상태 정보 패킷 준비
    uint8_t status[10];
    uint32_t currentTime = tick_->getTickCount();
    
    // 상태 정보 패킷 구성
    memcpy(&status[0], &currentTime, sizeof(currentTime));     // 현재 시간
    memcpy(&status[4], &currentCount_, sizeof(currentCount_)); // 현재 카운트
    memcpy(&status[6], &voltage_, sizeof(voltage_));          // 전압
    memcpy(&status[8], &current_, sizeof(current_));          // 전류
    
    // 상태 정보 전송
    sendData(senderId, 0, CMD_STATUS_SYNC_ACK, status, sizeof(status));
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
