#include <Arduino.h>




struct ModuleConfig	// about 130 bytes
{
	// RC15
	uint8_t ID = 0;
	char APname[ModStringLengths] = "ESP32Switcher";
	char APpassword[ModStringLengths] = "111222333";
};

ModuleConfig MDL;

IPAddress Wifi_DestinationIP(192, 168, 100, 255);
WiFiUDP UDP_Wifi;


// Try to connect to stored WiFi networks from EEPROM
// Returns true if connected successfully, false otherwise

void configWifi() {
  Serial.println("\n=== Starting WiFi Configuration ===");
  
  // Start Access Point mode for configuration
  Serial.println("Starting Access Point...");
  WiFi.mode(WIFI_MODE_APSTA); // AP + Station mode
  
  // Start AP using credentials from MDL structure
  WiFi.softAPConfig(IPAddress(192,168,200,1), IPAddress(192,168,200,1), IPAddress(255,255,255,0));
	//WiFi.softAP(AP.c_str(), MDL.APpassword, 6, false, 4);
  WiFi.softAP(MDL.APname, MDL.APpassword);
  IPAddress apIP = WiFi.softAPIP();
  Serial.print("Access Point started: ");
  Serial.println(MDL.APname);
  Serial.print("AP Password: ");
  Serial.println(MDL.APpassword);
  Serial.print("AP IP Address: ");
  Serial.println(apIP);
  Serial.println("Connect to this AP to configure WiFi networks");
  WifiUdp.begin(ListeningPort); // port that AgIO sends data out on

  
  // Now attempt to connect to stored networks
  bool connected = connectToStoredNetworks();
  
  if (!connected) {
    Serial.println("\n*** No stored networks connected ***");
    Serial.println("*** AP mode active for configuration ***");
    Serial.println("*** Connect to AP to add WiFi networks ***");
  } else {
    Serial.println("\n=== WiFi Configuration Complete ===");
  }
  startAccessPoint();
}


bool connectToStoredNetworks() {
  Serial.println("\nAttempting to connect to stored WiFi networks...");
  
  for (uint8_t i = 0; i < 3; i++) {
    if (eepromConfig.isNetworkConfigured(i)) {
      const char* storedSSID = eepromConfig.getSSID(i);
      const char* storedPassword = eepromConfig.getPassword(i);
      
      Serial.print("Trying stored network ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(storedSSID);
      
      // Try up to 3 times for this network
      for (uint8_t attempt = 0; attempt < 3; attempt++) {
        if (attempt > 0) {
          Serial.print("Retry attempt ");
          Serial.print(attempt + 1);
          Serial.println(" of 3...");
        }
        
        //WiFi.mode(WIFI_STA);
        WiFi.begin(storedSSID, storedPassword);
        
        // Wait up to 10 seconds for connection
        int connectionAttempts = 0;
        while (WiFi.status() != WL_CONNECTED && connectionAttempts < 20) {
          delay(500);
          Serial.print(".");
          connectionAttempts++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
          Serial.println("\nSuccessfully connected to stored network!");
          
          // Configure IP address
          myIp = WiFi.localIP();
          Serial.println((String)"ESP32 IP address (provided by router): " + myIp[0] + "." + myIp[1] + "." + myIp[2] + "." + myIp[3]);
          myIp[3] = lastIPOctet;
          Serial.println((String)"Changing IP address last octet to user setting, new desired IP address: " + myIp[0] + "." + myIp[1] + "." + myIp[2] + "." + myIp[3]);
          while (!WiFi.config(myIp, WiFi.gatewayIP(), {255, 255, 255, 0})) {
            Serial.println("!!!!!!!!IP address failing to set!!!!!!!!");
            delay(200);
          }
          myIp = WiFi.localIP();
          Serial.println((String)"IP address successfully set to: " + myIp[0] + "." + myIp[1] + "." + myIp[2] + "." + myIp[3]);
          reportWifiStrength();
          return true;
        } else {
          Serial.println("\nConnection attempt failed.");
          WiFi.disconnect();
          delay(1000); // Wait a bit before retrying
        }
      }
      
      Serial.println("Failed to connect after 3 attempts, trying next network...");
    }
  }
  
  Serial.println("No stored networks connected successfully.");
  return false;
}

void reportWifiStrength() {
  
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
}

void startAccessPoint() {
  
  Serial.println("Starting Access Point...");
  WiFi.mode(WIFI_AP_STA); // AP + Station mode
  
  	// Access Point
	Wifi_DestinationIP = IPAddress(192, 168, 200, 255);
	IPAddress AP_LocalIP = IPAddress(192, 168, 200, 1);
	IPAddress AP_GateWay = AP_LocalIP;
	IPAddress AP_Subnet(255, 255, 255, 0);

	uint64_t mac = ESP.getEfuseMac();
	uint32_t low32 = (uint32_t)(mac & 0xFFFFFFFF);

	char suffix[9]; // 8 hex + null
	sprintf(suffix, "%08X", low32);

	String AP = MDL.APname;
	AP += "_";
	AP += suffix;

	WiFi.softAPConfig(AP_LocalIP, AP_GateWay, AP_Subnet);
	WiFi.softAP(AP.c_str(), MDL.APpassword, 6, false, 4);
	//dnsServer.start(DNS_PORT, "*", AP_LocalIP);

	UDP_Wifi.begin(ListeningPort);

	Serial.println("");
	Serial.print("Access Point name: ");
	Serial.println(AP);
	Serial.print("Settings Page IP: ");
	Serial.println(AP_LocalIP);

	// web server
	Serial.println();
	Serial.println("Starting Web Server");

	server.on("/", HandleRoot);
	server.on("/page1", HandlePage1);
	server.on("/page2", HandlePage2);
	server.on("/style.css", HandleCSS);
	server.on("/ButtonPressed", ButtonPressed);
	server.onNotFound(HandleRoot);

	server.on("/generate_204", []() {server.send(204, "text/plain", "");	});	
	server.on("/fwlink", []() { server.send(200, "text/plain", "OK"); });	
	server.on("/hotspot-detect.html", HTTP_GET, []() { server.send(200, "text/html", "<html><body>Portal</body></html>"); });
	server.on("/ncsi.txt", HTTP_GET, []() { server.send(200, "text/plain", "Microsoft NCSI"); });

	// OTA
	server.on("/myurl", HTTP_GET, []() {
		server.sendHeader("Connection", "close");
		server.send(200, "text/plain", "Hello there!");
	});

	server.begin();

	/* INITIALIZE ESP2SOTA LIBRARY */
	ESP2SOTA.begin(&server);

	Serial.println("OTA started.");

  // Start AP using credentials from MDL structure
  WiFi.softAP(MDL.APname, MDL.APpassword);
  IPAddress apIP = WiFi.softAPIP();
  Serial.print("Access Point started: ");
  Serial.println(MDL.APname);
  Serial.print("AP Password: ");
  Serial.println(MDL.APpassword);
  Serial.print("AP IP Address: ");
  Serial.println(apIP);
  Serial.println("Connect to this AP to configure WiFi networks");
}