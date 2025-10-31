
// Sets pins as output and to their default state
// There is a rudimentary pin check that may catch pin assignment errors (cause the program crash)
void configOutputPins(void)
{

  Serial.println("\nStart of pinMode(pin, OUTPUT) config");
  Serial.println("Different ESP32 boards have different pinouts");
  Serial.println("It is possible and confusing to config a wrong pin");
  Serial.println("In that case the serial monitor will garble and restart");
  Serial.println("The garble/restart may not occur at the wrong pin, it may be delayed");

  Serial.print((String) "Setting pinLedWifi to pin " + pinLedWifi + "...");
  pinMode(pinLedWifi, OUTPUT);
  digitalWrite(pinLedWifi, LOW);
  Serial.println((String) "\t\t Pin state:" + digitalRead(pinLedWifi) + " pinMode config complete");

  Serial.print((String)sizeof(pinRelays) + " relay pins were declared: {");
  Serial.print(pinRelays[0]);
  for (int i = 1; i < sizeof(pinRelays); i++)
  {
    Serial.print(", ");
    Serial.print(pinRelays[i]);
  }
  Serial.println("}");

  for (int i = 0; i < sizeof(pinRelays); i++)
  {
    Serial.print((String) "Setting relay[" + i + "] to pin " + pinRelays[i] + "...");
    pinMode(pinRelays[i], INPUT);
    pinMode(pinRelays[i], OUTPUT);
    digitalWrite(pinRelays[i], !pinStateActive);
    Serial.println((String) "\t\t Pin state:" + digitalRead(pinRelays[i]) + " pinMode config complete");
  }

  updatePinStates(); // should set everything to off, required in case of hi-Z
  Serial.println("End of pinMode(pin, OUTPUT) config");
}

// Update pin states
void updatePinStates()
{
  for (int i = 0; i < 16; i++)
  {
    if (sectionStates[i])
    {
      relayBoard.setRelay(i,true);
    }
    else
    {
      relayBoard.setRelay(i,false);
    }
  }
  // if (!highZ) {
  //   for (int i = 0; i < sizeof(pinRelays); i++) {
  //     digitalWrite(pinRelays[i], sectionStates[i] ^ !pinStateActive);
  //   }
  // }
  // else {
  //   for (int i = 0; i < sizeof(pinRelays); i++) {
  //     if (sectionStates[i] ^ !pinStateActive) {
  //       pinMode(pinRelays[i], INPUT);
  //     }
  //     else {
  //       pinMode(pinRelays[i], OUTPUT);
  //       digitalWrite(pinRelays[i], LOW);
  //     }
  //   }
  // }
}
