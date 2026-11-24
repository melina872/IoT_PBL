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
#define RFID_SDA 5
#define RFID_RST 22

MFRC522 rfid(RFID_SDA, RFID_RST); // instance of MFRC522

//--- BME280 Pins ---


//--- SD Card Reader Pins ---


//-- Global Variables --
//--- Common ---
//--- LED ---
unsigned long indicatorStart = 0; 
unsigned long indicatorTime = 0; 
bool indicatorOn = false; 

String lastUID = ""; 
unsigned long lastScanTime = 0;

//--- Pushbutton ---
//--- Potentiometer ---
//--- OLED ---
//--- MFRC255 ---
String authorizedUIDs[] = {"53 56 29 56"}; // UID (white card) authorized 
int numAuthorized = 1; 

//--- BME280 ---
//--- SD Card Reader ---
//--- Web Page ---
//---- WiFi Information ----
const char* ssid = "Library_Wifi"; // WiFi SSID based on [*ref*], Lab 7 handout, Part
const char* password = "Password123"; // WiFi password based on [*ref*], Lab 7 handout, Part

//---- Web Server Variable ----
WebServer server(80); // Create a web server object that listens for HTTP request on port 80

//---- Web Page HTML Code ----
const char webpage [] PROGMEM = R"rawliteral(
    <!DOCTYPE html>
    <!-- This document will be merged with the main.cpp file -->

    <header>
        <title> Library Web Interface </title>
    </header>

    <body style="background-color: rgb(207, 238, 240);">
        <center>
            <h1 style="color: rgb(19, 4, 65); font-family:Verdana, Geneva, Tahoma, sans-serif;"> Staff Data Page </h1>
        </center>
        <p class="value">
            ROOM_STATUS
        </p>
        <p class="value">
            ROOM_TEMP
        </p>
        <p class="value">
            FAN_LEVEL
        </p>
        <p class="value">
            STATUS_BYTE
        </p>
    </body>
)rawliteral";

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

// decides if the card is authorized or not
bool isAuthorized(String uid) 
{ 
  for (int i = 0; i < numAuthorized; i++) 
  { 
    if (uid == authorizedUIDs[i]) 
    { 
      return true; 
    } 
  } 
  return false; 
} 

// event handlers for access granted and denied
void startIndicatorTimer(unsigned long duration) 
{ 
  indicatorStart = millis(); 
  indicatorTime = duration; 
  indicatorOn = true; 
} 

void allIndicatorsOff() 
{ 
  digitalWrite(Green_LED, LOW); 
  digitalWrite(Red_LED, LOW); 
}  

void onAccessGranted() 
{ 
  Serial.println("ACCESS GRANTED"); 
  allIndicatorsOff(); 
  digitalWrite(Green_LED, HIGH); 
  startIndicatorTimer(2000); 
} 

void onAccessDenied() 
{ 
  Serial.println("ACCESS DENIED"); 
  allIndicatorsOff(); 
  digitalWrite(Red_LED, HIGH);  
  startIndicatorTimer(1000); 
} 

void onTimerExpired() 
{ 
  allIndicatorsOff(); 
  indicatorOn = false; 
  Serial.println("Ready"); 
} 

void onCardScanned(String uid) 
{ 
  if (uid == lastUID && millis() - lastScanTime < 2000) 
  { 
    Serial.println("Repeat ignored"); 
    return; 
  } 
  lastUID = uid; 
  lastScanTime = millis(); 

  Serial.print("Card UID: "); 
  Serial.println(uid); 

  if (isAuthorized(uid) == true) 
  { 
    onAccessGranted(); 
  } 
  else 
  { 
    onAccessDenied(); 
  } 
} 

void setup(){

    Serial.begin(115200);

    //RFID code
    SPI.begin();
    rfid.PCD_Init();
    delay(10);
    rfid.PCD_DumpVersionToSerial();
    Serial.println("Tap a card...");

    //LED code
    pinMode(Green_LED, OUTPUT); 
    pinMode(Red_LED, OUTPUT);
}

void loop(){

    //Card Dectection
    if (rfid.PICC_IsNewCardPresent() == true && rfid.PICC_ReadCardSerial() == true) 
    { 
    String uid = getUID(); 
    onCardScanned(uid); 
    rfid.PICC_HaltA(); 
    rfid.PCD_StopCrypto1(); 
    } 

    // Indicator Timer
    if (indicatorOn == true && millis() - indicatorStart >= indicatorTime) 
    { 
    onTimerExpired(); 
    } 
}