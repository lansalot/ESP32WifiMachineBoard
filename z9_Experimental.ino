
//void updateAndSendPGNSwitch() {
//  switchControl[0] = 0x80;
//  switchControl[1] = 0x81;
//  switchControl[2] = 77;                // SRC
//  switchControl[3] = 234;               // PGN
//  switchControl[4] = 8;                 // number of bytes, not including header or CRC
//  switchControl[5] = 0;//planterDown;    // Main
//  switchControl[6] = 0;                 // Res
//  switchControl[7] = 0;                 // Res
//  switchControl[8] = 16;                // # sections
//  switchControl[9] = 0;                 // on group 0
//  switchControl[10] = 0;//!planterDown * 255;// off group 0
//  switchControl[11] = planterDown;                // on group 1
//  switchControl[12] = 0;                // off group 1
//  switchControl[13] = 0;                // CRC
//  calculateAndSetCRC(switchControl, sizeof(switchControl));
//  SendUdp(switchControl, sizeof(switchControl), {255, 255, 255, 255}, 9999);
//}
