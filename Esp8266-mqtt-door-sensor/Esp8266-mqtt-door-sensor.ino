#include <ESP8266WiFi.h>
#include <MQTTClient.h>

bool OPEN = false;
const char* ssid = "ASUS";
const char* password = "12345678";

const char* mqtt_server = "ec2-54-86-79-***.compute-1.amazonaws.com";
WiFiClient net;
MQTTClient client;
unsigned long lastMillis = 0;
int LED = D0;
int DOOR_SENSOR = D7;


void setup() {
  Serial.begin(9600);
  pinMode(DOOR_SENSOR, INPUT);   // declare sensor as input
  pinMode(LED, OUTPUT);
  WiFi.begin(ssid, password);
  client.begin(mqtt_server, net);
  connect();
  client.publish("/doorstatus", "initialised");
}
void loop() {
      if (!client.connected()) {
        connect();
      }
     
    long doorstate = digitalRead(DOOR_SENSOR);
    if(doorstate == HIGH && OPEN == true){
        Serial.println("Door Closed");
        client.publish("/doorstatus","Door Closed");
        digitalWrite(LED,LOW);
        OPEN = false;
      }
     else if(doorstate == LOW && OPEN == false) {
        Serial.println("Door Open");
        client.publish("/doorstatus","Door Open");
        digitalWrite(LED,HIGH);
        OPEN = true;
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
