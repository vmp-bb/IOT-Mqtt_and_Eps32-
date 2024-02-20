// From https://randomnerdtutorials.com/esp32-mqtt-publish-subscribe-arduino-ide
//http://www.hivemq.com/demos/websocket-client/
#include <WiFi.h>
#include "PubSubClient.h"


const char* ssid = "Wokwi-GUEST";
const char* password = "";
//const char* mqttServer = "broker.emqx.io";
const char* mqttServer = "broker.mqttdashboard.com";
int port = 1883;
String stMac;
char mac[50];
char clientId[50];


WiFiClient espClient;
PubSubClient client(espClient);


const int ledPin = 2;


void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  wifiConnect();
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
  stMac = WiFi.macAddress();
  stMac.replace(":", "_");
  Serial.println(stMac);
  client.setServer(mqttServer, port);
  client.setCallback(callback);
  pinMode(ledPin, OUTPUT);
}


void wifiConnect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}


void mqttReconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    long r = random(1000);
    sprintf(clientId, "clientId-%ld", r);
    if (client.connect(clientId)) {
      Serial.print(clientId);
      Serial.println(" connected");
      client.subscribe("topicName/new");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String stMessage;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    stMessage += (char)message[i];
  }
  Serial.println();
  if (String(topic) == "topicName/new") {
    Serial.print("Changing output to ");
    if(stMessage == "on"){
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if(stMessage == "off"){
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }
}


void loop() {
  delay(10);
  if (!client.connected()) {
    mqttReconnect();
  }
  client.loop();
}


