#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "credentials.h"

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
int chck_temp1;
int chck_temp2;

//#DELAYS
unsigned  millisTimer;
unsigned rd_temp;
unsigned rd_screen;
/*
WiFiClient client;
int status = WL_IDLE_STATUS;
*/
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

void readTemperatureDEBUG(){
    if(millisTimer - rd_temp >= 10000UL){
        Serial.print("# Requesting temperatures...");
        ds.requestTemperatures();
        Serial.println("DONE");
        Serial.print("# Reading from sensor #0: ");
        Serial.println(ds.getTempCByIndex(0));
        last_temp1 = ds.getTempCByIndex(0);
        Serial.print("# Reading from sensor #1: ");
        Serial.println(ds.getTempCByIndex(1));
        last_temp2 = ds.getTempCByIndex(1);
        Serial.println("#==================");
        Serial.print("# Reading from sensor #0 (variable): ");
        Serial.println(last_temp1);
        Serial.print("# Reading from sensor #1 (variable): ");
        Serial.println(last_temp2);
        Serial.println("###################");
        rd_temp = millisTimer;
    }
}

void readTemperature(){
    if(millisTimer - rd_temp >= 10000UL){
        ds.requestTemperatures();
        if(ds.getTempCByIndex(0) > 10 && ds.getTempCByIndex(0) < 40)
        last_temp1 = ds.getTempCByIndex(0);
        if(ds.getTempCByIndex(1) > 10 && ds.getTempCByIndex(1) < 40)
        last_temp2 = ds.getTempCByIndex(1);
        rd_temp = millisTimer;
    }
}

void refreshTemperatureOnDisplay(){
    if(millisTimer - rd_screen >= 10000UL)
    {
        if((chck_temp1 != last_temp1) || (chck_temp2 != last_temp2)){
            int d1 = (last_temp1 / 10) % 10;
            int d2 = last_temp1 % 10;
            int d3 = (last_temp2 / 10) % 10;
            int d4 = last_temp2 % 10;
            digitOutput(d1,d2,d3,d4,10,10); //Digit 5 & 6 is disabled
            chck_temp1 = last_temp1;
            chck_temp2 = last_temp2;
        }
        rd_screen = millisTimer;
    }
}

void setup() {
    ds.begin();
    Serial.begin(9600);
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    /*
    while(status != WL_CONNECTED)
    {
        status = WiFi.begin(WIFI_SSID,WIFI_PASSW);
        delay(5000);
    }
    Serial.println(WiFi.localIP());
     */
}

void loop(){
    millisTimer = millis();
    readTemperature();
<<<<<<< HEAD
    refreshTemperatureOnDisplay();TERT
=======
    refreshTemperatureOnDisplay();
}
>>>>>>> 14f27a1baae9d0836ee1e1d795810288b89c2200
