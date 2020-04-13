//Basic HV Serial Communication with ATTINY

byte writeHV(byte sdiByte, byte siiByte){
  byte in = 0;
  while (!digitalRead(SDO)){};
  digitalWrite(SDI, 0);
  digitalWrite(SII, 0);
  in = pulseClock();
  for (byte i = 0; i < 8; i++){
    digitalWrite(SDI, ((sdiByte >> (7-i)) & 1));
    digitalWrite(SII, ((siiByte >> (7-i)) & 1));
    if (i < 7){
      in = in << 1;
      in |= pulseClock();
    }
    else pulseClock();
  }
  digitalWrite(SDI, 0);
  digitalWrite(SII, 0);
  pulseClock();
  pulseClock();
  return in;
}

bool pulseClock(){
  delayMicroseconds(8); //This only needs to be <1 microsecond, but let's just play things safe :)
  digitalWrite(SCL, 1);
  delayMicroseconds(8); //Same...
  byte in = digitalRead(SDO); //Reading back from target
  digitalWrite(SCL, 0);
  return in;
}
