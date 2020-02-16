int buzzerPin = 8;
void setup() {
  //baudrate = 9600
  Serial.begin(9600);
  //pinMode(buzzerPin, OUTPUT);
}

void loop() {
  while (Serial.available() > 0) {
    String frequency = Serial.readString();
    Serial.println(frequency.toInt());
    //tone(pin, frequency, duration)
    tone(buzzerPin, frequency.toInt(), 1000);


    //noTone(pin)
    //If you want to play different pitches on multiple pins, 
    //you need to call noTone() on one pin 
    //before calling tone() on the next pin.

    //delay(300);
    //method delay is independent with tone.
    
  }

}
