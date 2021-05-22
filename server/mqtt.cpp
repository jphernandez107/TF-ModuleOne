#include "mqtt.h"
#include "wifi_setup.h"
#include <PubSubClient.h>
#include <EEPROM.h>
#include <WiFiClient.h>

String MODULE_MODEL = "001";

String clientID = "ESP-ModuleOne-" + MODULE_MODEL;
String invernaderoId = "A"; //TODO - estos datos son de MQTT o son del negocio?
String sectorId = "1";
String seccionId = "A";

long actualTimeSendData = 0;
long timeToSendData = 5000; // Intervalo de envio de información

// Datos mqtt
String mqttBroker = "192.168.1.105";
int mqttPort = 1883;
String mqtt_user = "";
String mqtt_pass = "";

config_t config;

WiFiClient wclient;
PubSubClient client(wclient);


String topicGen;// = "/" + invernaderoId + "/" + sectorId + "/" + seccionId + "/";
String topicTempAmb, topicHumAmb, topicHumSuelo, topicLuz;

/**
 * Declaracion de funciones privadas
*/
void updateTopics();
void sendData();

void MQTT_setup() {
  mqttBroker = config.mqtt_broker;
  mqttPort = config.broker_puerto;
  updateTopics();
  client.setServer(mqttBroker.c_str(), mqttPort);
}

void MQTT_loop(long millis) {
  if((millis - actualTimeSendData) >= timeToSendData && client.connected()){ //TODO - Chequear si es necesario verificar conexion wifi
      sendData();
      actualTimeSendData = millis;
    }
}

void MQTT_reconectar() { //TODO - Chequear si es necesario verificar conexion wifi
  if (!client.connected()) {
    Serial.println("Reconectando... ");
    if (client.connect(clientID.c_str(), mqtt_user.c_str(), mqtt_pass.c_str())) Serial.println("MQTT connected.");
  }
  if (client.connected()) {
    client.loop();
  }
}

void MQTT_setServer(String broker, int port) {
  updateTopics();
  client.disconnect();
  client.setServer(broker.c_str(), port);
}

void MQTT_updateConfig(config_t config) { 
  mqttBroker = config.mqtt_broker;
  mqttPort = config.broker_puerto;
  invernaderoId = config.invernadero_id;
  sectorId = config.sector_id;
  seccionId = config.seccion_id;
  mqtt_user = config.mqtt_user;
  mqtt_pass = config.mqtt_pass;
  clientID = config.client_id;

  MQTT_setServer(mqttBroker, mqttPort);
  MQTT_reconectar();
}


void updateTopics() {
  topicGen = "/" + invernaderoId + "/" + sectorId + "/" + seccionId + "/";
  topicTempAmb = topicGen + "temperatura-ambiente";
  topicHumAmb = topicGen + "humedad-ambiente";
  topicHumSuelo = topicGen + "humedad-suelo";
  topicLuz = topicGen + "luz";
}

void sendData() {
  //TODO - Agregar el codigo para enviar los datos por mqtt (tambien agregar el delay en el if)
  client.publish(topicTempAmb.c_str(), "tempAmb");
  client.publish(topicHumAmb.c_str(), "humAmb");
  client.publish(topicHumSuelo.c_str(), "humSuelo");
  client.publish(topicLuz.c_str(), "luz");
}
