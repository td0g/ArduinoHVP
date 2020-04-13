//Functions for handling EEPROM writing / reading

#define EECHUNK (32)
uint8_t write_eeprom(unsigned int length) {
  // here is a word address, get the byte address
  unsigned int start = here * 2;
  unsigned int remaining = length;
  if (length > param.eepromsize) {
    error++;
    return STK_FAILED;
  }
  while (remaining > EECHUNK) {
    write_eeprom_chunk(start, EECHUNK);
    start += EECHUNK;
    remaining -= EECHUNK;
  }
  write_eeprom_chunk(start, remaining);
  return STK_OK;
}
// write (length) bytes, (start) is a byte address
uint8_t write_eeprom_chunk(unsigned int start, unsigned int length) {
  // this writes byte-by-byte, page writing may be faster (4 bytes at a time)
  fillBuff(length);
  for (unsigned int x = 0; x < length; x++) {
    unsigned int addr = start + x;
    //spi_transaction(0xC0, (addr >> 8) & 0xFF, addr & 0xFF, buff[x]);
    delay(45);
  }
  return STK_OK;
}

char eeprom_read_page(int length) {
  // here again we have a word address
  int start = here * 2;
  for (int x = 0; x < length; x++) {
    int addr = start + x;
    uint8_t ee = 0; //VTG fix
    //uint8_t ee = spi_transaction(0xA0, (addr >> 8) & 0xFF, addr & 0xFF, 0xFF);
    SERIAL.print((char) ee);
  }
  return STK_OK;
}
