#include <Arduino.h>
#include <WebServer.h>
#include <LittleFS.h>

// Forward declarations for functions defined in other .ino files
String GetPage0();
String GetPage1();
String GetPage2();

// External variable declarations (defined in z9_main.ino)
extern WebServer server;
extern bool WifiMasterOn;
extern uint32_t WifiSwitchesTimer;
extern bool Button[16];

const uint8_t ModStringLengths = 15;

struct ModuleNetwork
{
	uint16_t Identifier = 9876;
	uint8_t IP0 = 192;
	uint8_t IP1 = 168;
	uint8_t IP2 = 1;
	uint8_t IP3 = 50;
	bool WifiModeUseStation = true;				    // false - AP mode, true - AP + Station 
	char SSID[ModStringLengths] = "Tractor";		// name of network ESP32 connects to
	char Password[ModStringLengths] = "111222333";
};

ModuleNetwork MDLnetwork;

void HandleRoot()
{
	if (server.hasArg("prop1"))
	{
		handleCredentials();
	}
	else
	{
		server.send(200, "text/html", GetPage0());
	}
}

void HandlePage1()
{
	// switches
	server.send(200, "text/html", GetPage1());
}

void HandlePage2()
{
	// network
	server.send(200, "text/html", GetPage2());
}

void HandleCSS()
{
	// Serve the CSS file from LittleFS
	File file = LittleFS.open("/style.css", "r");
	if (!file) {
		server.send(404, "text/plain", "CSS file not found");
		return;
	}
	server.streamFile(file, "text/css");
	file.close();
}

void handleCredentials()
{
	bool OldMode = MDLnetwork.WifiModeUseStation;
	String OldSSID = String(MDLnetwork.SSID);
	String OldPassword = String(MDLnetwork.Password);

	String newSSID = server.arg("prop1");
	newSSID.trim();  
	String newPassword = server.arg("prop2");
	newPassword.trim();

	newSSID.toCharArray(MDLnetwork.SSID, sizeof(MDLnetwork.SSID));
	newPassword.toCharArray(MDLnetwork.Password, sizeof(MDLnetwork.Password));
	MDLnetwork.WifiModeUseStation = server.hasArg("connect");

	server.send(200, "text/html", GetPage0());

	if (MDLnetwork.WifiModeUseStation != OldMode ||
		String(MDLnetwork.SSID) != OldSSID ||
		String(MDLnetwork.Password) != OldPassword)
	{
		//SaveNetworks();
		delay(3000);
		ESP.restart();
	}
}

void ButtonPressed()
{
	if (server.arg("Btn") == "Master")
	{
		WifiMasterOn = !WifiMasterOn;
		WifiSwitchesTimer = millis();
		HandlePage1();
	}
	else
	{
		int ID = server.arg("Btn").toInt() - 1;
		if (ID >= 0 && ID < 16)
		{
			Button[ID] = !Button[ID];
			WifiSwitchesTimer = millis();
			HandlePage1();
		}
	}
}




