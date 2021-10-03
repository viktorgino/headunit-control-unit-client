#pragma once

#include <stdint.h>

#include "../common.h"

class Protocol {
public:
    Protocol() {}
    virtual ~Protocol() {}
    virtual void receiveByte(char receivedByte) = 0;
    virtual void sendBusMessage(BusNumber /*bus*/, uint32_t /*id*/, uint8_t /*len*/, uint8_t */*buf*/) = 0;
    virtual void loop() = 0;
    virtual void sendClimateControlCommand(ClimateControlCommandFrame /*controlFrame*/) = 0;
    virtual void sendButtonInputCommand(Keys /*key*/) = 0;
    virtual void sendCustomCommand(CustomCommandFrame /*commandFrame*/) = 0;
    virtual void sendDebugMessageCommand(const char * message) = 0;
    virtual void setCallbacks(PlatformCallbacks *callbacks){
        m_callbacks = callbacks;
    }
protected:
    PlatformCallbacks *m_callbacks;
};
