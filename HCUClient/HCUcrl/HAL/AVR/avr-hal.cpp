#include "avr-hal.h"
#ifdef __AVR__

#include "avr-hal.h"
#include <FastCRC.h>
#include <Arduino.h>

FastCRC16 CRC16;

namespace HAL
{

void init() {
    Serial.begin(1000000);
}
void loop() {
    
}
void print(const char *buffer){
    Serial.println(buffer);
}

uint16_t calculateCRC16(char *buffer, int length){
    return CRC16.x25((uint8_t*) buffer, length);    
}
bool TimerExpired = false;
} // namespace HAL

SIGNAL(TIMER0_COMPA_vect) 
{
    HAL::TimerExpired = true;
}
#endif