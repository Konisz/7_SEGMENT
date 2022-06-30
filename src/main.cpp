#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//#PORTS
#define latchPin 12     // D6 -> STORAGE CLOCK INPUT
#define dataPin 14      // D5 -> SERIAL INPUT
#define clockPin 13     // D7 -> SR CLOCK INPUT
#define ONE_WIRE_BUS 2 // D0 -> ONE WIRE BUS

//#ONEWIRE_DS_INIT
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds(&oneWire);

//#DIGITS TABLE
int digits[] = {0xC0 ,0xF9 ,0xA4 ,0xB0 ,0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0xFF};

//#DATA STORAGE
int last_temp1;
int last_temp2;

//#DELAYS

//#FUNC SENDING DATA TO SHIFT REGISTERS
void digitOutput(int d1,int d2, int d3, int d4, int d5, int d6){
    digitalWrite(latchPin,LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, digits[d5]);
    shiftOut(dataPin, clockPin, MSBFIRST, digits[d6]);
    shiftOut(dataPin, clockPin, MSBFIRST, digits[d3]);
    shiftOut(dataPin, clockPin, MSBFIRST, digits[d4]);
    shiftOut(dataPin, clockPin, MSBFIRST, digits[d1]);
    shiftOut(dataPin, clockPin, MSBFIRST, digits[d2]);
    digitalWrite(latchPin,HIGH);
}

void getTemperatureDEBUG(){
    Serial.print("Requesting temperatures...");
    ds.requestTemperatures();
    Serial.println("DONE");
    Serial.print("Reading from sensor #0: ");
    Serial.println(ds.getTempCByIndex(0));
    Serial.print("Reading from sensor #1: ");
    Serial.println(ds.getTempCByIndex(1));
}

void getTemperature(){
    ds.requestTemperatures();
    last_temp1 = ds.getTempCByIndex(0);
    last_temp2 = ds.getTempCByIndex(1);
}

void setup() {
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);

    digitOutput(1,2,10,10,3,2);

    Serial.begin(9600);
    ds.begin();

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