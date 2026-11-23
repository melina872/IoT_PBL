//-- Libraries included --
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebServer.h>
#include <MFRC522.h>

//-- Pin Definitions --
//--- Common Pins ---
//--- LED Pins---
#define Green_LED 25
#define Red_LED 26

//--- Pushbutton Pin (Based on [*ref*], Lab 11 handout, Part III)---
#define Button_PIN 35 //pushbutton connected to GND

//--- Potentiometer Pin (Based on [*ref*], Lab 11 handout, Part III)---
#define Pot_PIN 34 //Potentiometer wiper 

//--- OLED Pins---


//--- MFRC255 Pins --- Based on [*ref*], Lab 9 handout, base code
#define RFID_SDA 16
#define RFID_RST 17

MFRC522 rfid(RFID_SDA, RFID_RST); // instance of MFRC522

//--- BME280 Pins ---


//--- SD Card Reader Pins ---


//-- Global Variables --
//--- Common ---
//--- LED ---
//--- Pushbutton ---
//--- Potentiometer ---
//--- OLED ---
//--- MFRC255 ---
//--- BME280 ---
//--- SD Card Reader ---

//-- Functions --

// getsUID() turns the card's UID into text
String getUID()
{
    String uid = "";
    for (byte i = 0; i < rfid.uid.size; i++)
    {
        if (rfid.uid.uidByte[i] < 0x10)
        {
            uid = uid + "0";
        }
        uid = uid + String(rfid.uid.uidByte[i], HEX);
        if (i < rfid.uid.size - 1)
        {
            uid = uid + " ";
        }
    }
    uid.toUpperCase();
    return uid;
}

void setup(){

    Serial.begin(115200);

    //RFID code
    SPI.begin();
    rfid.PCD_Init();
    delay(10);
    rfid.PCD_DumpVersionToSerial();
    Serial.println("Tap a card...");
}

void loop(){
    //Card Dectection
    if (rfid.PICC_IsNewCardPresent() == true && rfid.PICC_ReadCardSerial() == true)
    {
        String uid = getUID();
        Serial.print("Card UID: ");
        Serial.println(uid);

        rfid.PICC_HaltA();
    }
}