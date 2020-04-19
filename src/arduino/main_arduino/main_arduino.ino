#include <Wire.h>
#include <GSM.h>

#define LED_BUILTIN 13
#define LED_WARN_PIN 11
#define BUZZER_PIN 10
#define SMOKE_PIN A0
#define GAS_PIN 9
#define SIM_PINNUMBER "0832"

// Include the required Wire library for I2C<br>#include <Wire.h>
float tempValue;
int gasValue;
int smokeValue;
bool isWarning = false;
int gasThreshold = 400;
int smokeThreshold = 400;

// GSM initial
GSM gsmAccess;
GSM_SMS sms;
boolean gsmConnected = false;

//load from first login
char* warningNumber;
char* receiveNumber;
char* txtDefaultMsg = "FireAlarm's WARNING !!!";
char* txtMsg = "Smoke detected in your house now";
void setup() {
  // Define the LED pin as Output
  Serial.begin(9600);
  setupMQ2();
  //setupGSM();
}

void loop() {
  //blink led to warning smoke or gas
  if (isWarning == true) {
    // led & buzzer warning
    digitalWrite(LED_WARN_PIN, HIGH);
    tone(BUZZER_PIN, 1000, 500);
    delay(300);
    digitalWrite(LED_WARN_PIN, LOW);
    delay(300);

    //send SMS
    //sendSmsWarning();
    
    //

  }
  delay(5000);
  //receiveSmsEvent();
}

//setup MQ2 sensor and component
//put this function in setup()
void setupMQ2() {
  // MQ2 warming up
  delay(20000);
  pinMode(LED_WARN_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(SMOKE_PIN, INPUT);
  pinMode(GAS_PIN, INPUT);

  // Start the I2C Bus as Slave on address 2
//  Wire.begin(2);
//  Serial.println(" Registre address 2 - OK");
//
//  // Attach a function to trigger when something is received.
//  Wire.onReceive(receiveDataEvent);
}

//set up and connect GSM
//put this function to setup()
void setupGSM() {
  while (gsmConnected == false) {
    if (gsmAccess.begin(SIM_PINNUMBER) == GSM_READY) {
      gsmConnected = true;
    } else {
      Serial.println("Connecting GSM");
      delay(1000);
    }
  }
  Serial.println("GSM initialized");
}

//send Sms Warning to remoteNumber
//return true if completely send
//else return false and warning to Server
boolean sendSmsWarning() {
  if (notConnected == false) {
    //FIX MEsms.beginSMS(&warningNumber);
    //write message's content
    sms.print(txtDefaultMsg);
    sms.print(txtMsg);
    sms.endSMS();
    return true;
    Serial.println("GSM: SEND SMS COMPLETELY");
  } else {
    Serial.println("GSM: GSM is not connected");
    return false;
  }
}

//FIXME:
//put this function to loop()
boolean receiveSmsEvent() {
  if (sms.available()) {
    char c;
    Serial.println("Message received from: ");
    sms.remoteNumber(receiveNumber, 20);
    Serial.println(receiveNumber);

    while (c = sms.read()) {
      //FIXME: get message's content
      Serial.print(c);
    }
    Serial.println("\n End of messages");

    //TODO: use message's content to setting this device
    //setting
    
    //FIXME: checking if need to delete this message
    sms.flush();
    Serial.println("Message deleted");
  }
}

//
void receiveDataEvent(int manyBytes) {
  if (manyBytes) {
    //read temp
    //tempValue = "15.5";
    //read gas
    int gasValue = digitalRead(GAS_PIN);
    //read smoke
    int smokeValue = analogRead(SMOKE_PIN);
//    Wire.write("15.5");
//    Wire.write("200");
//    Wire.write("200");
    Serial.println(tempValue + " " + gasValue + " " + smokeValue);
    if (gasValue > gasThreshold || smokeValue > smokeThreshold) {
      isWarning = true;
      Serial.println("warning warning warning");
    }
  }
}
