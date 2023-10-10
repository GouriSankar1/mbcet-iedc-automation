#include <ArduinoIoTCloud.h>
#include <WiFiConnectionManager.h>

const char THING_ID[] = "YOUR_THING_ID";
const char DEVICE_ID[] = "YOUR_DEVICE_ID";
const char SSID[] = "YOUR_WIFI_SSID";
const char PASSWORD[] = "YOUR_WIFI_PASSWORD";

int relayPins[] = {D1, D2, D3};  // Define the pins connected to the relays
const int NUM_RELAYS = 3;       // Number of relays

void onSwitchChange(int switchIndex) {
  // Callback function to handle changes in the switch state
  if (ArduinoIoTCloud.getProperty(switchIndex)) {
    digitalWrite(relayPins[switchIndex], ArduinoIoTCloud.getProperty(switchIndex));
  }
}

void setup() {
  for (int i = 0; i < NUM_RELAYS; i++) {
    pinMode(relayPins[i], OUTPUT);
  }

  // Initialize Arduino IoT Cloud
  ArduinoIoTCloud.begin(THING_ID, DEVICE_ID, SSID, PASSWORD);
  
  // Add three switch properties (Switch1, Switch2, Switch3)
  for (int i = 0; i < NUM_RELAYS; i++) {
    ArduinoIoTCloud.addProperty(i, "Switch" + String(i + 1), INT, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    ArduinoIoTCloud.onPropertyChange(i, [i]() { onSwitchChange(i); });
  }

  // Add your additional setup code here if needed
}

void loop() {
  // Update the Arduino IoT Cloud connection
  ArduinoIoTCloud.update();

  // Add your additional loop code here if needed
}
