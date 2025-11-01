#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>

// Forward declarations
//extern ModuleConfig MDL;

String GetPage2()
{
	File file = LittleFS.open("/page2.html", "r");
	if (!file) {
		return "<html><body><h1>Error: page2.html not found</h1></body></html>";
	}
	String st = file.readString();
	file.close();
	
	// Add dynamic WiFi status
	if (WiFi.isConnected())
	{
		st += "<p>Wifi Connected</p>";
	}
	else
	{
		st += "<p>Wifi Not Connected</p>";
	}
	
	st += "</body></HTML>";
	
	return st;
}






