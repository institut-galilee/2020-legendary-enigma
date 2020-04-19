#include <GSM.h>

// GSM initial
GSM gsmAccess;
GSM_SMS sms;
boolean gsmConnected = false;
char* warningNumber = "+33636128318";
char* receiveNumber;
char* txtDefaultMsg = "FireAlarm's WARNING !!!";
char* txtMsg = "Smoke detected in your house now";
const SIM_PINNUMBER = "0832"

void setup() {
  setupGSM();
}


void loop() {

}

//set up and connect GSM
//put this function to setup()
void setupGSM() {
  while (gsmConnected) {
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
    sms.beginSMS(&warningNumber);
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