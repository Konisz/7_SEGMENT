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
uint8_t sensor1[8] = {0x28, 0xA0, 0xBD, 0xF8, 0x0C, 0x00, 0x00, 0x37};
uint8_t sensor2[8] = {0x28, 0x5A, 0x2E, 0x57, 0x04, 0x92, 0x3C, 0x2A};

//#DIGITS TABLE
int digits[] = {0xC0 ,0xF9 ,0xA4 ,0xB0 ,0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0xFF,0xBF, 0xAB};

//#DATA STORAGE
float last_temp0;
float last_temp1;
float last_temp2;

//#WIFI STATUS
bool wifiConnected = false;

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
        last_temp0 = ds.getTempC(sensor0);
        if(ds.getTempC(sensor1) > 10 && ds.getTempC(sensor1) < 40)
        last_temp1 = ds.getTempC(sensor1);
        if(ds.getTempC(sensor2) > 10 && ds.getTempC(sensor2) < 40)
        last_temp2 = ds.getTempC(sensor2);

}

void refreshTemperatureOnDisplay(){
            int d1 = ((int)last_temp0 / 10) % 10;
            int d2 = (int)last_temp0 % 10;
            int d3 = ((int)last_temp1 / 10) % 10;
            int d4 = (int)last_temp1 % 10;
            int d5 = ((int)last_temp2 / 10) % 10;
            int d6 = (int)last_temp2 % 10;
            digitOutput(d1,d2,d3,d4,d5,d6);
}

void sendDataToDatabase(){

        if(WiFi.status()== WL_CONNECTED){
            WiFiClient client;
            HTTPClient http;
            http.begin(client, ESP_DATA_PICK);
            http.addHeader("Content-Type", "application/x-www-form-urlencoded");
            String httpRequestData = "api_key=" + apiKeyValue + "&sensor0=" + last_temp0 + "&sensor1=" + last_temp1 + "&sensor2=" + last_temp2 + "";
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

uint8_t findDevices(int pin)
{
  OneWire ow(pin);

  uint8_t address[8];
  uint8_t count = 0;


  if (ow.search(address))
  {
    Serial.print("\nuint8_t pin");
    Serial.print(pin, DEC);
    Serial.println("[][8] = {");
    do {
      count++;
      Serial.println("  {");
      for (uint8_t i = 0; i < 8; i++)
      {
        Serial.print("0x");
        if (address[i] < 0x10) Serial.print("0");
        Serial.print(address[i], HEX);
        if (i < 7) Serial.print(", ");
      }
      Serial.println("  },");
    } while (ow.search(address));

    Serial.println("};");
    Serial.print("// nr devices found: ");
    Serial.println(count);
  }

  return count;
}

void setup() {
    Serial.begin(9600);
    ds.begin();
    //findDevices(ONE_WIRE_BUS);

    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    digitOutput(11,11,0,12,11,11);

    WiFi.begin(WIFI_SSID,WIFI_PASSW);

    unsigned long startTime = millis();
    unsigned long connectionTimeout = 15000;  //WI-FI TIMEOUT

    Serial.println("[I] Inicjalizacja modułu Wi-Fi");

    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(">");
      delay(500);

    if (millis() - startTime >= connectionTimeout)
      Serial.println(""); 
      Serial.println("[!] Błąd inicjalizacji modułu Wi-Fi");
      break;
    }

  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.print("[I] Połączono do sieci Wi-Fi. IP: ");
    Serial.println(WiFi.localIP());
  }
}


void loop(){
    if (!wifiConnected) {
    // Wykonaj ponowną próbę połączenia z siecią Wi-Fi
    WiFi.begin(WIFI_SSID,WIFI_PASSW);

    unsigned long startTime = millis();
    unsigned long connectionTimeout = 10000;  // Czas maksymalnego oczekiwania na połączenie (np. 10 sekund)

    while (WiFi.status() != WL_CONNECTED && millis() - startTime < connectionTimeout) {
      Serial.println("[!] Błąd inicjalizacji modułu Wi-Fi (loop)");
      readTemperature();
      refreshTemperatureOnDisplay();
      delay(5000);

      if (millis() - startTime >= 500) {
        WiFi.begin(WIFI_SSID,WIFI_PASSW);
        startTime = millis();
      }
    }

    if (WiFi.status() == WL_CONNECTED) {
      wifiConnected = true;
      Serial.print("[I] Połączono do sieci Wi-Fi. IP: ");
      Serial.println(WiFi.localIP());
    }
  }
    readTemperature();
    refreshTemperatureOnDisplay();
    sendDataToDatabase();
    delay(30000);
    Serial.println(ds.getTempC(sensor0));
    Serial.println(ds.getTempC(sensor1));
    Serial.println(ds.getTempC(sensor2));
    Serial.println("@@@@@@@@@@@@@@");
}

