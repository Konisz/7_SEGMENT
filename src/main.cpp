#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
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
int digits[] = {0xC0 ,0xF9 ,0xA4 ,0xB0 ,0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0xFF,0xBF, 0xAB};

//#DATA STORAGE
int last_temp1;
int last_temp2;
int chck_temp1;
int chck_temp2;

//#DELAYS
//unsigned millisTimer;
//unsigned refresh_time_h;
//unsigned refresh_time = 6000UL;

WiFiClient client;
String apiKeyValue = "tPmAT5Ab3j7F9";

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
}

void readTemperature(){

        ds.requestTemperatures();
        if(ds.getTempCByIndex(0) > 10 && ds.getTempCByIndex(0) < 40)
        last_temp1 = ds.getTempCByIndex(0);
        if(ds.getTempCByIndex(1) > 10 && ds.getTempCByIndex(1) < 40)
        last_temp2 = ds.getTempCByIndex(1);

}

void refreshTemperatureOnDisplay(){

        if((chck_temp1 != last_temp1) || (chck_temp2 != last_temp2)){
            int d1 = (last_temp1 / 10) % 10;
            int d2 = last_temp1 % 10;
            int d3 = (last_temp2 / 10) % 10;
            int d4 = last_temp2 % 10;
            digitOutput(d1,d2,d3,d4,10,10); //Digit 5 & 6 is disabled
            chck_temp1 = last_temp1;
            chck_temp2 = last_temp2;
        }

}

void sendDataToDatabase(){

        if(WiFi.status()== WL_CONNECTED){
            WiFiClient client;
            HTTPClient http;
            http.begin(client, ESP_DATA_PICK);
            http.addHeader("Content-Type", "application/x-www-form-urlencoded");
            String httpRequestData = "api_key=" + apiKeyValue + "&sensor0=" + last_temp1 + "&sensor1=" + last_temp2 + "";
            Serial.print("httpRequestData: ");
            Serial.println(httpRequestData);
            int httpResponseCode = http.POST(httpRequestData);
            if (httpResponseCode>0) {
                 Serial.print("HTTP Response code: ");
                Serial.println(httpResponseCode);
            }
            else {
                Serial.print("Error code: ");
                Serial.println(httpResponseCode);
            }
        http.end();
    }
    else {
    Serial.println("WiFi Disconnected");
    }

}

void setup() {
    //Serial.begin(115200);
    ds.begin();
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    digitOutput(11,11,0,12,11,11);

    WiFi.begin(WIFI_SSID,WIFI_PASSW);
    while(WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println(WiFi.localIP());
     
}

void loop(){
    //millisTimer = millis();
    //if(millisTimer - refresh_time_h >= refresh_time){
    readTemperature();
    refreshTemperatureOnDisplay();
    sendDataToDatabase();
    delay(10000);
    //}
    //refresh_time_h = millisTimer;
}

