#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char WIFI_SSID[] = "IOT-2.4G";
const char WIFI_PASS[] = "2022iedcmbcet";

const char DEVICE_ID[] = "486ca265-3779-43dd-8d6a-723e7fda6466";
const char THING_ID[] = "3f92035e-2bb9-4917-b099-3df9cdf26313";

const int RELAY_PIN_1 = D1; // GPIO pin for Relay 1
const int RELAY_PIN_2 = D2; // GPIO pin for Relay 2
const int RELAY_PIN_3 = D3; // GPIO pin for Relay 3

bool stateSwitch1 = false;
bool stateSwitch2 = false;
bool stateSwitch3 = false;

void onPowerStateChange() {
  if (ArduinoIoTCloud.readPower(stateSwitch1) == true) {
    digitalWrite(RELAY_PIN_1, HIGH);
    stateSwitch1 = true;
  } else {
    digitalWrite(RELAY_PIN_1, LOW);
    stateSwitch1 = false;
  }
}

void onPowerStateChange2() {
  if (ArduinoIoTCloud.readPower(stateSwitch2) == true) {
    digitalWrite(RELAY_PIN_2, HIGH);
    stateSwitch2 = true;
  } else {
    digitalWrite(RELAY_PIN_2, LOW);
    stateSwitch2 = false;
  }
}

void onPowerStateChange3() {
  if (ArduinoIoTCloud.readPower(stateSwitch3) == true) {
    digitalWrite(RELAY_PIN_3, HIGH);
    stateSwitch3 = true;
  } else {
    digitalWrite(RELAY_PIN_3, LOW);
    stateSwitch3 = false;
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  pinMode(RELAY_PIN_3, OUTPUT);

  // Initialize Arduino IoT Cloud
  ArduinoIoTCloud.begin(ArduinoIoTCloud.PRODUCTION, DEVICE_ID, THING_ID);

  // Set callback functions for power state changes
  ArduinoIoTCloud.observePower(onPowerStateChange);
  ArduinoIoTCloud.observePower(onPowerStateChange2);
  ArduinoIoTCloud.observePower(onPowerStateChange3);

  // Read the initial state of your relays and set the initial state variables
  stateSwitch1 = digitalRead(RELAY_PIN_1) == HIGH;
  stateSwitch2 = digitalRead(RELAY_PIN_2) == HIGH;
  stateSwitch3 = digitalRead(RELAY_PIN_3) == HIGH;
}

void loop() {
  ArduinoIoTCloud.update();
}
