#ifdef ENABLE_DEBUG
       #define DEBUG_ESP_PORT Serial
       #define NODEBUG_WEBSOCKETS
       #define NDEBUG
#endif 

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "SinricPro.h"
#include "SinricProSwitch.h"

#include <map>

//Edit here
#define WIFI_SSID         "************"     //Add your Wi-Fi name here   
#define WIFI_PASS         "************"     //Add your Wi-Fi password here   
#define APP_KEY           "*************************************************************************"     //Add your APP_KEY here   
#define APP_SECRET        "*************************************************************************"     //Add your APP_SECRET here   

//Edit here
#define device_ID_1   "************************"     //Add your device_ID_1 here   
#define device_ID_2   "************************"     //Add your device_ID_2 here    
#define device_ID_3   "************************"     //Add your device_ID_3 here   
#define device_ID_4   "************************"     //Add your device_ID_4 here   

// define pins
#define RelayPin1 5  //D1 of ESP
#define RelayPin2 4  //D2 of ESP
#define RelayPin3 14 //D5 of ESP
#define RelayPin4 12 //D6 of ESP

#define SwitchPin1 10  //SD3 of ESP 
#define SwitchPin2 0   //D3 of ESP 
#define SwitchPin3 13  //D7 of ESP
#define SwitchPin4 3   //RX of ESP

#define wifiLed   16   //D0 of ESP

#define BAUD_RATE   9600

#define DEBOUNCE_TIME 250

typedef struct {      
  int relayPIN;
  int flipSwitchPIN;
} deviceConfig_t;



std::map<String, deviceConfig_t> devices = {
    {device_ID_1, {  RelayPin1, SwitchPin1 }},
    {device_ID_2, {  RelayPin2, SwitchPin2 }},
    {device_ID_3, {  RelayPin3, SwitchPin3 }},
    {device_ID_4, {  RelayPin4, SwitchPin4 }}     
};

typedef struct {     
  String deviceId;
  bool lastFlipSwitchState;
  unsigned long lastFlipSwitchChange;
} flipSwitchConfig_t;

std::map<int, flipSwitchConfig_t> flipSwitches;    
                                                 
void setupRelays() { 
  for (auto &device : devices) {           
    int relayPIN = device.second.relayPIN; 
    pinMode(relayPIN, OUTPUT);             
    digitalWrite(relayPIN, HIGH);
  }
}

void setupFlipSwitches() {
  for (auto &device : devices)  {                    
    flipSwitchConfig_t flipSwitchConfig;              

    flipSwitchConfig.deviceId = device.first;         
    flipSwitchConfig.lastFlipSwitchChange = 0;      
    flipSwitchConfig.lastFlipSwitchState = true;     

    int flipSwitchPIN = device.second.flipSwitchPIN; 

    flipSwitches[flipSwitchPIN] = flipSwitchConfig;   
    pinMode(flipSwitchPIN, INPUT_PULLUP);                 
  }
}

bool onPowerState(String deviceId, bool &state)
{
  Serial.printf("%s: %s\r\n", deviceId.c_str(), state ? "on" : "off");
  int relayPIN = devices[deviceId].relayPIN; 
  digitalWrite(relayPIN, !state);            
  return true;
}

void handleFlipSwitches() {
  unsigned long actualMillis = millis();                                         
  for (auto &flipSwitch : flipSwitches) {                                        
    unsigned long lastFlipSwitchChange = flipSwitch.second.lastFlipSwitchChange;  

    if (actualMillis - lastFlipSwitchChange > DEBOUNCE_TIME) {                    

      int flipSwitchPIN = flipSwitch.first;                                       
      bool lastFlipSwitchState = flipSwitch.second.lastFlipSwitchState;          
      bool flipSwitchState = digitalRead(flipSwitchPIN);                         
      if (flipSwitchState != lastFlipSwitchState) {                               
#ifdef TACTILE_BUTTON
        if (flipSwitchState) {                                                   
#endif      
          flipSwitch.second.lastFlipSwitchChange = actualMillis;                 
          String deviceId = flipSwitch.second.deviceId;                          
          int relayPIN = devices[deviceId].relayPIN;                            
          bool newRelayState = !digitalRead(relayPIN);                           
          digitalWrite(relayPIN, newRelayState);                                  

          SinricProSwitch &mySwitch = SinricPro[deviceId];                      
          mySwitch.sendPowerStateEvent(!newRelayState);                          
#ifdef TACTILE_BUTTON
        }
#endif      
        flipSwitch.second.lastFlipSwitchState = flipSwitchState;                
      }
    }
  }
}

void setupWiFi()
{
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.printf(".");
    delay(250);
  }
  digitalWrite(wifiLed, LOW);
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

void setupSinricPro()
{
  for (auto &device : devices)
  {
    const char *deviceId = device.first.c_str();
    SinricProSwitch &mySwitch = SinricPro[deviceId];
    mySwitch.onPowerState(onPowerState);
  }

  SinricPro.begin(APP_KEY, APP_SECRET);
  SinricPro.restoreDeviceStates(true);
}

void setup()
{
  Serial.begin(BAUD_RATE);

  pinMode(wifiLed, OUTPUT);
  digitalWrite(wifiLed, HIGH);

  setupRelays();
  setupFlipSwitches();
  setupWiFi();
  setupSinricPro();
}

void loop()
{
  SinricPro.handle();
  handleFlipSwitches();
}



