/*Hardware: ESP32, step Motor 28BYJ-48, Joy-it 1.3" ST7789 display
***************************************************************************
Copyright (c)   Written by Joevin Bonzi, University of Hohenheim, 440e
                bonzi.wiomoujoevin@uni-hohenheim.de
***************************************************************************/

#include <esp_wifi.h>
#include <esp_bt.h>
#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include "network.h"
#include "rlagentFunction.h"
#include "motor.h"
#include "screen.h"
#include <Adafruit_GFX.h>
  
// Timer variables
unsigned long lastDataUpdateTime = 0;
unsigned long dataUpdateDelay = 5 * 60000;  // 5 minutes

const int NUM_TEST_CASES = 6;
double testInputs[NUM_TEST_CASES][7] = {
  {7,   3,  211.3,      0,   60, 267.17, 20},  // For test sunny
  {7,  10,      0,      0, 50.0, 217.79, 0},  // For test cloudy
};
int selectedTestCase = 1;  // Change this index to switch test cases
double input[7]; // {hour,month,PP,PvP,SOC,Irrweek,prevAct}
double backedInput[7];
int chosen_action=0;
double previousPosition;

double output[10]= {0, 20, 26.25, 32.5, 38.75, 45, 51.25, 57.5, 63.75, 70};
double prevAct = testInputs[selectedTestCase][6];  // Default starting action
double lastHourlyAction = testInputs[selectedTestCase][6];  // Tracks this hour's action
double prevActHour = -1;   // To track when we last updated it
double prevANNHour= -1;
bool actionTakenInWindow = false;
double timeSimu= 0;
void setup() 
{
  esp_bt_controller_disable();
  Serial.begin(115200);
  delay(100);
  Serial.println("Get initial data");

  for (int i = 0; i < 7; i++) {
    input[i] = testInputs[selectedTestCase][i];
    backedInput[i] = testInputs[selectedTestCase][i];
  }
  screen_init(); 
  initializeThingSpeak();
  connectToNetwork();
  getVRMData(input[2]); // VRM  PP
  if (isnan(input[2])) {input[2] = backedInput[2];}
  delay(1000);
  //get tgspk       hour,     month,    PvP,      irrWeek   SOC
  getDataThingSpeak(timeSimu, input[0], input[1], input[3], input[5], input[4]);
  if (input[1] == 0) { for (int i = 0; i < 7; i++) {input[i] = backedInput[i];}}
  WiFi.disconnect(true); WiFi.mode(WIFI_OFF);
  previousPosition=input[6];
  initializeMotor(previousPosition); 
  input[4] =  backedInput[4];
  actionScreen();
}

void loop()
{
  unsigned long currentMillis = millis();
  if ((currentMillis - lastDataUpdateTime) > dataUpdateDelay || lastDataUpdateTime == 0) {
    actionForWifi();
    if ( lastDataUpdateTime == 0) {Serial.print("First action loop");virtualPower();}
    lastDataUpdateTime = currentMillis;
    int testHour = (int)floor(input[0]);

    if (testHour != (int)prevANNHour) {
      actionForANN();
      Serial.print("h: "); Serial.print(input[0]); Serial.print("  M: "); Serial.print(input[1]); Serial.print("  PP: "); Serial.print(input[2]);  
      Serial.print("  PvP: "); Serial.print(input[3]); Serial.print("  SOC: "); Serial.print(input[4]); Serial.print("  IrrW: "); Serial.print(input[5]);  Serial.print("  prevAct: "); Serial.println(input[6]); 
      Serial.print("Output speed: "); Serial.print(output[chosen_action]);Serial.print("     ");
      actionForMotor();
      Serial.println(); Serial.println();
      delay(1000);
      prevANNHour = testHour;
    }
    actionScreen();
  }
}

void actionForWifi()
{
  connectToNetwork(); 
  for (int i = 0; i < 7; i++) {backedInput[i] = input[i];}
  getVRMData(input[2]);
  if (isnan(input[2])) {input[2] = backedInput[2];}
  delay(1000);
  //get ThingSpeak  hour,     month,    PvP,      irrWeek   SOC
  getDataThingSpeak(timeSimu, input[0], input[1], input[3], input[5], input[4]);
  if (input[1] == 0) {for (int i = 0; i < 7; i++) {input[i] = backedInput[i];}}

  delay(15000); 
  sendDataThingSpeak(output[chosen_action]); 
  Serial.println();
  WiFi.disconnect(true);WiFi.mode(WIFI_OFF); 
}

void actionForANN(){               
  double currentHour = floor(input[0]);
  if (currentHour != prevActHour) {
      prevAct = lastHourlyAction;
      prevActHour = currentHour;
  }
  input[6] = prevAct;  // Feed the delayed prevAct to the ANN
  chooseGreedyAction(input, 7, chosen_action);
  lastHourlyAction = output[chosen_action];
}

void actionForMotor(){
  setMotortoPosition(output[chosen_action],previousPosition); 
  previousPosition=output[chosen_action];
}

void actionScreen() {
  Serial.println("Print in screen");
  //screen_displayDashboard(hourOfYear,  power,  pvPower,  soc,  irrWeek,    prevAction,    currSpeed)
  screen_displayDashboard(timeSimu, input[2], input[3], input[4], input[5], input[6], output[chosen_action]);
  delay(1000);
}

//only for initial power or for testing without oil press
void virtualPower(){
  if (output[chosen_action] == 0) {input[2]= 0;} 
  else {input[2]= (57.3+output[chosen_action]*8.7-output[chosen_action]*output[chosen_action]*0.05)*(1+random(-5,6)/100);}
}
