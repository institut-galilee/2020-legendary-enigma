#include <eeprom.h>

#define EEPROM_SIZE 256

char* DEVICE_ID = "FA_LE_0001";
char* warningNumber = "33446740004";
char* WIFI_SSID = "01234567890123456789012345678912";
char* WIFI_PASSWORD = "01234567890123456789012345678912";

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
  EEPROM.begin(EEPROM_SIZE);
  Serial.begin(9600);
  Serial.println(settingData.deviceId);
  Serial.println(settingData.warningNumber);
  Serial.println(settingData.wifiName);
  Serial.println(settingData.wifiPass);
  EEPROM.put(0, settingData);

  SettingStruct wroteData;
  EEPROM.get(0, wroteData);
  Serial.println("Data from ")
  Serial.println(wroteData.deviceId);
  Serial.println(wroteData.warningNumber);
  Serial.println(wroteData.wifiName);
  Serial.println(wroteData.wifiPass);
}

void loop() {

}

/*structure of EEPROM
[0]         length of id, normally 10 (d)
[1 -> 11]    device's id
[12]        length of warning number (t)
[13 -> 13+t] warning number, ex (33123456789)
[14+t]      length of wifi name (n)
[15+t -> 15+t+n] wifi name
[16+t+n]    length of wifi pass (p)
[17+t+n -> 17+t+n+p] wifi pass    
*/
// void saveSetting(char* deviceId, char* warningNumber, char* wifiName, char* wifiPass) {
//   int d = strlen(deviceId);
  
//   int t = strlen(warningNumber);
//   int n = strlen(wifiName);
//   int p = strlen(wifiPass);


// }