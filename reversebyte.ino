byte b1 = byte(27);
byte b2 = byte(130);

byte Bit_Reverse(byte Monbyte) {
  byte  rtn = 0;
  for (byte i=0; i<8; i++) {
    bitWrite(rtn, 7-i, bitRead(Monbyte, i));
  }
  return rtn;
}

void setup() {
  
  Serial.begin(9600);
  b1 = Bit_Reverse(b1);
  b2 = b2 & byte(192);
  b2 = Bit_Reverse(b2);
  int b = b2*256;
  Serial.println(b1 + b);

}

void loop() {
  // put your main code here, to run repeatedly:

}
