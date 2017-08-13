#include <ESP8266WiFi.h>
#include <MQTTClient.h>
const char* ssid = "ATT2.4";
const char* password = "Srirama7*";
/*
const char* ssid = "aftab";
const char* password = "abgoosht";
*/
const char* mqtt_server = "ec2-54-86-79-172.compute-1.amazonaws.com";
WiFiClient net;
MQTTClient client;
unsigned long lastMillis = 0;
int PIR_SENSOR = D7;  // Digital pin D7
int DOOR_SENSOR = D3;


void setup() {
  Serial.begin(9600);
  pinMode(PIR_SENSOR, INPUT);   // declare sensor as input
  pinMode(DOOR_SENSOR, INPUT);   // declare sensor as input
  WiFi.begin(ssid, password);
  client.begin(mqtt_server, net);
  connect();
  client.publish("/motion", "initialised");
  client.publish("/doorstatus", "initialised");
}
void loop() {
   if (!client.connected()) {
      connect();
    }
   long state = digitalRead(PIR_SENSOR);
   long doorstate = digitalRead(DOOR_SENSOR);
   
    if(state == HIGH) {
      Serial.println("Motion detected!");
      client.publish("/motion","Motion detected");  
    }
    else {
      Serial.println("Motion absent!");
      client.publish("/motion","Motion absent");
      }
    if(doorstate ==HIGH){
        Serial.println("Door Closed");
        client.publish("/doorstatus","Door Closed");
      }
     else {
        Serial.println("Door Open");
        client.publish("/doorstatus","Door Open");
      }
      delay(1000);
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
