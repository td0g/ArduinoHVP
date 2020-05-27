//Serial Functions

uint8_t getSerialChar() { //Waits for a Serial char (blocking!)
  while (!SERIAL.available());
  return SERIAL.read();
}

void fillBuff(int n) {    //Collects Serial chars and saves to the buffer
  for (int x = 0; x < n; x++)  buff[x] = getSerialChar();
}

void empty_reply() {      //Sends an OK / NOT OK signal
  if (CRC_EOP == getSerialChar()) {
    SERIAL.print((char)STK_INSYNC);
    SERIAL.print((char)STK_OK);
  } else {
    error++;
    SERIAL.print((char)STK_NOSYNC);
  }
}

void breply(uint8_t b) {  //Sends an OK + byte / NOT OK signal
  if (CRC_EOP == getSerialChar()) {
    SERIAL.print((char)STK_INSYNC);
    SERIAL.print((char)b);
    SERIAL.print((char)STK_OK);
  } else {
    error++;
    SERIAL.print((char)STK_NOSYNC);
  }
}


void get_version(uint8_t c) { //Sends version info over Serial
  switch (c) {
    case 0x80:
      breply(HWVER);
      break;
    case 0x81:
      breply(SWMAJ);
      break;
    case 0x82:
      breply(SWMIN);
      break;
    case 0x93:
      breply('S'); // serial programmer
      break;
    default:
      breply(0);
  }
}


void universal() {
  /*
   * Oh boy...
   * This function sends four RAW bytes from Serial to the microcontroller
   * However, the commands for HV programming are DIFFERENT than commands for normal programming
   * So we need to intercept the commands, try interpret them, and make the correct HV commands
   * 
   * This could use some more work but seems to be functional!
   * -td0g
   */
  uint8_t ch;
  fillBuff(4);
  if (buff[0] == 0x30 && buff[1] == 0){ //0x30 0x00 0b000000aa 0x00 = Signature
    writeHV(0b00001000, 0b01001100);  //Read Signature Bytes
    writeHV(buff[2], 0b00001100);     //Read Signature Bytes
    writeHV(0, 0b01101000);           //Read Signature Bytes
    breply(writeHV(0, 0b01101100));   //Read Signature Bytes
  }
  else if (buff[0] == 0xAC && buff[1] == 0x80 && buff[2] == 0 && buff[3] == 0){ //Erase Chip
    writeHV(0b10000000,0b01001100); //Chip Erase
    writeHV(0, 0b01100100);         //Chip Erase
    writeHV(0, 0b01101100);         //Chip Erase
    breply(0);  //AVRDUDE expects a reply...  Just send a 0
  } else if (buff[0] == 0x58 && buff[1] == 0x08 && buff[2] == 0 && buff[3] == 0){ //Read High Fuse
    writeHV(0x04, 0x4C);
    writeHV(0x00, 0x7A);
    breply(writeHV(0x00, 0x7E));
  } else if (buff[0] == 0xAC && buff[1] == 0xA8 && buff[2] == 0){ //Write High Fuse - buff[3] contains value
    writeHV(0x40, 0x4C);
    writeHV(buff[3], 0x2C);
    writeHV(0x00, 0x74);
    writeHV(0x00, 0x7C);
    breply(0);
  } else if (buff[0] == 0x50 && buff[1] == 0 && buff[2] == 0 && buff[3] == 0){ //Read Low Fuse
    writeHV(0x04, 0x4C);
    writeHV(0x00, 0x68);
    breply(writeHV(0x00, 0x6C));
  } else if (buff[0] == 0xAC && buff[1] == 0xA0 && buff[2] == 0){ //Write Low Fuse - buff[3] contains value
    writeHV(0x40, 0x4C);
    writeHV(buff[3], 0x2C);
    writeHV(0x00, 0x64);
    writeHV(0x00, 0x6C);
    breply(0);
  } else if (buff[0] == 0x50 && buff[1] == 0x08 && buff[2] == 0 && buff[3] == 0){ //Read Ext Fuse
    writeHV(0x04, 0x4C);
    writeHV(0x00, 0x6A);
    breply(writeHV(0x00, 0x7C));
  } else if (buff[0] == 0xAC && buff[1] == 0xA4 && buff[2] == 0){ //Write Ext Fuse - buff[3] contains value
    writeHV(0x40, 0x4C);
    writeHV(buff[3], 0x2C);
    writeHV(0x00, 0x66);
    writeHV(0x00, 0x6C);
    breply(0);
  }
  #ifdef saveDebugToEEPROM
    else {
      EEPROM.write(EEPROMaddress, buff[0]);
      EEPROM.write(EEPROMaddress+1, buff[1]);
      EEPROM.write(EEPROMaddress+2, buff[2]);
      EEPROM.write(EEPROMaddress+3, buff[3]);
      EEPROMaddress += 4;
    }
  #endif
}
