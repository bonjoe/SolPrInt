#include "network.h"

const char* KNOWN_SSID[] = {SECRET_SSID};
const char* KNOWN_PASSWORD[] = {SECRET_PASS};
const int   KNOWN_SSID_COUNT = sizeof(KNOWN_SSID) / sizeof(KNOWN_SSID[0]); // number of known networks

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
const char * myReadAPIKey = SECRET_READ_APIKEY;
WiFiClient  client;
unsigned long WIFI_TIMEOUT = 10000;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

// VRM API details 
const char* apiUrl = SECRET_API_URL;
const char* bearerToken = SECRET_BEARER_TOKEN; 

void connectToNetwork() {
  boolean wifiFound = false;
  int i, n;
  WiFi.mode(WIFI_STA);WiFi.disconnect();
  delay(100);
  int nbVisibleNetworks = WiFi.scanNetworks();
  if (nbVisibleNetworks == 0) {
    Serial.println(F("no networks found. Reset to try again"));
  }
  // check if we recognize
  for (i = 0; i < nbVisibleNetworks; ++i) {
    for (n = 0; n < KNOWN_SSID_COUNT; n++) { // walk through the list of known SSID and check for a match
      if (strcmp(KNOWN_SSID[n], WiFi.SSID(i).c_str())) {
      } else { // we got a match
        wifiFound = true;
        break; // n is the network index we found
      }
    } // end for each known wifi SSID
    if (wifiFound) break; // break from the "for each visible network" loop
  } // end for each visible network

  if (!wifiFound) {
    Serial.println(F("no Known network identified. Reset to try again"));
  }

  if (wifiFound) {
  // 1 known SSID
  WiFi.begin(KNOWN_SSID[n], KNOWN_PASSWORD[n]);
  // Keep track of when we started our attempt to get a WiFi connection
  unsigned long startAttemptTime = millis();
  // Keep looping while we're not connected AND haven't reached the timeout
  while (WiFi.status() != WL_CONNECTED && 
         millis() - startAttemptTime < WIFI_TIMEOUT){
         delay(10);
  }
  // Make sure that we're actually connected, otherwise go to deep sleep
  if(WiFi.status() != WL_CONNECTED){Serial.println("FAILED");}
  Serial.print(F("WiFi connected to  ")); Serial.println(KNOWN_SSID[n]);
  }
}

void initializeThingSpeak() { 
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  delay(1000);
}

void sendDataThingSpeak(double act) {
  const int maxRetries = 5;
  int attempt = 0;
  int x = -1;
 Serial.print("ThingSpeak update ");
  while (attempt < maxRetries) {
    ThingSpeak.setField(6, (float)act);
    x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (x == 200) {
      Serial.print("successful:  Action: "); Serial.println(act);
      break;
    } else {
      Serial.print(" error "); Serial.print(x);Serial.print("  ...");
      attempt++;
      delay(15000);  // wait before retrying
    }
  }
  if (x != 200) {Serial.println("Failed to update ThingSpeak after multiple attempts.");}
}

// Function to retrieve the last valid (non-NaN) data from ThingSpeak
double getLastValidValue( int field) {
    double value = NAN;
    int maxAttempts = 5;
    int attempts = 0;
    while (attempts < maxAttempts) {
        double fieldValue = ThingSpeak.readFloatField(myChannelNumber, field, myReadAPIKey);
        value = ThingSpeak.getLastReadStatus() == 200 ? fieldValue : NAN;
        if (!isnan(value)) {return value;}
        Serial.print("retrying field "); Serial.print(field); Serial.print("... ");
        delay(15000);
        attempts++;
    }
    Serial.println(".");
    return NAN;
}

// Function to get data from ThingSpeak and retrieve last valid values for multiple fields
void getDataThingSpeak(double& timeSimu, double& hour, double& month, double& PvP, double& irrWeek, double& SOC) {
    timeSimu= getLastValidValue(1);   // Field 1 for 'hour'
    Serial.print("ThingSpeak Data :");Serial.print("  HourSimu: "); Serial.print(timeSimu,2);
    hour = fmod(timeSimu, 24); // Remainder when divided by 24 gives the hour of the day (0-23)

    int dayOfYear = timeSimu / 24; // Integer division to get day of the year
    int daysInMonths[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int cumulativeDays = 0;
    month = 1; 
    for (int i = 0; i < 12; i++) {
        if (dayOfYear < cumulativeDays + daysInMonths[i]) {break;}
        cumulativeDays += daysInMonths[i];
        month++;
    }
    
    PvP = getLastValidValue(3);     // Field 3 for 'PvP'
    irrWeek = getLastValidValue(5); // Field 5 for 'irrWeek'
	  SOC = getLastValidValue(4); // Field 4 for 'SOC'

    if (isnan(hour) || isnan(month) || isnan(PvP) || isnan(irrWeek) || isnan(SOC)) {
        Serial.println("Warning: Some data fields may not be available.");
        month= 0;
    } else {
        Serial.print("   Hour: "); Serial.print(hour); Serial.print("   Month: "); Serial.println(month);
        Serial.print("                   PvP: ");  Serial.print(PvP);  Serial.print("   Irradiance for Week: "); Serial.print(irrWeek); Serial.print("   SOC: "); Serial.println(SOC);
    }
}

// Function to get last action data from ThingSpeak
void getLastSpeed(double& lastAction) {
    lastAction = getLastValidValue(6);     // Field 6 for last action
    if (isnan(lastAction)) {Serial.println("Warning: last action not available.");} 
    else {Serial.print("Last action from Thingspeak : ");Serial.print(lastAction); Serial.println(" rpm");}
}

void getVRMData(double& OP1_avg) {
  setupTime();
  if ((WiFi.status() == WL_CONNECTED)) {
    unsigned long currentTimestamp = 0;
    while (currentTimestamp < 1000000000) {
      currentTimestamp = getCurrentTimestamp();
      if (currentTimestamp < 1000000000) {Serial.print(" ..."); delay(1000);}
    }
    // Request data with 15-minute averaging
    String requestUrl = String(apiUrl) +"start=" + String(currentTimestamp - 1200) +  // 20 minutes ago
      "&end=" + String(currentTimestamp) +"&type=custom&attributeCodes[]=OP1&interval=15mins";

    HTTPClient http;
    http.begin(requestUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-Authorization", bearerToken);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("Response VRM: ");
      StaticJsonDocument<4096> doc;
      DeserializationError error = deserializeJson(doc, response);

      if (error) {
        Serial.print("JSON parsing failed: ");Serial.println(error.f_str());
        OP1_avg = NAN;
        return;
      }

      JsonArray op1Array = doc["records"]["OP1"];
      if (!op1Array.isNull() && op1Array.size() > 0) {
        OP1_avg = op1Array[op1Array.size() - 1][1];  // Take latest 15min average
        if (OP1_avg < 0) OP1_avg = 0; 
        Serial.print("   Power : ");Serial.print(OP1_avg); Serial.println(" W");
      } else {OP1_avg = NAN;Serial.println("Power data not available.");}
    } else {Serial.print("Error on VRM HTTP request: ");Serial.println(httpResponseCode); OP1_avg = NAN;}
    http.end();
  }
}

// Setup NTP for Unix time
void setupTime() {
  configTime(0, 0, "pool.ntp.org");  // NTP server
  delay(2000);  // Allow time for NTP to set up
}

// Function to get the current Unix timestamp
unsigned long getCurrentTimestamp() {
  time_t now;time(&now);
  return (unsigned long)now;
}
