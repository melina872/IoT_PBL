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
#define OLED_CS 15
#define OLED_DC 16
#define OLED_RST 17

//--- MFRC255 Pins ---


//--- BME280 Pins ---
#define BME280_SDA 21

//--- SD Card Reader Pins ---
#define SD_CS 2

//-- Global Variables --
//--- Common ---
//--- LED ---
//--- Pushbutton ---
//---- Interupt ----
// All variables based on [], Lab 10 handout, Part IV
volatile bool buttonFlag = false; //When button pressed, the flag is raised
volatile unsigned long lastInterrupt = 0; //last accepted press
int pressCountButton = 0; //Counts the number of times the button has been pressed

//--- Potentiometer ---
//--- OLED ---
//--- MFRC255 ---
//--- BME280 ---
//--- SD Card Reader ---

//-- Functions --
//--- Help Button ---
//---- Count ---- (Based on [], Lab 10 handout, Part IV)
void onButtonPressed(){
    pressCountButton++; //increases the press count
    Serial.print("Button Pressed. Count ="); //Displays on the serial monitor the amount of times it counts
    Serial.print(pressCountButton);
}

//---- ISR ---- (Based on [], Lab 10 handout, Part IV)
void IRAM_ATTR button_ISR(){
    if (millis() - lastInterrupt > 200){
        lastInterrupt = millis(); //Remembers the last time the button was pressed
        buttonFlag = true; //raises the flag for loop()
    }
}

void HelpButton(){

}

void setup(){
  //Button set-up
  pinMode(Button_PIN, INPUT_PULLUP);

  //Interrupt set-up (Based on [], Lab 10 handout, Part IV)
  attachInterrupt(digitalPinToInterrupt(Button_PIN), button_ISR, FALLING); //Set the mode to look at the falling edge
}

void loop(){

}