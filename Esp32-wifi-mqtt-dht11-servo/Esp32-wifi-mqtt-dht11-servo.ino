/*This program reads the temparature and humidity values from DHT11 sensor */
/* Send sdata through MQTT */
/* Control temp value (t >35) using Servo motor*/
//TODO : set temp thres as 32

#include <WiFi.h>
#include <MQTTClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "esp32-hal-ledc.h"

#define COUNT_LOW 0
#define COUNT_HIGH 8888
#define TIMER_WIDTH 16
#define SERVO_MOTOR       12
#define DHTPIN            27         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT11     // DHT 11 
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

const char* ssid = "ATT2.4";
const char* password = "Srirama7*";
const char* mqtt_server = "ec2-54-86-79-172.compute-1.amazonaws.com";
char tempvalue[15];
char humvalue[15];
char servostatus[15];
WiFiClient net;
MQTTClient client;
void setup()
{
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    client.begin(mqtt_server,net);
    connect();
    client.publish("/temp","initialised");
    dht.begin();
    sensor_t sensor;
      dht.temperature().getSensor(&sensor);
    Serial.println("------------------------------------");
    Serial.println("Temperature");
    Serial.print  ("Sensor:       "); Serial.println(sensor.name);
    Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
    Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
    Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
    Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
    Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");  
    Serial.println("------------------------------------");
    // Print humidity sensor details.
    dht.humidity().getSensor(&sensor);
    Serial.println("------------------------------------");
    Serial.println("Humidity");
    Serial.print  ("Sensor:       "); Serial.println(sensor.name);
    Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
    Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
    Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
    Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
    Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");  
    Serial.println("------------------------------------");
    // Set delay between sensor readings based on sensor details.
    delayMS = sensor.min_delay / 1000;
    ledcSetup(1, 50, TIMER_WIDTH); // channel 1, 50 Hz, 16-bit width
    ledcAttachPin( SERVO_MOTOR, 1); 
    
}

void loop(){
  if (!client.connected()) {
      connect();
    }
    
    
   // Delay between measurements.
    delay(delayMS);
    // Get temperature event and print its value.
    sensors_event_t event;  
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      Serial.println("Error reading temperature!");
    }
    else {
      Serial.print("Temperature: ");
      Serial.print(event.temperature);
      Serial.println(" *C");
      if(event.temperature > 35)
       {
          Serial.println("Temp very high");
          for (int i=COUNT_LOW ; i < COUNT_HIGH ; i=i+100)
           {
              ledcWrite(1, i);       // sweep servo 1
              delay(50);
           }
       }
      dtostrf(event.temperature, 5, 2, tempvalue);
      client.publish("/temp",tempvalue); 
      //Serial.println(tempvalue);
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      Serial.println("Error reading humidity!");
    }
    else {
      /*Serial.print("Humidity: ");
      Serial.print(event.relative_humidity);
      Serial.println("%");*/
      dtostrf(event.temperature, 5, 2, humvalue);
      client.publish("/humidity",humvalue); 
      
    }
    //bool servostatus=false;
    client.subscribe("/servo");
    //Serial.println(client.subscribe("/servo"));
  
}

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("esp", "try", "try")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");
}

void messageReceived(String topic, String payload, char * bytes, unsigned int length) {
  Serial.print("incoming: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.print(payload);
  Serial.println();
}
