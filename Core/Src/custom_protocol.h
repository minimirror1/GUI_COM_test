#ifndef CUSTOM_PROTOCOL_H_
#define CUSTOM_PROTOCOL_H_

#include "com_protocol_class.h"
#include "ISerialInterface.h"
#include "ITick.h"

class Custom_Protocol : public Com_Protocol {
public:
    // 생성자
    Custom_Protocol(ISerialInterface* serial, ITick* tick);
    virtual ~Custom_Protocol();

protected:
    // Com_Protocol의 가상 함수들 재정의
    virtual void handlePing(uint16_t senderId, uint8_t* payload, size_t length) override;
    virtual void handleStatusSync(uint16_t senderId, uint8_t* payload, size_t length) override;
    virtual void handleMainPowerControl(uint16_t senderId, uint8_t* payload, size_t length) override;
    virtual void setMainPower(uint8_t powerFlag) override;

private:
    // Custom_Protocol 전용 멤버 변수들
    uint32_t lastPingTime_;
    uint32_t totalRunTime_;
    uint16_t currentCount_;
    uint16_t totalCount_;
    uint16_t voltage_;
    uint16_t current_;
    bool mainPowerState_;
};

#endif /* CUSTOM_PROTOCOL_H_ */
