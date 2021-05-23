#include "mqtt.h"
#include <PubSubClient.h>
#include <EEPROM.h>
#include <WiFiClient.h>

String MODULE_MODEL = "001";

String clientID = "ESP-ModuleOne-" + MODULE_MODEL;
String invernaderoId = "A"; //TODO - estos datos son de MQTT o son del negocio?
String sectorId = "1";
String seccionId = "A";

// Datos mqtt
String mqttBroker = "192.168.1.105";
int mqttPort = 1883;
String mqtt_user = "";
String mqtt_pass = "";

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
  updateTopics();
  client.setServer(mqttBroker.c_str(), mqttPort);
}

void MQTT_publishData() {
  if(client.connected()) sendData(); //TODO - Chequear si es necesario verificar conexion wifi
}

void MQTT_reconectar() { //TODO - Chequear si es necesario verificar conexion wifi
  if (!client.connected()) {
    if (client.connect(clientID.c_str(), mqtt_user.c_str(), mqtt_pass.c_str())) Serial.println("MQTT connected.");
  }
  if (client.connected()) {
    client.loop();
  }
}

void MQTT_setServer() {
  client.disconnect();
  client.setServer(mqttBroker.c_str(), mqttPort);
}

void MQTT_updateConfig() { 
  config_t newConfig;
  EEPROM.get(0, newConfig);
  mqttBroker = newConfig.mqtt_broker;
  mqttPort = newConfig.broker_puerto;
  invernaderoId = newConfig.invernadero_id;
  sectorId = newConfig.sector_id;
  seccionId = newConfig.seccion_id;
  mqtt_user = newConfig.mqtt_user;
  mqtt_pass = newConfig.mqtt_pass;
  clientID = newConfig.client_id;

  MQTT_setServer();
  updateTopics();
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
  //TODO - Agregar los parametros para enviar los datos por mqtt
  if (client.publish(topicTempAmb.c_str(), "tempAmb")) Serial.println(topicHumAmb);
  client.publish(topicHumAmb.c_str(), "humAmb");
  client.publish(topicHumSuelo.c_str(), "humSuelo");
  client.publish(topicLuz.c_str(), "luz");
}
