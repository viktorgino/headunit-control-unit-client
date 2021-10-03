#include "ProtocolManager.h"

ProtocolManager::ProtocolManager() { protocols[Protocol_HUDSerial] = (Protocol *)new HUDSerial(); }

void ProtocolManager::setProtocol(Protocols protocol) { currentProtocol = protocol; }

void ProtocolManager::receiveByte(char incomingByte) {
    protocols[currentProtocol]->receiveByte(incomingByte);
}

void ProtocolManager::loop() { protocols[currentProtocol]->loop(); }

void ProtocolManager::sendClimateControlCommand(ClimateControlCommandFrame controlFrame) {
    protocols[currentProtocol]->sendClimateControlCommand(controlFrame);
}

void ProtocolManager::sendCustomCommand(CustomCommandFrame commandFrame) {
    protocols[currentProtocol]->sendCustomCommand(commandFrame);
}
void ProtocolManager::sendButtonInputCommand(Keys key) {
    protocols[currentProtocol]->sendButtonInputCommand(key);
}

void ProtocolManager::sendDebugMessageCommand(const char * message) {
    protocols[currentProtocol]->sendDebugMessageCommand(message);
}
void ProtocolManager::setCallbacks(PlatformCallbacks *callbacks) {
    protocols[currentProtocol]->setCallbacks(callbacks);
}