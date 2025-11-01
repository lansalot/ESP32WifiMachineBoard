
void configUDP(void)
{
  Serial.println("\nSetting UDP Ports");
  WifiUdp.begin(8888); // port that AgIO sends data out on
  Serial.println("UDP Ports set successfully");
}

void ReceiveUdp()
{

  // When wifi is not running, return directly. parsePacket() will block if not connected
  if (WiFi.status() != WL_CONNECTED)
    return;

  // Check for a packet length > 4 (because we have to check byte 0, 1, 2 and 3)
  if (WifiUdp.parsePacket() > 4)
  {
    // read in the packet
    WifiUdp.read(autoSteerUdpData, sizeof(autoSteerUdpData));

    // The first four bytes from AOG are always [0x80, 0x81, 0x7F, 0xPGN] aka [128, 129, 127, PGN]
    if (autoSteerUdpData[0] == 0x80 && autoSteerUdpData[1] == 0x81 && autoSteerUdpData[2] == 0x7F)
    {

      lastTimePgnReceived = currentMillis;

      // PGN_0xC8_200 Hello from AgIO, respond with reply to AgIO
      if (autoSteerUdpData[3] == 0xC8)
      {

        // Create the Machine Reply message. (0x80, 0x81, 123, 123, 5, relayLo, relayHi, *, *, *, CRC}
        uint8_t helloFromMachineModule[11] = {0x80, 0x81, 123, 123, 5, 0, 0, 0, 0, 0, 0};
        for (uint8_t i = 0; i < 8; i++)
        {
          helloFromMachineModule[5] |= sectionStates[i] << i;     // relayLo
          helloFromMachineModule[6] |= sectionStates[i + 8] << i; // relayHi
        }

        // Send the reply message. This should make the machine module in AgIO turn green
        SendUdp(helloFromMachineModule, sizeof(helloFromMachineModule), {255, 255, 255, 255}, 9999);

      }

      // PGN_0xC9_201 AgIO subnet change. AgIO would like us to change to this subnet.
      else if (autoSteerUdpData[3] == 201)
      {

        myIp = WiFi.localIP();

        if ((autoSteerUdpData[7] == myIp[0]) && (autoSteerUdpData[8] == myIp[1]) && (autoSteerUdpData[9] == myIp[2]))
        {
          if (debug)
            Serial.print("\tWe're already on that subnet, so no need to do anything");
        }
        else
        {
          if (debug)
            Serial.print("\tChanging IP...");
          myIp[0] = autoSteerUdpData[7];
          myIp[1] = autoSteerUdpData[8];
          myIp[2] = autoSteerUdpData[9];

          while (!WiFi.config(myIp, WiFi.gatewayIP(), {255, 255, 255, 0}))
          {
            if (debug)
              Serial.println("!!!!!!!!IP address failing to set!!!!!!!!");
            delay(1000);
          }
          myIp = WiFi.localIP();
          if (debug)
            Serial.println((String) "\tIP address was changed successfully to: " + myIp[0] + "." + myIp[1] + "." + myIp[2] + "." + myIp[3]);
        }
      }

      // PGN_0xCA_202 Scan Request from AgIO, reply with ip and Src.
      // myIP is what will be shown in AgIO Ethernet Configuration Module Scan Machine
      else if (autoSteerUdpData[3] == 202)
      {
        if (debug)
          Serial.print("PGN_0xCA_202 Scan Request from AgIO");

        // Create the Machine Reply message. (0x80, 0x81, 123, 203, 7, ipOne, ipTwo, ipThree, 123, SrcOne, SrcTwo, SrcThree, CRC}
        myIp = WiFi.localIP();
        IPAddress rem_ip = WifiUdp.remoteIP();
        uint8_t scanReply[13] = {0x80, 0x81, 123, 203, 7, myIp[0], myIp[1], myIp[2], myIp[3], rem_ip[0], rem_ip[1], rem_ip[2], 0};

        // Send the reply message
        SendUdp(scanReply, sizeof(scanReply), {255, 255, 255, 255}, 9999);

        if (debug)
        {
          Serial.print("\tReply message: ");
          for (int i = 0; i < sizeof(scanReply); i++)
            Serial.print((String)scanReply[i] + " ");
          Serial.println();
        }
      }

      // PGN_0xE5_229 Machine Data 64 sections + tip speed
      else if (autoSteerUdpData[3] == 0xE5)
      {

        // read state of 64 sections
        for (uint8_t j = 0; j < 8; j++)
        {
          for (uint8_t i = 0; i < 8; i++)
          {
            sectionStates[i + j * 8] = bitRead(autoSteerUdpData[5 + j], i);
          }
        }

        if (debug)
        {
          Serial.print("PGN_0xE5_229 Machine Module 64 sections: ");
          for (int i = 0; i < sizeof(sectionStates); i++)
            Serial.print(sectionStates[i]);
          Serial.println();
        }
      }

      // PGN_0xEC_236 Pin COnfig
      else if (autoSteerUdpData[3] == 0xEC)
      {

        // read the 24 pin configs
        for (uint8_t i = 0; i < 24; i++)
        {
          pinConfig[i] = autoSteerUdpData[5 + i];
        }

        if (debug)
          Serial.print("PGN_0xEC_236 Pin Config: ");
        for (uint8_t i = 0; i < 24; i++)
        {
          if (debug)
            Serial.print(pinConfig[i]);
          if (debug)
            Serial.print("'");
        }
        if (debug)
          Serial.println();
      }

      // PGN_0xEF_239 Machine Data
      else if (autoSteerUdpData[3] == 0xEF)
      {

        // sc 1-16, hyd up, hyd down, tram right, tram left, geostop

        // sections 1-8
        for (uint8_t i = 0; i < 8; i++)
        {
          machineData[i] = bitRead(autoSteerUdpData[11], i);
        }

        // sections 9-16
        for (uint8_t i = 0; i < 8; i++)
        {
          machineData[i + 8] = bitRead(autoSteerUdpData[12], i);
        }

        machineData[16] = bitRead(autoSteerUdpData[7], 0); // hyd up
        machineData[17] = bitRead(autoSteerUdpData[7], 1); // hyd down
        machineData[18] = bitRead(autoSteerUdpData[8], 0); // tram left
        machineData[19] = bitRead(autoSteerUdpData[8], 1); // tram right
        machineData[20] = bitRead(autoSteerUdpData[9], 0); // geo stop

        if (debug)
          Serial.print("PGN_0xEF_239 Machine Data: ");
        for (uint8_t i = 0; i < 21; i++)
        {
          if (debug)
            Serial.print(machineData[i]);
          if (debug)
            Serial.print("'");
        }
        if (debug)
          Serial.println();
      }
    }
  }
}

void SendUdp(uint8_t *data, uint8_t datalen, IPAddress dip, uint16_t dport)
{

  // calculate and set CRC
  uint8_t crc = 0;
  for (byte i = 2; i < datalen - 1; i++)
  {
    crc = (crc + data[i]);
  }
  data[datalen - 1] = crc;

  WifiUdp.beginPacket(dip, dport);
  WifiUdp.write(data, datalen);
  WifiUdp.endPacket();
}
