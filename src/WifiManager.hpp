#pragma once
#include <Arduino.h>
#include "credentials.h"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
const char* topic_test = "doorbell/test";

class WifiManager {
 public:
  void setup_wifi();
  void setup_mqtt();
  bool mqttConnect();
  void publish(const char* topic, const char* payload);
  static void mqttCallback(char* topic, byte* payload,
                                        unsigned int len);
};

// WiFi
void WifiManager::setup_wifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Connecting to WiFi: ");
    Serial.println(WIFI_SSID);
  }
  Serial.println("WiFi Connected!");
}

void WifiManager::setup_mqtt() {
  Serial.println("Setup mqttClient");
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  Serial.println(mqttConnect() ? " ...done" : " ...failed");
}

void WifiManager::mqttCallback(char* topic, byte* payload, unsigned int len) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.write(payload, len);
  Serial.println();
}

bool WifiManager::mqttConnect() {
  Serial.print("Mqtt connecting to ");
  Serial.print(MQTT_SERVER);

  boolean status = mqttClient.connect(MQTT_CLIENTID, MQTT_USERNAME,
                                MQTT_PASSWORD);  // mqttClient.connect(MQTT_CLIENTID);

  if (status == false) {
    Serial.println(" fail");
    return false;
  }
  Serial.println(" success");

  // subscribe to topic
  mqttClient.subscribe(topic_test);
  mqttClient.subscribe("doorbell/state");

  Serial.print("Subscribed topic [");
  Serial.print(topic_test);
  Serial.println("]");

  // publish to topic
  publish(topic_test, "doorbell connect to mqtt");

  return mqttClient.connected();
}

void WifiManager::publish(const char* topic, const char* payload) {
  mqttClient.publish(topic, payload);
  Serial.print("Published [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(payload);
}