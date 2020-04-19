#include "secret.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include <BluetoothSerial.h>

#define AWS_IOT_PUBLISH_TOPIC "FireAlarm/sensor_data"
#define AWS_IOT_SUBSCRIBE_TOPIC "FireAlarm/settings/FA_LE_0001"
char* WIFI_SSID = "freebox_VITRY";          //freebox_VITRY
char* WIFI_PASSWORD = "DD2BF3B4EE"; //DD2BF3B4EE
WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);
BluetoothSerial SerialBT;
char* DEVICE_ID = "FA_LE_0001";

bool listenBT = true; //true = SerialBT.begin("FireAlarm_LE")
bool wifiCntd = false;
bool wifiBegan = false;
bool awsCntd = false;

void setup() {
  Serial.begin(9600);
  checkWiFi();
  checkAWS();
  Serial2.begin(19200, SERIAL_8N1, 16, 17);
  SerialBT.begin("FireAlarm_LE");
  checkBluetooth();
  delay(5000);
}
byte k = 0;
void loop() {
  Serial.println(F("Start a loop"));
  if (k == 0) {
    Serial.println(F("check BT"));
    checkBluetooth();
    Serial.println(F("check wifi"));
    checkWiFi();
    Serial.println(F("check aws"));
    checkAWS();
    k = 5;
  }
  k--;
  Serial.println(F("get sensor data to publish"));
  publishSensorData();
  client.loop();
  delay(5000);
}
void checkBluetooth(){
  
  if (listenBT)
  {
    String msgRecvBT = "";
    char chRecv;
    while (SerialBT.available() > 0)
    {
      Serial.println(F("reading Bluetooth data"));
      chRecv = SerialBT.read();
      msgRecvBT.concat(chRecv);
    }
    if (msgRecvBT != "") {
      String data = msgRecvBT.substring(4);
      if (msgRecvBT.startsWith("101")) {
        data.toCharArray(WIFI_SSID, data.length());
        wifiCntd = false;
        wifiBegan = false;
      }
      if (msgRecvBT.startsWith("102")) {
        data.toCharArray(WIFI_PASSWORD, data.length());
        wifiCntd = false;
        wifiBegan = false;
      }
      if (msgRecvBT.startsWith("103")) {
        //TODO:
      }
      else {
        Serial.println(F("strange connection BT"));
        Serial.println(msgRecvBT.c_str());
      }
    }
  }
}
void checkWiFi() {
  if (wifiBegan == false && WIFI_SSID != "" && WIFI_PASSWORD != "") {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    wifiBegan = true;
    Serial.println(F("Wifi Begin"));
    delay(5000);
  }
  if (wifiBegan && wifiCntd == false) {
    while (WiFi.status() != WL_CONNECTED) {
      Serial.println(F("connecting wifi..."));
      delay(2000);
    }

    if (WiFi.status() == WL_CONNECTED) {
      wifiCntd = true;
      Serial.println(F("WiFi cntd"));
    }
  }
}
void checkAWS() {
  if (WiFi.status() == WL_CONNECTED && wifiCntd && awsCntd == false) {
    net.setCACert(AWS_CERT_CA);
    net.setCertificate(AWS_CERT_CRT);
    net.setPrivateKey(AWS_CERT_PRIVATE);
    client.begin(AWS_IOT_ENDPOINT, 8883, net);
    client.onMessage(messageHandler);
    while (!client.connect(THINGNAME)) {
      Serial.print(F("."));
      delay(2000);
    }
    if (!client.connected()) {
       Serial.println(F("AWS time out"));
       awsCntd = false;
       return; 
    }
    awsCntd = true;
    client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
    Serial.println(F("aws cntd"));
    
  }
}

void publishSensorData() {
  while (Serial2.available() > 0) {
    unsigned int temp = Serial2.parseInt();
    if (temp <= 0) {
      temp = Serial2.parseInt();
    }
    unsigned int gas = Serial2.parseInt();
    if (gas <= 0) {
      gas = Serial2.parseInt();
    }
    unsigned int smoke = Serial2.parseInt();
    if (smoke <= 0) {
      smoke = Serial2.parseInt();
    }
    Serial.println(temp);
    Serial.println(gas);
    Serial.println(smoke);
    publishMessage(temp, gas, smoke);
    return;
  }
}
void publishMessage(unsigned int temp, unsigned int gas, unsigned int smoke) {
  StaticJsonDocument<200> doc;
  doc["device_id"] = DEVICE_ID;
  doc["temp"] = temp;
  doc["gas"] = gas;
  doc["smoke"] = smoke;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}
void messageHandler(String &topic, String &payload) {
  // Serial.println("incoming: " + topic + " - " + payload);
  //  StaticJsonDocument<200> doc;
  //  deserializeJson(doc, payload);
  //  const char* message = doc["message"];
}
