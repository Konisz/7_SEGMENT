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
#define ONE_WIRE_BUS 2  // D0 -> ONE WIRE BUS

//#ONEWIRE_DS_INIT
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds(&oneWire);

//#DS SENSORS ADRESS
uint8_t sensor0[8] = {0x28, 0x92, 0x4E, 0x45, 0x40, 0x21, 0x08, 0x1D};
uint8_t sensor1[8] = {0x28, 0x31, 0x87, 0x62, 0x14, 0x21, 0x01, 0x46};

//#DIGITS TABLE
int digits[] = {0xC0 ,0xF9 ,0xA4 ,0xB0 ,0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0xFF,0xBF, 0xAB};

//#DATA STORAGE
float last_temp1;
float last_temp2;

WiFiClient client;
String apiKeyValue = HTTP_POST_API_KEY;

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

void readTemperature(){

        ds.requestTemperatures();
        if(ds.getTempC(sensor0) > 10 && ds.getTempC(sensor0) < 40)
        last_temp1 = ds.getTempCByIndex(0);
        if(ds.getTempC(sensor1) > 10 && ds.getTempC(sensor1) < 40)
        last_temp2 = ds.getTempCByIndex(1);

}

void refreshTemperatureOnDisplay(){
            int d1 = ((int)last_temp1 / 10) % 10;
            int d2 = (int)last_temp1 % 10;
            int d3 = ((int)last_temp2 / 10) % 10;
            int d4 = (int)last_temp2 % 10;
            digitOutput(d1,d2,d3,d4,10,10);
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
    Serial.begin(9600);
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
    readTemperature();
    refreshTemperatureOnDisplay();
    sendDataToDatabase();
    delay(60000);
}

