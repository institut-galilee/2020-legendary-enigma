#include <HardwareSerial.h>

void setup() {
  Serial.begin(9600);
  // esp32_serial.begin(9600, SERIAL_8N1, 16, 17);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
}


void loop() {
  while (Serial2.available() > 0) {
    Serial.println("available");
    int temp = Serial2.parseInt();
    if (temp <= 0) {
      temp = Serial2.parseInt();
    }
    int gas = Serial2.parseInt();
    if (gas <= 0) {
      gas = Serial2.parseInt();
    }
    int smoke = Serial2.parseInt();
    if (smoke <= 0) {
      smoke = Serial2.parseInt();
    }
    Serial.println(temp);
    Serial.println(gas);
    Serial.println(smoke);
  }
  delay(1000);
}