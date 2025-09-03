#include <DHT.h>
#include <LiquidCrystal_I2C.h>

// ------------------ DHT11 ------------------
#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ------------------ LCD I2C ------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ------------------ RF433 TX ------------------
int txPin = 12; // DATA pin connected to RF433 TX module

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

// ---- Function to send a bit ----
void sendBit(bool bitVal) {
  digitalWrite(txPin, bitVal ? HIGH : LOW);
  delayMicroseconds(500); // speed ~2000bps
}

// ---- Function to send 1 byte (with start/stop bit) ----
void sendByte(byte data) {
  // start bit
  sendBit(0);
  for (int i = 0; i < 8; i++) {
    sendBit(bitRead(data, i));
  }
  // stop bit
  sendBit(1);
}

// ---- Send the whole frame: [preamble][data][checksum] ----
void sendFrame(const char *payload) {
  byte preamble = 0xAA;
  uint8_t crcVal = crc8(payload, strlen(payload));

  // send preamble
  sendByte(preamble);

  // send each character in the payload
  for (int i = 0; i < strlen(payload); i++) {
    sendByte((byte)payload[i]);
  }

  // send checksum
  sendByte(crcVal);
}

void setup() {
  pinMode(txPin, OUTPUT);
  Serial.begin(9600);
  dht.begin();
  delay(2000);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("RF TX Ready!");
  delay(2000);
  lcd.clear();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Error reading DHT11!");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("DHT Error");
    delay(2000);
    return;
  }

  // Convert float to string
  char tStr[8], hStr[8];
  dtostrf(t, 4, 1, tStr);
  dtostrf(h, 4, 1, hStr);

  // Payload: T=xx.x;H=yy.y
  char payload[20];
  snprintf(payload, sizeof(payload), "T=%s;H=%s", tStr, hStr);

  // Send frame
  sendFrame(payload);

  // Print to Serial
  Serial.print("Send: ");
  Serial.println(payload);

  // Display on LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  lcd.print(t,1);
  lcd.print((char)223);
  lcd.print("C");

  lcd.setCursor(0,1);
  lcd.print("Humid: ");
  lcd.print(h,1);
  lcd.print("%");

  delay(1000);
}
