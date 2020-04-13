//Functions to enter / exit HV programming mode

void start_pmode() {
  if (!pmode){
    pinMode(SDO, OUTPUT);
    digitalWrite(VCC, 1);
    delayMicroseconds(20);
    digitalWrite(RST, 0);
    delayMicroseconds(10);
    pinMode(SDO, INPUT);
    delayMicroseconds(200);
    //HVP mode entered.  Now command Flash Writing
    pmode = 1;
  }
}


void end_pmode() {
  if (pmode){
    digitalWrite(RST, 1);
    digitalWrite(VCC, 0);
    pinMode(SDO, OUTPUT);
    pmode = 0;
  }
}
