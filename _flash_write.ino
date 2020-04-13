//Functions to save Serial data to flash

void program_page() {
  char result = (char) STK_FAILED;
  unsigned int length = 256 * getSerialChar();
  length += getSerialChar();
  char memtype = getSerialChar();
  // flash memory @here, (length) bytes
  if (memtype == 'F') {
    write_flash(length);
    return;
  }
  if (memtype == 'E') {
    result = (char)write_eeprom(length);
    if (CRC_EOP == getSerialChar()) {
      SERIAL.print((char) STK_INSYNC);
      SERIAL.print(result);
    } else {
      error++;
      SERIAL.print((char) STK_NOSYNC);
    }
    return;
  }
  SERIAL.print((char)STK_FAILED);
  return;
}


void write_flash(int length) {
  fillBuff(length);
  if (CRC_EOP == getSerialChar()) {
    SERIAL.print((char) STK_INSYNC);
    SERIAL.print((char) write_flash_pages(length));
  } else {
    error++;
    SERIAL.print((char) STK_NOSYNC);
  }
}


uint8_t write_flash_pages(int length) {
  if (!pmode) start_pmode();
  int x = 0;
  writeHV(0b00010000,0b01001100); //Load “Write Flash” Command.  Seems to be called every buffer load cycle.
  while (x < length) {
    flash(here, buff[x+1], buff[x]);
    if (x > 0 && ((byte)here == 255)){   //Moving onto next page...  ATTINY85:   Page size = 32 words (64 bytes)    128 pages / 256 bytes
      uint16_t addr = here;
      addr = addr >> 8;
      writeHV(addr, 0b00011100); //pp. 159 error - FOUR high bits in address LOL
      writeHV(0, 0b01100100);
      writeHV(0, 01101100);
      writeHV(0b00010000,0b01001100); //Load “Write Flash” Command.  Seems to be called every buffer load cycle.
    }
    x += 2;
    here++;
  }

    //Load Flash High Address and Program Page
  uint16_t addr = here-1;
  addr = addr >> 8;
  writeHV(addr, 0b00011100); //pp. 159 error - FOUR high bits in address for ATTINY85 LOL
  writeHV(0, 0b01100100);
  writeHV(0, 01101100);
  writeHV(0, 0b01001100); //Load "No Operation" Command
  return STK_OK;
}


void flash(unsigned int addr, uint8_t high, uint8_t low) {  //Load Flash Page Buffer
  writeHV((byte)addr, 0b00001100);
  writeHV(0, 0b00101100);
  writeHV(low, 0b00101100);
  writeHV(high, 0b00111100);
  writeHV(0, 0b01111101);
  writeHV(0, 0b01111100);
}
