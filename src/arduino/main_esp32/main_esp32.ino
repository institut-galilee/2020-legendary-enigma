#include "secret.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <BluetoothSerial.h>
#include <EEPROM.h>

#define AWS_IOT_PUBLISH_TOPIC "FireAlarm/sensor_data"
#define AWS_IOT_PUBLISH_WARNING_TOPIC "FireAlarm/warning"
#define AWS_IOT_SUBSCRIBE_TOPIC "FireAlarm/settings/FA_LE_0001"
#define EEPROM_SIZE 256
char* WIFI_SSID = "freebox_VITRY";          //freebox_VITRY
char* WIFI_PASSWORD = "DD2BF3B4EE"; //DD2BF3B4EE
HTTPClient http;
WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);
BluetoothSerial SerialBT;
char* DEVICE_ID = "FA_LE_0001";
char* warningNumber;

bool listenBT = true; //true = SerialBT.begin("FireAlarm_LE")
bool wifiCntd = false;
bool wifiBegan = false;
bool awsCntd = false;
bool updatedSetting = false;

struct SettingStruct {
  char* deviceId;
  char* warningNumber;
  char* wifiName;
  char* wifiPass;
};

SettingStruct settingData = {
  DEVICE_ID,
  warningNumber,
  WIFI_SSID,
  WIFI_PASSWORD
};

void setup() {
  Serial.begin(19200);
  EEPROM.begin(EEPROM_SIZE);
  checkWiFi();
  checkAWS();
  Serial2.begin(19200, SERIAL_8N1, 16, 17);
  SerialBT.begin("FireAlarm_LE");
  checkBluetooth();
  updateSetting();
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
  delay(10000);
}
void checkBluetooth() {
  if (listenBT)
  {
    String msgRecvBT = "";
    char chRecv;
    byte count = 0;
    while (SerialBT.available() > 0)
    {
      Serial.println(F("reading Bluetooth data"));
      chRecv = SerialBT.read();
      if (chRecv == '=') {
        count++;
        // the end of message
        if (count == 2) {
          break;
        }
      } else {
        msgRecvBT.concat(chRecv);
        count = 0;
      }
    }
    //
    if (msgRecvBT != "") {
      String data = msgRecvBT.substring(4);
      if (msgRecvBT.startsWith("100")) {
        //start communication
        //send setting data to mobile;
        SerialBT.println(WIFI_SSID);
        SerialBT.println(WIFI_PASSWORD);
        SerialBT.println(warningNumber);
      } else if (msgRecvBT.startsWith("101")) {
        WIFI_SSID = string2char(data);
        Serial.println("new wifi ssid");
        Serial.println(WIFI_SSID);
        wifiCntd = false;
        wifiBegan = false;
        updatedSetting = true;
        //second time to read wifi's password
        checkBluetooth();
      } else if (msgRecvBT.startsWith("102")) {
        WIFI_PASSWORD = string2char(data);
        Serial.println("new wifi pass");
        Serial.println(WIFI_PASSWORD);
        wifiCntd = false;
        wifiBegan = false;
        updatedSetting = true;
      } else if (msgRecvBT.startsWith("103") || msgRecvBT.startsWith("104")) {
        warningNumber = string2char(data);
        //Setting
        updatedSetting = true;
        Serial2.print(msgRecvBT);
      } else if (msgRecvBT.startsWith("105")) {
        //Testing
        char* testingData = string2char(data);
        postWarningMessage(400);
        Serial2.print(msgRecvBT);
      } else {
        Serial.println(F("strange connection BT"));
        Serial.println(msgRecvBT.c_str());
      }
    }
  }
}

void updateSetting() {
  if (updatedSetting) {
     /* 
      *  struct SettingStruct {
        char* deviceId;
        char* warningNumber;
        char* wifiName;
        char* wifiPass;
      };
     */
    //save data to EEPROM
    settingData = {
      DEVICE_ID,
      warningNumber,
      WIFI_SSID,
      WIFI_PASSWORD
    };
    EEPROM.put(0, settingData);
    updatedSetting = false;
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
    unsigned int isWarning = Serial2.parseInt();
    if (isWarning <= 0) {
      isWarning = Serial2.parseInt();
      if (isWarning == 1) {
        postWarningMessage(gas);  
      }
    }
    Serial.println("---------------");
    Serial.println(temp);
    Serial.println(gas);
    Serial.println(smoke);
    Serial.println("---------------");
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

//http
void postWarningMessage(int warningValue) {
  if (WiFi.status() == WL_CONNECTED) {

    http.begin("http://54.172.128.55/send_sms");
    http.addHeader("Content-Type", "application/json");
    DynamicJsonDocument doc(200);
    //device's id, warning value and warning number to send sms.
    doc["deviceId"] = DEVICE_ID;
    doc["warningValue"] = warningValue;
    doc["warningNumber"] = warningNumber;
    
    String postBody;
    serializeJson(doc, postBody);

    int httpCode = http.POST(postBody);
    if (httpCode == 200) {
      //TODO:
      //send SMS successfully
    } else if (httpCode == 400) {
      // error in postBody
    } else {
      // error in postRequest
    }


  }
}

/* 
structure of EEPROM
[0]         length of id, normally 10 (d)
[1 -> 11]    device's id
[12]        length of warning number (t)
[13 -> 13+t] warning number, ex (33123456789)
[14+t]      length of wifi name (n)
[15+t -> 15+t+n] wifi name
[16+t+n]    length of wifi pass (p)
[17+t+n -> 17+t+n+p] wifi pass    
*/



void saveSetting(char* deviceId, char* warningNumber, char* wifiName, char* wifiPass) {
  int d = strlen(deviceId);
  
  int t = strlen(warningNumber);
  int n = strlen(wifiName);
  int p = strlen(wifiPass);


}


void messageHandler(String &topic, String &payload) {
  // Serial.println("incoming: " + topic + " - " + payload);
  //  StaticJsonDocument<200> doc;
  //  deserializeJson(doc, payload);
  //  const char* message = doc["message"];
}


char* string2char(String command){
    if(command.length()!=0){
        char *p = const_cast<char*>(command.c_str());
        return p;
    }
}
