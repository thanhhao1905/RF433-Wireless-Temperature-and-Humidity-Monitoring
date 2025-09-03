#include <LiquidCrystal_I2C.h>
 
// ------------------ LCD I2C ------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);
 
// ------------------ RF433 RX ------------------
int rxPin = 11; // DATA pin connected to RF433 RX module
 
// ---- CRC8 checksum calculation ----
uint8_t crc8(const char *data, size_t len) {
  uint8_t crc = 0x00;
  while (len--) {
    uint8_t extract = *data++;
    for (uint8_t i = 8; i; i--) {
      uint8_t sum = (crc ^ extract) & 0x01;
      crc >>= 1;
      if (sum) crc ^= 0x8C;
      extract >>= 1;
    }
  }
  return crc;
}
 
// ---- Function to read 1 bit ----
bool readBit() {
  int val = digitalRead(rxPin);
  delayMicroseconds(500); // synchronize speed with TX
  return val;
}
 
// ---- Function to read 1 byte ----
byte readByte() {
  // wait for start bit (0)
  while (digitalRead(rxPin) == HIGH);
  delayMicroseconds(500);
 
  byte data = 0;
  for (int i = 0; i < 8; i++) {
    if (readBit()) bitWrite(data, i, 1);
    else bitWrite(data, i, 0);
  }
 
  // read stop bit
  readBit();
  return data;
}
 
// ---- Function to receive a frame ----
bool receiveFrame(char *outPayload, int maxLen) {
  // find preamble 0xAA
  byte preamble;
  do {
    preamble = readByte();
  } while (preamble != 0xAA);
 
  // read payload string until checksum is found
  int i = 0;
  while (i < maxLen - 1) {
    byte b = readByte();
 
    // the checksum can be the last byte
    if (b < 32 || b > 126) {
      uint8_t crcVal = b;
 
      outPayload[i] = '\0';
      uint8_t calcCRC = crc8(outPayload, strlen(outPayload));
 
      return (crcVal == calcCRC);
    }
 
    outPayload[i++] = (char)b;
  }
  outPayload[i] = '\0';
  return false;
}
 
void setup() {
  pinMode(rxPin, INPUT);
  Serial.begin(9600);
 
  lcd.init();
  lcd.backlight();
 
  Serial.println("RF Receiver Ready!");
  lcd.setCursor(0,0);
  lcd.print("Receiver Ready!");
}
 
void loop() {
  char payload[32];
  if (receiveFrame(payload, sizeof(payload))) {
    Serial.print("Recv: ");
    Serial.println(payload);
 
    int tIndex = String(payload).indexOf("T=");
    int hIndex = String(payload).indexOf("H=");
 
    if (tIndex >= 0 && hIndex > 0) {
      float t = String(payload).substring(tIndex+2, hIndex-1).toFloat();
      float h = String(payload).substring(hIndex+2).toFloat();
 
      Serial.print(" Temperature: ");
      Serial.print(t,1);
      Serial.print(" *C, Humidity: ");
      Serial.print(h,1);
      Serial.println(" %");
 
      // Display on LCD
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Temperature:");
      lcd.print(t,1); lcd.print((char)223); lcd.print("C");
 
      lcd.setCursor(0,1);
      lcd.print("Humidity   :");
      lcd.print(h,1); lcd.print("%");
    }
  }
 
  // fast loop, no long delay
  delay(10);
}
