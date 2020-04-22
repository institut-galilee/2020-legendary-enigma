#include <SoftwareSerial.h>

#define LED_WARN_PIN 11
#define BUZZER_PIN 10
#define SMOKE_PIN A0
#define GAS_PIN 8
#define TEMP_PIN A2
#define SIM_PINNUMBER "0832"

#define TEMP_RESISTOR 10000
#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 25
#define BCOEFFICIENT 3950
#define SERIESRESISTOR 320 //320

// sensor variable
int tempValue;
int gasValue;
int smokeValue;
bool isWarning = false;
bool isTesting = false;
char* testingData;
int smokeThreshold = 400;
float ther_resistor;


//Create software serial object to communicate with SIM800L
SoftwareSerial smsSerial(3, 2); //SIM800L Tx & Rx is connected to Arduino #3 & #2
boolean notConnected = true;
char* warningNumber;
char* receiveNumber;
char* agencyNumber;
char* txtMsg = "FireAlarm's WARNING !!! Smoke is detected in your house now !!!";



int convertTempC(float valuePinA2)
{
  valuePinA2 = 1023 / valuePinA2 - 1;
  valuePinA2 = SERIESRESISTOR / valuePinA2;
  valuePinA2 = valuePinA2 / THERMISTORNOMINAL;
  valuePinA2 = log(valuePinA2);
  valuePinA2 /= BCOEFFICIENT;
  valuePinA2 += 1.0 / (TEMPERATURENOMINAL + 273.15);
  valuePinA2 = 1.0 / valuePinA2;
  valuePinA2 -= 273.15;
  return valuePinA2;
}

void setup()
{
  // Define the LED pin as Output
  Serial.begin(19200);
  setupPin();
  setupSIM800L();
  setupMQ2();
}

void setupPin()
{
  pinMode(LED_WARN_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(SMOKE_PIN, INPUT);
  pinMode(GAS_PIN, INPUT);
  pinMode(TEMP_PIN, INPUT);
}

//setup MQ2 sensor and component
void setupMQ2()
{
  // MQ2 warming up
  delay(20000);
}

void setupSIM800L() {
  smsSerial.begin(19200);
  smsSerial.println("AT");
}

void loop()
{
  sendSensorData();
  delay(5000);
}

void sendSensorData()
{
  smokeValue = analogRead(SMOKE_PIN);
  gasValue = digitalRead(GAS_PIN);
  ther_resistor = analogRead(TEMP_PIN);
  tempValue = convertTempC(ther_resistor);
  if (smokeValue > smokeThreshold || isTesting)
  { //|| gasValue == HIGH) {
    isWarning = true;
    if (isTesting) {
      smokeValue = smokeThreshold;
    }
    // sendSMS();
    for (byte w = 0; w < 5; w++)
    {
      // led & buzzer warning
      digitalWrite(LED_WARN_PIN, HIGH);
      tone(BUZZER_PIN, 1000, 700);
      delay(700);
      digitalWrite(LED_WARN_PIN, LOW);
      delay(300);
    }
    if (isTesting) {
      isTesting = false;
    }
  }
  else
  {
    isWarning = false;
  }
  Serial.println(tempValue);
  Serial.println(smokeValue);
  if (gasValue == 0) {
    //avoid zero value in communication
    Serial.println(2);
  } else {
    //gas detected !!! gasValue == 1 == HIGH
    Serial.println(gasValue);
  }
  
  if (isWarning) {
    Serial.println(1);
  } else {
    Serial.println(2);
  }
}

void receiveData() {
  String msgRecv = "";
  char chRecv;
  byte count = 0;
  while (Serial.available() > 0)
  {
    chRecv = Serial.read();
    if (chRecv == '=') {
      count++;
      // the end of message
      if (count == 2) {
        break;
      }
    } else {
      if (count == 1) {
        msgRecv.concat('=');
      }
      msgRecv.concat(chRecv);
      
      count = 0;
    }
  }

  if (msgRecv != "") {
    String data = msgRecv.substring(4);
    if (msgRecv.startsWith("103")) {
      //setting warningNumber;
      warningNumber = string2char(data);
    } else if(msgRecv.startsWith("104")) {
      //setting agencyNumber;
      agencyNumber = string2char(data);
    } else if(msgRecv.startsWith("105")) {
      //data 1111 -> 2222 with 1==true and 2==false in the order (smoke, gas, sms, notification)
      isTesting = true;
      testingData = string2char(data);
    } else if(msgRecv.startsWith("106")) {
      isTesting = false;
    }
  }
}

void sendSMS() {
  smsSerial.println("AT+CMGF=1"); // Configuring TEXT mode
  // smsSerial.print("AT+CMGS=\"+ZZxxxxxxxxxx\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  smsSerial.print("AT+CMGS=\"+33");
  smsSerial.print(warningNumber);
  smsSerial.print("\"");
  smsSerial.print(txtMsg); //text content
  smsSerial.write(26); //like CTRL + Z
}

char* string2char(String command){
    if(command.length()!=0){
        char *p = const_cast<char*>(command.c_str());
        return p;
    }
}
