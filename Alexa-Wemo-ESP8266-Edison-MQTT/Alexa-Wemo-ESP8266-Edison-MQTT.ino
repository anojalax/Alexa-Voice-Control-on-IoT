#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include <MQTTClient.h>
#include "switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"

#define office_pin D1
#define kitchen_pin D2
#define bedroomlight_pin D3
#define garagedoor_pin D4
#define dishwasher_pin D5
#define charger_pin D6
#define livingroom_pin D7

// prototypes
boolean connectWifi();

//on/off callbacks 
void officeLightsOn();
void officeLightsOff();
void kitchenLightsOn();
void kitchenLightsOff();
void bedroomlightOn();
void bedroomlightOff();
void garagedoorOn();
void garagedoorOff();
void dishwasherOn();
void dishwasherOff();
void chargerOn();
void chargerOff();
void livingroomOn();
void livingroomOff();

    
const char* ssid = "ATT2.4";
const char* password = "Srirama7*";
const char* mqtt_server = "ec2-54-86-79-172.compute-1.amazonaws.com";
boolean wifiConnected = false;
MQTTClient client;
WiFiClient net;
unsigned long lastMillis = 0;
UpnpBroadcastResponder upnpBroadcastResponder;

Switch *office = NULL;
Switch *kitchen = NULL;
Switch *bedroomlight = NULL;
Switch *garagedoor = NULL;
Switch *dishwasher = NULL;
Switch *charger = NULL;
Switch *livingroom = NULL;

void setup()
{
    Serial.begin(9600);
    
    //pinMode(office_pin, OUTPUT); 
    pinMode(kitchen_pin, OUTPUT); 
    //pinMode(bedroomlight_pin, OUTPUT); 
    pinMode(garagedoor_pin, OUTPUT); 
    pinMode(dishwasher_pin, OUTPUT); 
    pinMode(charger_pin, OUTPUT); 
    pinMode(livingroom_pin, OUTPUT); 
    
   // Initialise wifi connection
    wifiConnected = connectWifi();
  
    if(wifiConnected){
    upnpBroadcastResponder.beginUdpMulticast();
    
    // Define your switches here. Max 14
    // Format: Alexa invocation name, local port no, on callback, off callback
    office = new Switch("office lights", 80, officeLightsOn, officeLightsOff);
    kitchen = new Switch("kitchen lights", 81, kitchenLightsOn, kitchenLightsOff);
    bedroomlight = new Switch("bedroom lights", 82, bedroomlightOn, bedroomlightOff);
    garagedoor = new Switch("garage door", 83, garagedoorOn, garagedoorOff);
    dishwasher = new Switch("dish washer", 84, dishwasherOn, dishwasherOff);
    charger = new Switch("charger", 85, chargerOn, chargerOff);
    livingroom = new Switch("living room lights", 86, livingroomOn, livingroomOff);

    Serial.println("Adding switches upnp broadcast responder");
    //upnpBroadcastResponder.addDevice(*office);
    upnpBroadcastResponder.addDevice(*kitchen);
    //upnpBroadcastResponder.addDevice(*bedroomlight);
    upnpBroadcastResponder.addDevice(*garagedoor);
    upnpBroadcastResponder.addDevice(*dishwasher);
    upnpBroadcastResponder.addDevice(*charger);
    upnpBroadcastResponder.addDevice(*livingroom);
    
  }
  client.begin(mqtt_server, net);
  connect();
  client.publish("/motion", "initialised");
  client.publish("/doorstatus", "initialised");
}
    
void loop()
{
  if (!client.connected()) {
      connect();
    }
   if(wifiConnected){
      upnpBroadcastResponder.serverLoop();
      
      kitchen->serverLoop();
      office->serverLoop();
      bedroomlight->serverLoop();
      garagedoor->serverLoop();
      dishwasher->serverLoop();
      charger->serverLoop();
      livingroom->serverLoop();
   }
}

void officeLightsOn() {
    digitalWrite(office_pin, HIGH); 
    Serial.print("Switch 1 turn on ...");
}

void officeLightsOff() {
    digitalWrite(office_pin, LOW);
    Serial.print("Switch 1 turn off ...");
}

void kitchenLightsOn() {
    digitalWrite(kitchen_pin, HIGH); 
    Serial.print("Switch 2 turn on ...");
}

void kitchenLightsOff() {
  digitalWrite(kitchen_pin, LOW);
  Serial.print("Switch 2 turn off ...");
}
void bedroomlightOn() {
    digitalWrite(bedroomlight_pin, HIGH); 
    Serial.print("Switch 3 turn on ...");
}

void bedroomlightOff() {
  digitalWrite(bedroomlight_pin, LOW);
  Serial.print("Switch 3 turn off ...");
  
}void garagedoorOn() {
    digitalWrite(garagedoor_pin, HIGH); 
    Serial.print("Switch 4 turn on ...");
}

void garagedoorOff() {
  digitalWrite(garagedoor_pin, LOW);
  Serial.print("Switch 4 turn off ...");
  
}void dishwasherOn() {
    digitalWrite(dishwasher_pin, HIGH); 
    Serial.print("Switch 5 turn on ...");
}

void dishwasherOff() {
  digitalWrite(dishwasher_pin, LOW);
  Serial.print("Switch 5 turn off ...");
}

void chargerOn() {
    digitalWrite(charger_pin, HIGH); 
    Serial.print("Switch 6 turn on ...");
}

void chargerOff() {
  digitalWrite(charger_pin, LOW);
  Serial.print("Switch 6 turn off ...");
}

void livingroomOn() {
    digitalWrite(livingroom_pin, HIGH); 
    Serial.print("Switch 7 turn on ...");
    client.publish("/home/livingroom","Living room ON");
}

void livingroomOff() {
  digitalWrite(livingroom_pin, LOW);
  Serial.print("Switch 7 turn off ...");
  client.publish("/home/livingroom","Living room OFF");
}

// connect to wifi – returns true if successful or false if not
boolean connectWifi(){
  boolean state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10){
      state = false;
      break;
    }
    i++;
  }
  
  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
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
