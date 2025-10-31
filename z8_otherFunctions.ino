
// inclusive minimum, not inclusive maximum
bool inRange(int val, int minimum, int maximum) {
  return ((minimum <= val) && (val < maximum));
}


// incorporated into sendUDP()
//// calculate and sets the last byte as CRC8 for messages to AgIO
//void calculateAndSetCRC(uint8_t myMessage[], uint8_t myLen) {
//  if (myLen <= 2 ) return;
//
//  uint8_t crc = 0;
//  for (byte i = 2; i < myLen - 1; i++) {
//    crc = (crc + myMessage[i]);
//  }
//  myMessage[myLen - 1] = crc;
//}
