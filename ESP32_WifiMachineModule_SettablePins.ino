#include "z0ESP32RelayClass.ino"
// Wifi udp section control using ESP32 for AgOpenGPS (written/tested with v6.0.5)
// Written by WildBuckwheat using Arduino IDE 1.8.19
// Started on 2024-02-06
// Finished on 2024-02-27
// Adapted for 16-channel relay board from Amazon (AndyAOG)
// Started 2025-10-31


RelayBoard relayBoard(12, 13, 14, 5);
#include <WiFi.h> // version 1.2.7

// Enter Wifi details here
const char *ssid = "xxxx";
const char *password = "xxxx";
const uint8_t lastIPOctet = 123; // The IP address will be set to xxx.xxx.xxx.lastIPOctet. Machine modules are expected to end in 123

// Debug variables
const bool debugWaitForMe = 0; // wait for someone to open a serial connection before proceeding with setup
const bool debug = 0;          // additional serial messages to aid debug

// Program variables
bool sectionStates[64]; // holds the state of AOG's sections. AOG supports up to 64 sections, we read all of them
uint8_t pinConfig[24];  // uses the pin config's from the Arduino section of AOG (Do I care about this??)
bool machineData[21];   // sc 1-16, hyd up, hyd down, tram right, tram left, geostop
IPAddress myIp;
WiFiUDP WifiUdp = WiFiUDP();
uint8_t autoSteerUdpData[30]; // Buffer For Receiving UDP Data
unsigned long currentMillis;
unsigned long lastTimePgnReceived;
unsigned long lastWifiLedService;
unsigned long lastWifiReconnectMillis;
unsigned long lastHeartBeatMillis;

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println("\n\nESP32_WifiSectionControl");

  if (debugWaitForMe)
  { // wait for you to open serial monitor
    while (!Serial.available())
    {
      Serial.println("ESP32_WifiSectionControl debug - Send any char to continue");
      delay(2000);
    }
  }

  Serial.println("Instantiating relay board");
  relayBoard.begin();
  Serial.println("Setup complete");
  configWifi();
  configUDP();
}

void loop()
{
  // put your main code here, to run repeatedly:

  currentMillis = millis();

  // receive UDP as often as possible
  ReceiveUdp();

  // if Wifi is down attempt to re-connect
  if (WiFi.status() != WL_CONNECTED)
  {
    if ((currentMillis - lastWifiReconnectMillis >= 5000))
    {
      Serial.print(millis());
      Serial.println(" Wifi is down, Reconnecting to WiFi...");
      WiFi.disconnect();
      WiFi.reconnect();
      lastWifiReconnectMillis = currentMillis;
    }
  }

  // if no PGNs received for a long time then set pins to failsafe state
  if ((currentMillis - lastTimePgnReceived) > 10000)
  {
    relayBoard.allOff();
    Serial.println((String) "No PGNS received for a long time!!!! Setting all relays Off!");
  }

  if ((currentMillis - lastWifiLedService) >= 1000)
  {
    if ((WiFi.status() == WL_CONNECTED) && ((currentMillis - lastTimePgnReceived) < 1000))
    {
      //sendHardwareMessage("FUCK!");
    }

    lastWifiLedService = currentMillis;
  }

  updatePinStates();
}
