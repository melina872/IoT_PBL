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
#define MOSI_PIN 23
#define MISO_PIN 19
#define CLK_PIN 18

//--- LED Pins---
#define Green_LED 25
#define Red_LED 26

//--- Pushbutton Pin (Based on [*ref*], Lab 11 handout, Part III)---
#define Button_PIN 35 //pushbutton connected to GND

//--- Potentiometer Pin (Based on [*ref*], Lab 11 handout, Part III)---
#define Pot_PIN 34 //Potentiometer wiper 

//--- OLED Pins---
#define OLED_CS 15
#define OLED_DC 16
#define OLED_RST 17

//--- MFRC255 Pins --- Based on [*ref*], Lab 9 handout, base code
#define RFID_SDA 5
#define RFID_RST 22

MFRC522 rfid(RFID_SDA, RFID_RST); // instance of MFRC522

//--- BME280 Pins ---
#define BME280_SDA 21

//--- SD Card Reader Pins ---
#define SD_CS 2

//-- Global Variables --
//--- Common ---
//--- LED --- Based on [*ref*], Lab 9 handout, base code
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

//--- BME280 --- Based on [*ref*], Lab 7 handout, base code
Adafruit_BMP280 bmp;
float temperature;

//--- SD Card Reader ---

//-- Functions --

// getsUID() turns the card's UID into text | Based on [*ref*], Lab 9 handout, base code
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

// decides if the card is authorized or not | Based on [*ref*], Lab 9 handout, base code
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

// event handlers for access granted and denied | Based on [*ref*], Lab 9 handout, base code
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
  Serial.println("OCCUPIED"); 
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

    //RFID code | Based on [*ref*], Lab 9 handout, base code
    SPI.begin();
    rfid.PCD_Init();
    delay(10);
    rfid.PCD_DumpVersionToSerial();
    Serial.println("AVAILABLE");

    //LED code
    pinMode(Green_LED, OUTPUT); 
    pinMode(Red_LED, OUTPUT);

    //BMP280 code (initialization) | Based on [*ref*], Lab 7 handout, base code
    Wire.begin();
 
    if (bmp.begin(0x76))
    {
     Serial.println("BMP280 Found at 0x76");
    }
      else if (bmp.begin(0x77))
    {
      Serial.println("BMP280 Found at 0x77");
    }
    else
    {
      Serial.println("BMP280 Not Found");
    }

}

void loop(){

    //Card Dectection | Based on [*ref*], Lab 9 handout, base code
    if (rfid.PICC_IsNewCardPresent() == true && rfid.PICC_ReadCardSerial() == true) 
    { 
    String uid = getUID(); 
    onCardScanned(uid); 
    rfid.PICC_HaltA(); 
    rfid.PCD_StopCrypto1(); 
    } 

    // Indicator Timer | Based on [*ref*], Lab 9 handout, base code
    if (indicatorOn == true && millis() - indicatorStart >= indicatorTime) 
    { 
    onTimerExpired(); 
    }
    
    //read temp lab 6
    temperature = bmp.readTemperature();
    
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");
    
    delay(1000);

}