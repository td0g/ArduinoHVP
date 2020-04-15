//Function to set fuses
//See datasheet and http://www.rickety.us/2010/03/arduino-avr-high-voltage-serial-programmer/

void burnFuses(){
  byte hfuse = 0b01011111; //0xDF;  //Default, ATTINY85 Datasheet pp. 148
  byte lfuse = 0b01100010; //0x62;  //Default
  Serial.println(F("Please select fuse settings"));
  Serial.println(F("0 - Default\n1 - Disable Reset\n2 - 8MHz Clock\n3 - Disable Reset, 8MHz Clock"));
  Serial.println(F("4 - EEPROM Preserve\n5 - EEPROM Preserve,Disable Reset\n6 - EEPROM Preserve,8MHz Clock\n7 - EEPROM Preserve,Disable Reset, 8MHz Clock"));
  Serial.println(F("9 - Read Only"));
  while (Serial.available()) Serial.read();
  while (!Serial.available()){};
  byte thisChar = Serial.read();
  Serial.println(F("Entering Programming Mode"));
  if (!pmode) start_pmode();
  readFusesHV();
  if (thisChar != '9'){
    if (thisChar & 0b00000001)  hfuse &= 0b01111111;
    if (thisChar & 0b00000010) lfuse |= 0b10000000;
    if (thisChar & 0b00000100) hfuse &= 0b11110111;
    writeFusesHV(lfuse, hfuse);
    readFusesHV();
  }
  else {
    here = 0;
    for (byte i = 0; i < 32; i++){
      Serial.print("0x");
      writeHV(0b00000010, 0b01001100);  //Load "Read Flash" Command
      pmode = 3;
      Serial.println(flash_read(here), HEX);
      here++;
    }
  }
  Serial.println("Exiting Programming Mode");
  end_pmode();
  Serial.println("FINISHED");
}

void writeFusesHV(byte _l, byte _h){
  Serial.print("Writing hfuse = ");
  Serial.println(_h, BIN);
  writeHV(0x40, 0x4C);
  writeHV(_h, 0x2C);
  writeHV(0x00, 0x74);
  writeHV(0x00, 0x7C);
  //Write lfuse
  Serial.print("Writing lfuse = ");
  Serial.println(_l, BIN);
  writeHV(0x40, 0x4C);
  writeHV(_l, 0x2C);
  writeHV(0x00, 0x64);
  writeHV(0x00, 0x6C);
  Serial.println(); 
}

void readFusesHV(){
  writeHV(0x04, 0x4C);
  writeHV(0x00, 0x7A);
  byte inData = writeHV(0x00, 0x7E);
  Serial.print(F("hfuse = "));
  Serial.println(inData, BIN);
  
  writeHV(0x04, 0x4C);
  writeHV(0x00, 0x68);
  inData = writeHV(0x00, 0x6C);
  Serial.print(F("lfuse = "));
  Serial.println(inData, BIN);
  
  //Read efuse
  writeHV(0x04, 0x4C);
  writeHV(0x00, 0x6A);
  inData = writeHV(0x00, 0x6E);
  Serial.print(F("efuse = "));
  Serial.println(inData, BIN);
  Serial.println(); 
}
