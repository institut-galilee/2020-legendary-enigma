# IoT Project 2020 - Legendary Enigma

## 1. Description de projet
Notre projet est de développer une cloche d'alarme incendie. La cloche alerte l'utilisateur sur la presence de gaz, de fumée dans la maison.
Après, nous avons développerons les fonctionalités d'un maison intélligent.

## 2. Function
- Détecter le gaz et la fumée

- Mesurer la température, l'humidité, la pression

- Envoyer un avertissement au client lorsque du gaz est détecté, de la fumée détectée par application mobile.
	+ Si Wifi n'est pas disponible, device envoiera un SMS au client
	
- Le client peut voir les données (température, humidité, pression) sur l'application mobile en temps réel et en un moi, un week.

- Le client peut configurer des paramètre de device (wifi & password, temps de collecter, connection de bluetooth, ...)

- Le client peut peut ouvrir la porte de maison et de garage

- Server est de stocker les données et envoyer les informations au client.

- Frontend est l'application mobile.

Usecase Diagram:
![Usecase Diagram](doc/image/usecase diagram.png)

## 3.Description des fonctionalités:


## 4. Component
- Arduino UNO R3 	: Micro Controller

- MQ2 sensor 		: Gaz & Smoke sensor
![MQ2 Sensor](doc/image/MQ-2 sensor.png)

- ESP32 Dev Kit 	: Wifi and Bluetooth device
![](doc/image/.png)

- BME280 sensor		: Humidity, temperature
![BME280](doc/image/.png)

- MFRC522 			: RFID Reader
![MFRC522 _ RFID](doc/image/MFRC522 _ RFID.png)

- SIM800L 			: GSM Module
![800L GSM Module](doc/image/800L GSM Module.png)

- Expansion Board	: Extension

- Led, Buzzer 		: Warning

- Wired


## 5. Schema
![Schematic](doc/image/schema.png)

## 6. References:

[ESP32 DevKit Wifi]
(https://techtutorialsx.com/2017/06/29/esp32-arduino-getting-started-with-wifi/)

[ESP32 DevKit Bluetooth]
(https://randomnerdtutorials.com/esp32-bluetooth-classic-arduino-ide/)

[BME280 Tutorial]
(https://randomnerdtutorials.com/bme280-sensor-arduino-pressure-temperature-humidity/

[MQTT and Node.js Server]
(https://www.digikey.com/en/maker/blogs/2018/how-to-use-basic-mqtt-on-arduino)
(https://blog.risingstack.com/getting-started-with-nodejs-and-mqtt/)

[MQ2 and OLED Tutorial]
(https://www.electronics-lab.com/project/arduino-breathalyzer-using-mq3-gas-sensor-oled-display/)

[SIM800L GSM Module Tutorial]
(https://www.youtube.com/watch?v=wqraxBVwF70)
(https://lastminuteengineers.com/sim800l-gsm-module-arduino-tutorial/)

[MFRC522 RFID Tutorial]
(https://randomnerdtutorials.com/security-access-using-mfrc522-rfid-reader-with-arduino/)
(https://www.youtube.com/watch?v=KQiVLEhzzV0)
(https://forum.mysensors.org/topic/2439/rfid-garage-door-opener)
