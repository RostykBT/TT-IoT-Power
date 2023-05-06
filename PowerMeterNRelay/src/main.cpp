#include <Arduino.h>
#include <ittiot.h>
#include "EmonLib.h"
#include <Ticker.h>

#define MODULE_TOPIC "ESP30/UntitledTeam/RelayTopic"
#define SENSOR_TOPIC "UntitledTeam/CurrValue"


#define ADC_PIN A0  // Analog pin 0 (A0)
#define RELAY_PIN 5 // Relay pin 5 (D1)

EnergyMonitor emon1;
Ticker adcTicker;

bool adcFlag;
uint16_t adcSampleCount;

void setAdcFlag()
{
  adcFlag = true;
}

// Function started after the connection to the server is established.
void iot_connected()
{
  Serial.println("MQTT connected callback");
  iot.subscribe(MODULE_TOPIC); // subscribe to topic relay
  iot.log("IoT current example!");
}

void iot_received(String topic, String msg)
{
  Serial.print("MSG FROM USER callback, topic: ");
  Serial.print(topic);
  Serial.print(" payload: ");
  Serial.println(msg);

  if (topic == MODULE_TOPIC)
  {
    // If message received and it is 1, then switch relay on
    if (msg == "1")
    {
      digitalWrite(RELAY_PIN, HIGH);
    }
    // If message received and it is 0, then switch relay on
    if (msg == "0")
    {
      digitalWrite(RELAY_PIN, LOW);
    }
  }
}

void setup()
{
  // setting up serial connection parameter
  Serial.begin(115200);
  Serial.println("Booting");

  pinMode(RELAY_PIN, OUTPUT); // The relay pin is defined as output type

  iot.setConfig("wname", "Lviv");
  iot.setConfig("wpass", "$tr0ngP@$$w0rd");
  iot.setConfig("msrv", "192.168.0.103");
  iot.setConfig("mport", "1884");

  // print json config to serial
  iot.printConfig();
  // Initialize IoT library
  iot.setup();

  // Current: input pin, calibration (how big current can it read).
  emon1.current(ADC_PIN, 30);
  adcSampleCount = 1500;

  adcTicker.attach(0.5, setAdcFlag);
}

void loop()
{
  iot.handle();

  if (adcFlag)
  {
    adcFlag = false;
    double val = emon1.calcIrms(adcSampleCount);
    val = val / 100; // "100 aprox coeficient to get real value, can not be determined without calibration"
    String msg = String(val);
    // publishing measured current value to a MQTT broker
    iot.publishMsg(SENSOR_TOPIC, msg.c_str());
    // sending measured current value to a computer
    Serial.println(msg);
  }
} 