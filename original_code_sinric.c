#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define DEBUG_ESP_PORT Serial
#define NODEBUG_WEBSOCKETS
#define NDEBUG
#endif

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>
#include <map>

#define WIFI_SSID "IOT-2.4G"
#define WIFI_PASS "2022iedcmbcet"
#define APP_KEY "08bf5a87-3b03-469b-bc95-09d7af608707"
#define APP_SECRET "c52e7831-4f1d-4b33-a4c4-415f73bb1ca1-534b55ca-e802-4c16-96c5-ccf490ecdfda"

#define device_ID_1 "65005548e2a1e41147659333"
#define device_ID_2 "6500557cb1deae87502153be"
#define device_ID_3 "65005591b1deae8750215407"

#define RelayPin1 5  // D1
#define RelayPin2 4  // D2
#define RelayPin3 14 // D5

#define SwitchPin1 10  // SD3
#define SwitchPin2 0   // D3
#define SwitchPin3 13  // D7

#define wifiLed 16  // D0

#define BAUD_RATE 9600
#define DEBOUNCE_TIME 250

typedef struct {      // struct for the std::map below
  int relayPIN;
  int flipSwitchPIN;
} deviceConfig_t;

std::map<String, deviceConfig_t> devices = {
    {device_ID_1, {D1, SwitchPin1}},
    {device_ID_2, {D2, SwitchPin2}},
    {device_ID_3, {D5, SwitchPin3}},
};

typedef struct {      // struct for the std::map below
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
  for (auto &device : devices) {
    flipSwitchConfig_t flipSwitchConfig;
    flipSwitchConfig.deviceId = device.first;
    flipSwitchConfig.lastFlipSwitchChange = 0;
    flipSwitchConfig.lastFlipSwitchState = true;
    int flipSwitchPIN = device.second.flipSwitchPIN;
    flipSwitches[flipSwitchPIN] = flipSwitchConfig;
    pinMode(flipSwitchPIN, INPUT_PULLUP);
  }
}

bool onPowerState(String deviceId, bool &state) {
  Serial.printf("%s: %s\r\n", deviceId.c_str(), state ? "on" : "off");
  int relayPIN = devices[deviceId].relayPIN;
  digitalWrite(relayPIN, state);
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
        flipSwitch.second.lastFlipSwitchChange = actualMillis;
        String deviceId = flipSwitch.second.deviceId;
        int relayPIN = devices[deviceId].relayPIN;
        digitalWrite(relayPIN, flipSwitchState);
        SinricProSwitch &mySwitch = SinricPro[deviceId];
        mySwitch.sendPowerStateEvent(flipSwitchState);
        flipSwitch.second.lastFlipSwitchState = flipSwitchState;
      }
    }
  }
}

void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  digitalWrite(wifiLed, LOW);
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

void setupSinricPro() {
  for (auto &device : devices) {
    const char *deviceId = device.first.c_str();
    SinricProSwitch &mySwitch = SinricPro[deviceId];
    mySwitch.onPowerState(onPowerState);
  }
  SinricPro.begin(APP_KEY, APP_SECRET);
  SinricPro.restoreDeviceStates(true);
}

void setup() {
  Serial.begin(BAUD_RATE);
  pinMode(wifiLed, OUTPUT);
  digitalWrite(wifiLed, HIGH);
  setupRelays();
  setupFlipSwitches();
  setupWiFi();
  setupSinricPro();
}

void loop() {
  SinricPro.handle();
  handleFlipSwitches();
}
