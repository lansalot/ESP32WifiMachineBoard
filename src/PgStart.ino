#include <Arduino.h>
#include <LittleFS.h>

String GetPage0()
{
	File file = LittleFS.open("/page0.html", "r");
	if (!file) {
		return "<html><body><h1>Error: page0.html not found</h1></body></html>";
	}
	String content = file.readString();
	file.close();
	return content;
}

