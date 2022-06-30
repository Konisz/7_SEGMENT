#include <Arduino.h>

//#PORTS
#define latchPin 12     // D6 -> STORAGE CLOCK INPUT
#define dataPin 14      // D5 -> SERIAL INPUT
#define clockPin 13     // D7 -> SR CLOCK INPUT

int digits[] = {0xC0 ,0xF9 ,0xA4 ,0xB0 ,0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0xFF};

void SET_ADDR(int num) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, num);
    digitalWrite(latchPin, HIGH);
}

void digitOutput(int d1,int d2, int d3, int d4, int d5, int d6){
    digitalWrite(latchPin,LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, digits[d6]);
    shiftOut(dataPin, clockPin, MSBFIRST, digits[d5]);
    shiftOut(dataPin, clockPin, MSBFIRST, digits[d4]);
    shiftOut(dataPin, clockPin, MSBFIRST, digits[d3]);
    shiftOut(dataPin, clockPin, MSBFIRST, digits[d2]);
    shiftOut(dataPin, clockPin, MSBFIRST, digits[d1]);
    digitalWrite(latchPin,HIGH);
}

void setup() {
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);

    digitOutput(1,2,10,10,3,2);
}

void loop(){
/*
    for(int i = 0; i <= 10; i++ )
    {
        SET_ADDR(digits[i]);
        delay(300);
    }
    */
}