/*Downlink from Lora IoT by CAT is 012020012412105920200124143000 (00yyyyMMddhhmmssyyyyMMddhhmmss)
       Device : RelayA => 01, RelayB => 02
       Start : 2020-01-24 12:10:59 => 20200124121059
       Stop : 2020-01-24 14:30:00 => 20200124143000
*/

#include <TimeLib.h>
#include <Wire.h>
#include <DS3231.h>

#include "CatLoRaS76S.h"
#include "LedModule.h"
#include "DateTimeLib.h"
#include "RelayModule.h"

#define RED_LED_PIN 13
#define GREEN_LED_PIN 12
#define RELAY_A_PIN 5
#define RELAY_B_PIN 6

#define LORA_ABP_CLASS "C"
#define LORA_DEV_EUI "????????????????"
#define LORA_DEV_ADDR "????????"
#define LORA_NWKS_KEY "????????????????????????????????"
#define LORA_APPS_KEY "????????????????????????????????"

String startTimeRelayA = "1970-01-01 00:00:01";
String stopTimeRelayA = "1970-01-01 00:00:01";
String startTimeRelayB = "1970-01-01 00:00:01";
String stopTimeRelayB = "1970-01-01 00:00:01";

LedModule redLed("Red LED", RED_LED_PIN);
LedModule greenLed("Green LED", GREEN_LED_PIN);

DS3231 clock;
RTCDateTime dt;
uint8_t seconds = 0;

CatLoRaS76S lora;
uint8_t intervalLoraTX = 30;
uint8_t intervalLoraRX = 1;
uint8_t port = 1;
String payload = "00";

RelayModule relayA("Relay A", RELAY_A_PIN);
RelayModule relayB("Relay B", RELAY_B_PIN);

void setup() {
  Serial.begin(115200);
  delay(2000);

  // Initialize LED
  redLed.begin();
  greenLed.begin();

  redLed.on();

  // Initialize Relay
  relayA.begin();
  relayB.begin();
  relayA.trunOnWhen(startTimeRelayA, stopTimeRelayA);
  relayB.trunOnWhen(startTimeRelayB, stopTimeRelayB);

  // Initialize DS3231
  Serial.println("Initialize DS3231");
  clock.begin();
  if (clock.isReady() && Serial) {
    clock.setDateTime(__DATE__, __TIME__);
  }

  // Initialize LoRa
  Serial.println("-> Lora Setting...");
  lora.begin(115200);
  Serial.println("-> Lora ABP Join...");
  lora.joinABP(String(LORA_ABP_CLASS),
               String(LORA_DEV_EUI),
               String(LORA_DEV_ADDR),
               String(LORA_NWKS_KEY),
               String(LORA_APPS_KEY));


  // start-up success -> red led turn-on;

  redLed.off();
  greenLed.on();
  Serial.println("-> Ready Go.");
}

void loop() {
  dt = clock.getDateTime();

  if (dt.second != seconds) {
    seconds = dt.second;

    // ----- Show current datetime on terminal
    String datetime = String(clock.dateFormat("Y-m-d H:i:s", dt));
    Serial.println(datetime);

    // ----- Check datetime for action -----
    relayA.doAction(dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
    relayB.doAction(dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);

    // ----- LoRa TX -----
    if (seconds % intervalLoraTX == 0) {
      loraTransmit();
    }

    // ----- LoRa RX -----
    if (seconds % intervalLoraRX == 0) {
      loraReceive();
    }
  }
}

void loraTransmit() {
  lora.transmit(port, payload);
}

void loraReceive() {
  String receive = lora.receive();
  if (receive != "") {
    unsigned int rxPort = lora.getPortReceive(receive);
    String rxPayload = lora.getPayloadReceive(receive);
    if (rxPort != 0 && rxPayload != "") {
      Serial.println("-----RX From CAT LORA-----");
      Serial.println(String("Port = ") + rxPort);
      Serial.println(String("Payload = ") + rxPayload);

      /*Downlink from Lora IoT by CAT is 012020012412105920200124143000 (00yyyyMMddhhmmssyyyyMMddhhmmss)
        Device : RelayA => 01, RelayB => 02
        Start : 2020-01-24 12:10:59 => 20200124121059
        Stop : 2020-01-24 14:30:00 => 20200124143000
        rxPayload = 012020012412105920200124143000;
      */

      //for device
      uint8_t device = (uint8_t) rxPayload.substring(0, 2).toInt();

      //new start datetime
      uint16_t sYear = (uint16_t) rxPayload.substring(2, 6).toInt();
      uint8_t sMonth = (uint8_t) rxPayload.substring(6, 8).toInt();
      uint8_t sDay = (uint8_t) rxPayload.substring(8, 10).toInt();
      uint8_t sHour = (uint8_t) rxPayload.substring(10, 12).toInt();
      uint8_t sMinute = (uint8_t) rxPayload.substring(12, 14).toInt();
      uint8_t sSecond = (uint8_t) rxPayload.substring(14, 16).toInt();

      //new stop datetime
      uint16_t eYear = (uint16_t) rxPayload.substring(16, 20).toInt();
      uint8_t eMonth = (uint8_t) rxPayload.substring(20, 22).toInt();
      uint8_t eDay = (uint8_t) rxPayload.substring(22, 24).toInt();
      uint8_t eHour = (uint8_t) rxPayload.substring(24, 26).toInt();
      uint8_t eMinute = (uint8_t) rxPayload.substring(26, 28).toInt();
      uint8_t eSecond = (uint8_t) rxPayload.substring(28, 30).toInt();

      //set startLedUnixtime and stopLedUnixtime
      String deviceSelected = "Not Found";
      if (device == 1) {
        deviceSelected = "Relay A";
        relayA.trunOnWhen(sYear, sMonth, sDay, sHour, sMinute, sSecond, eYear, eMonth, eDay, eHour, eMinute, eSecond);
      } else if (device == 2) {
        deviceSelected = "Relay B";
        relayB.trunOnWhen(sYear, sMonth, sDay, sHour, sMinute, sSecond, eYear, eMonth, eDay, eHour, eMinute, eSecond);
      }

      //set datetime format yyyy-MM-dd HH:mm:ss
      char startLedDateTime[32];
      char stopLedDateTime[32];
      sprintf(startLedDateTime, "%04d-%02d-%02d %02d:%02d:%02d", sYear, sMonth, sDay, sHour, sMinute, sSecond);
      sprintf(stopLedDateTime, "%04d-%02d-%02d %02d:%02d:%02d", eYear, eMonth, eDay, eHour, eMinute, eSecond);

      //Show new datetime
      Serial.println(String("Device Select : ") + deviceSelected);
      Serial.println(String("New Start LED : ") + startLedDateTime);
      Serial.println(String("New Stop LED : ") + stopLedDateTime);
    }
  }
}
