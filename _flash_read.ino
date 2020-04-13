//Functions to read flash back to Serial

uint16_t flash_read(unsigned int addr) {
  unsigned int _addr = addr;
  
  //Read Flash Low and High Bytes
  writeHV(addr, 0b00001100);  
  addr = addr >> 8;
  //addr &= 0b00001111; //That blasted error again...
  writeHV(addr, 0b00011100);
  writeHV(0, 0b01101000);
  byte low = writeHV(0, 0b01101100);
  writeHV(0, 0b01111000);
  unsigned int high = writeHV(0, 0b01111100);
  //Return value
  high = high << 8;
  high |= low;
  return high;
}

char flash_read_page(int length) {
  writeHV(0b00000010, 0b01001100);  //Load "Read Flash" Command
  pmode = 3;
  for (int x = 0; x < length; x += 2) {
    uint16_t input = flash_read(here);
    byte inputByte = input;
    SERIAL.print((char) inputByte);
    input = input >> 8;
    SERIAL.print((char) input);
    here++;
  }
  return STK_OK;
}

void read_page() {
  char result = (char)STK_FAILED;
  int length = 256 * getSerialChar();
  length += getSerialChar();
  char memtype = getSerialChar();
  if (CRC_EOP != getSerialChar()) {
    error++;
    SERIAL.print((char) STK_NOSYNC);
    return;
  }
  SERIAL.print((char) STK_INSYNC);
  if (memtype == 'F') result = flash_read_page(length);
  if (memtype == 'E') result = eeprom_read_page(length);
  SERIAL.print(result);
}

void read_signature() {
  if (!pmode) start_pmode();
  if (CRC_EOP != getSerialChar()) {
    error++;
    SERIAL.print((char) STK_NOSYNC);
    return;
  }
  SERIAL.print((char) STK_INSYNC);
  writeHV(0b00001000,0b01001100);
  writeHV(0b00000000,0b00001100);
  writeHV(0b00000000,0b01101000);
  uint8_t high = writeHV(0b00000000,0b01101100);
  SERIAL.print((char) high);
  writeHV(0b00001000,0b01001100);
  writeHV(0b00000001,0b00001100);
  writeHV(0b00000000,0b01101000);
  uint8_t middle = writeHV(0b00000000,0b01101100);
  SERIAL.print((char) middle);
  writeHV(0b00001000,0b01001100);
  writeHV(0b00000010,0b00001100);
  writeHV(0b00000000,0b01101000);
  uint8_t low = writeHV(0b00000000,0b01101100);
  SERIAL.print((char) low);
  SERIAL.print((char) STK_OK);
}
