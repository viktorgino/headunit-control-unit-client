#pragma once

#include "../common.h"
#include "HUDSerial/HUDSerial.h"


typedef enum {
    Protocol_HUDSerial = 0,
    Protocol_GVRET,
    Protocol_None
} Protocols;

class ProtocolManager {
public:
    ProtocolManager();
    void setProtocol(Protocols protocol);
    void loop();
    void receiveByte(char incomingByte);
    void sendClimateControlCommand(ClimateControlCommandFrame controlFrame);
    void sendButtonInputCommand(Keys key);
    void sendCustomCommand(CustomCommandFrame commandFrame);
    void sendDebugMessageCommand(const char * message);
    void setCallbacks(PlatformCallbacks *callbacks);
private:
    Protocols currentProtocol;
    Protocol *protocols[Protocol_None];
};

extern ProtocolManager protocolManager;