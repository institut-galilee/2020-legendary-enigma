
//wifi, aws cert, key
#include "secret.h"
//MQTT Communication
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
//I2C Communication
#include <Wire.h>
//Bluetooth Communication
#include <BluetoothSerial.h>

//Data Warning
int gasThreshold = 400;
int smokeThreshold = 400;

#define AWS_IOT_PUBLISH_TOPIC   "FireAlarm/sensor_data"
#define AWS_IOT_SUBSCRIBE_TOPIC  "FireAlarm/settings"

//WIFI and MQTT
const char* WIFI_SSID; //freebox_VITRY
const char* WIFI_PASSWORD = ""; //DD2BF3B4EE
WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);
boolean wifiConnected = false;
boolean wifiBegin = false;

//Bluetooth for first setup
BluetoothSerial SerialBT;
String msgRecv = "";
char chRecv;
boolean listeningBT = false; //update when click button

void setup() {
  Serial.begin(9600);
  SerialBT.begin("FireAlarm_LE");
  delay(5000);
  //setupAWS();
}

void loop() {
  //requestDataAndPublish();
  //client.loop();
  //delay(10000);
  Serial.println("a loop");
  checkBluetooth();

  //
  checkWiFi();

  delay(5000);
}



//check bluetooth listening
void checkBluetooth() {
  if (listeningBT) {
    msgRecv = "";
    //read message of BT
    while (SerialBT.available()) {
      chRecv = SerialBT.read();
      msgRecv.concat(chRecv);
    }

    //Have a msg of BT
    if (msgRecv != "") {
      //String code = msgRecv.substring(0, 3);
      String data = msgRecv.substring(4);
      //set WIFI_SSID
      if (msgRecv.startsWith("101")) {
        WIFI_SSID = data.c_str();
        Serial.println(WIFI_SSID);
        wifiConnected = false;
        wifiBegin = false;
      }
      //set WIFI_PASSWORD
      if (msgRecv.startsWith("102")) {
        WIFI_PASSWORD = data.c_str();
        Serial.print("new wifi password: ");
        Serial.println(WIFI_PASSWORD);
        wifiConnected = false;
        wifiBegin = false;
      }
      //setting telephone's number (receivers)
      if (msgRecv.startsWith("103")) {
        //TODO:
      } else {
        Serial.println(msgRecv.c_str());
      }
      //TODO: code == "104" - language
    }
  }
}

//check wifi connection - delay 5000 if connecting WiFi
void checkWiFi() {
  if (wifiBegin == false && WIFI_SSID != "" && WIFI_PASSWORD != "") {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    wifiBegin = true;
    delay(5000);
  }

  if (wifiBegin && WiFi.status() != WL_CONNECTED ) {
    Serial.println("Connecting to Wi-Fi");
    if (WiFi.status() == WL_CONNECTED) {
      wifiConnected = true;
      Serial.println("WiFi connected");
    }
  }

}

//check aws connection
void setupAWS()
{
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Create a message handler
  client.onMessage(messageHandler);

  Serial.print("Connecting to AWS IOT");
  Serial.println(THINGNAME);
  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(3000);
  }
  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }
  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  Serial.println("AWS IoT Connected!");
}

void publishMessage(float temp, int gas, int smoke)
{
  StaticJsonDocument<200> doc;
  doc["time"] = millis() / 1000;
  doc["temp"] = temp;
  doc["gas"] = gas;
  doc["smoke"] = smoke;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void requestDataAndPublish() {
  //requestFrom(i,n) request data from slave's address i, n byte : int = 2-byte
  //ESP32 (Master) request data from Arduino (Slave - address 2) 8 byte: float (4) temp, int (2) gas, int (2) smoke
  Wire.requestFrom(2, 8);
  int tempValue;
  int gasValue;
  int smokeValue;
  while (Wire.available()) {
    tempValue = Wire.read();
    gasValue = Wire.read();
    smokeValue = Wire.read();
    if (gasValue > gasThreshold || smokeValue > smokeThreshold) {
      //WIFI: notification to user's mobile

      //Bluetooth: notification to user's mobile


      Serial.println(" !!! Fire Alarm Warning !!!");
    }
  }
  publishMessage(tempValue, gasValue, smokeValue);
}

void messageHandler(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  //  StaticJsonDocument<200> doc;
  //  deserializeJson(doc, payload);
  //  const char* message = doc["message"];
}
