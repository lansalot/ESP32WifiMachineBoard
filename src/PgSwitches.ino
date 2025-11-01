#include <Arduino.h>
#include <LittleFS.h>

// Forward declarations
extern bool WifiMasterOn;
extern bool Button[16];

String tmp;
String GetPage1()
{
	// Start with HTML header - link to external CSS
	String st = R"rawliteral(<HTML>
  <head>
    <META content='text/html; charset=utf-8' http-equiv=Content-Type>
    <meta name=vs_targetSchema content='HTML 4.0'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>Wifi AOG</title>
    <link rel="stylesheet" href="/style.css">
  </head>

  <BODY>
    <h1 align=center>Switches</h1>
    <form id=FORM1 method=post action='/'>&nbsp;
)rawliteral";

	// Add dynamic buttons
	if (WifiMasterOn) tmp = "button-on"; else tmp = "button-off";
	st += "      <p> <input class='" + tmp + "' name='Btn' type=submit formaction='/ButtonPressed' value='Master'> </p>";

	for (int i = 0; i < 16; i++)
	{
		if (Button[i]) tmp = "button-on"; else tmp = "button-off";
		st += "      <p> <input class='" + tmp + "' name='Btn' type=submit formaction='/ButtonPressed' value='" + String(i + 1) + "'> </p>";
	}

	st += R"rawliteral(
      <p> <a href='/page0'>Back</a> </p>
    </form>
</HTML>
)rawliteral";

	return st;
}




