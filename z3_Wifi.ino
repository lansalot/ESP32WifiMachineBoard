
// setup and connect to wifi network
void configWifi (void) {

  Serial.println("\nStart of configWifi()");

  // connect to wifi
  WiFi.mode(WIFI_STA);  // station mode: the ESP32 connects to an access point
  WiFi.begin(ssid, password);
  Serial.println((String)"Attempting to connect to ssid:" + ssid + " password:" + password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting........");
    delay(500);
  }

  Serial.println("Successfully connected to the WiFi network");

  // RSSI stuff
  int rssi = WiFi.RSSI();
  Serial.println((String)"\nSignal Strength RSSI : " + rssi + " dB");

  Serial.print("RSSI > -30 dB Amazing");
  if (-30 <= rssi) Serial.print("\t\t\t <<< < Current signal strength");

  Serial.print("\nRSSI > -55 dB Very good");
  if (inRange(rssi, -55, -30)) Serial.print("\t\t\t <<<< Current signal strength");

  Serial.print("\nRSSI > -67 dB Fairly good");
  if (inRange(rssi, -67, -55)) Serial.print("\t\t\t <<<< Current signal strength");

  Serial.print("\nRSSI > -70 dB Okay");
  if (inRange(rssi, -70, -67)) Serial.print("\t\t\t <<<< Current signal strength");

  Serial.print("\nRSSI > -80 dB Not good");
  if (inRange(rssi, -80, -70)) Serial.print("\t\t\t <<<< Current signal strength");

  Serial.print("\nRSSI < -80 dB Extremely weak signal");
  if (rssi < -80) Serial.print("\t\t\t <<< < Current signal strength");

  Serial.println("\n");

  // IP address stuff
  myIp = WiFi.localIP();
  Serial.println((String)"ESP32 IP address (provided by router): " + myIp[0] + "." + myIp[1] + "." + myIp[2] + "." + myIp[3]);

  myIp[3] = lastIPOctet;
  Serial.println((String)"Changing IP address last octet to user setting, new desired IP address: " + myIp[0] + "." + myIp[1] + "." + myIp[2] + "." + myIp[3]);
  while (!WiFi.config(myIp, WiFi.gatewayIP(), {255, 255, 255, 0})) {
    Serial.println("!!!!!!!!IP address failing to set!!!!!!!!");
    delay(1000);
  }
  myIp = WiFi.localIP();
  Serial.println((String)"IP address was changed successfully. The current IP address is: " + myIp[0] + "." + myIp[1] + "." + myIp[2] + "." + myIp[3]);
  Serial.println("End of configWifi()");

}
