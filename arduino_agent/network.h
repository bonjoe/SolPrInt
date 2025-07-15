/*
  Copyright (c)   Code related to ntp client server
                  Rui Santos, https://RandomNerdTutorials.com/esp32-date-time-ntp-client-server-arduino/
                  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
                  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 
  For more informations about victron vrm api https://vrm-api-docs.victronenergy.com/
  For more informations about thingspeak      https://github.com/mathworks/thingspeak-arduino
*/

#ifndef NETWORK
#define NETWORK

#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_bt.h>
#include "driver/adc.h"
#include "ThingSpeak.h"
#include "time.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define SECRET_SSID "MySSID"    // replace MySSID with your WiFi network name
#define SECRET_PASS "MyPassword"  // replace MyPassword with your WiFi password

#define SECRET_CH_ID 0000000     // replace 0000000 with your thingpeak channel number
#define SECRET_WRITE_APIKEY "XYZ"   // replace XYZ with yourthingpeak channel write API Key
#define SECRET_READ_APIKEY "XYZ"   // replace XYZ with your thingpeak channel write API Key

#define SECRET_API_URL "https://vrmapi.victronenergy.com/v2/installations/{idSite}/stats?" // replace {idSite} with your vrm site ID
#define SECRET_BEARER_TOKEN  "Bearer <bearer_token_value>" // replace <bearer_token_value> with your vrm bearer token

void connectToNetwork();
void sendDataThingSpeak(double preAct); 
double getLastValidValue( int field); 
void getDataThingSpeak(double& timeSimu, double& hour, double& month, double& PvP, double& irrWeek, double& SOC);
void getLastSpeed(double& lastAction);
void initializeThingSpeak();
void print_wakeup_reason();
void goToDeepSleep();
void getVRMData(double& OP1);
void setupTime();
unsigned long getCurrentTimestamp(); 

#endif
