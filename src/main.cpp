#include <Arduino.h>

//#PORTS
#define latchPin 12     // D6 -> STORAGE CLOCK INPUT
#define dataPin 14      // D5 -> SERIAL INPUT
#define clockPin 13     // D7 -> SR CLOCK INPUT

int digits[] = {0xC0 ,0xF9 ,0xA4 ,0xB0 ,0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

void SET_ADDR(int num) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, num);
    digitalWrite(latchPin, HIGH);
}

void setup() {
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
}

void loop(){

    for(int i = 0; i <= 9; i++ )
    {
        SET_ADDR(digits[i]);
        delay(300);
    }
}