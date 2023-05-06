#include <Arduino.h>
#include <ittiot.h>
 
#define MODULE_TOPIC "ESP30/pir"
 
//Pin definition for the PIR (GPIO14)
#define PIR_PIN D5
//Pin definition for the PIR LED (GPIO16)
#define PIR_LED_PIN D4
 
// PIR state for detection
bool pirState;
// State that switches PIR on and off
bool onState;
 
// If message received for PIR topic. For example:
// mosquitto_pub -u test -P test -t "ITT/IOT/3/pir" -m "1"
void iot_received(String topic, String msg)
{
  Serial.print("MSG FROM USER callback, topic: ");
  Serial.print(topic);
  Serial.print(" payload: ");
  Serial.println(msg);
 
  if(topic == MODULE_TOPIC)
  {
    // Switching the PIR shield on or off, depending what message is received
    if(msg == "1")
    {
  	   onState = true;
    }
    if(msg == "0")
    {
  	   onState = false;
    }
  }
}
// Function started after the connection to the server is established.
void iot_connected()
{
  Serial.println("MQTT connected callback");
  // Subscribe to the topic "pir"
  iot.subscribe(MODULE_TOPIC);
  iot.log("IoT PIR example!");
}
 
void setup()
{
  Serial.begin(115200); // setting up serial connection parameter
  Serial.println("Booting");
 
  iot.setConfig("wname", "Lviv");
  iot.setConfig("wpass", "$tr0ngP@$$w0rd");
  iot.setConfig("msrv", "192.168.0.103");
  iot.setConfig("mport", "1884");
  iot.printConfig(); // Print json config to serial
  iot.setup(); // Initialize IoT library
 
  // Initialize PIR pin
  pinMode(PIR_PIN, INPUT);
  pinMode(PIR_LED_PIN, OUTPUT);
}
 
unsigned long Start = 0;
float counter = 0;
unsigned long Current =0;



void loop()
{
  iot.handle();

  if (millis() - Start >= 60000)//every minute
  {
      if (millis() - Current >= 1000)//every second
      {
          Current = millis();
          if (digitalRead(PIR_PIN) == HIGH)
          {
              counter++;
              Serial.println(counter);
          }
      }
  }
  else
  {
      counter = counter / 60 * 10; 
      counter = round(counter);
      Serial.println("Counter: " + String(int(counter)));

      counter = 0;
      Start = millis();

  }

  // publish Activiti Value to the server
  iot.publishMsg(MODULE_TOPIC, String(int(counter)).c_str());
}