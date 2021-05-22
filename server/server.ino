#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include "config.h"
#include "webserver.h"

#include <SPI.h>

///--------------------Configuraciones básicas--------------------------///
String MODULE_MODEL = "001";

String clientID = "ESP-ModuleOne-" + MODULE_MODEL;
String invernaderoId = "A";
String sectorId = "1";
String seccionId = "A";

String wifiAPssid = "Módulo Vacío" + MODULE_MODEL;//"Module " + MODULE_MODEL + " - inv:" + invernaderoId + " - " + sectorId + " - " + seccionId;
String wifiAPpass = "";  //TODO - cambiar pass

const char* ssid = "Vacio";
const char* pass = "";

uint8_t wiFlag = 0;
long timeFlag = 3000;
long actualTimeOut = 0;
long timeOut = 10000;
long actualTimeSendData = 0;
long timeToSendData = 5000; // Intervalo de envio de información

config_t conf;

DNSServer dnsServer;
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

// Datos mqtt
String mqttBroker = "192.168.1.105";
int mqttPort = 1883;
String mqtt_user = "";
String mqtt_pass = "";

WiFiClient wclient;
PubSubClient client(wclient);

String topicGen;// = "/" + invernaderoId + "/" + sectorId + "/" + seccionId + "/";
String topicTempAmb, topicHumAmb, topicHumSuelo, topicLuz;

///--------------------No modificar a partir de aquí--------------------------///

bool updateFlag = false;

void callback(char* topic, byte* payload, unsigned int length) {}

void setup(void) {
  EEPROM.begin(sizeof(config_t));
  EEPROM.get(0, conf);

  Serial.begin(115200);
  WiFi.disconnect();
  ssid = conf.wifi_ssid;
  pass = conf.wifi_pass;
  mqttBroker = conf.mqtt_broker;
  mqttPort = conf.broker_puerto;
  updateTopics();

  iniciarWebServer();

  client.setServer(mqttBroker.c_str(), mqttPort);
  client.setCallback(callback);
  actualTimeOut = millis();
}

void loop(void) {

  updateFlag = WebServer_getActualizar();
  if((millis() - actualTimeOut) >= timeOut){
    wifiSetup();
    actualTimeOut = millis();
  }
  // Verificamos si hubo cambio en los datos a traves del server.
  if (updateFlag) {
    Serial.println("Se actualizaron los datos UPDATEFLAG");
    updateConfig();
  }

  if (!WebServer_isRunning()) {
    iniciarWebServer();
    Serial.println("Server iniciado");
  }
  //DNS
  dnsServer.processNextRequest();
  //HTTP
  WebServer_loop();

  if (WiFi.status() == WL_CONNECTED && client.connected()) {
    if((millis() - actualTimeSendData) >= timeToSendData){
      sendData();
      actualTimeSendData = millis();
    }
  }
  delay(10);
}


void iniciarWebServer() {
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(wifiAPssid, wifiAPpass);

  // Wait for connection

  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  } else {
    Serial.println("mDNS responder started");
    // Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);
  }
  WebServer_init();

  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(53, "*", apIP);

  Serial.println("HTTP server started");
}
void pararWebServer() {
  Serial.println("Parando WebServer");
  WebServer_stop();
  WiFi.softAPdisconnect(true);
}

// Conexion de wifi y conexion con el broker MQTT
void wifiSetup() {

  if (WiFi.status() != WL_CONNECTED) {
    if (updateFlag) {
      EEPROM.get(0, conf);
      ssid = conf.wifi_ssid;
      pass = conf.wifi_pass;
      WebServer_setActualizar(false);
    }
    Serial.print("Conectando a ");
    Serial.print(ssid);
    Serial.println("...");
    Serial.println("IP de Conf: 192.168.4.1");

    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      return;
    }
    Serial.println("");
    Serial.println("WiFi conectado");
    Serial.print("Direccion IP: ");
    Serial.println(WiFi.localIP());
  }

  reconectarMQTT();
}

void reconectarMQTT() {
  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      if (client.connect(clientID.c_str(), mqtt_user.c_str(), mqtt_pass.c_str())) Serial.println("MQTT connected.");
    }
    if (client.connected()) {
      client.loop();
    }
  }
}

void updateConfig() {
  EEPROM.get(0, conf);
  ssid = conf.wifi_ssid;
  pass = conf.wifi_pass;
  mqttBroker = conf.mqtt_broker;
  mqttPort = conf.broker_puerto;
  invernaderoId = conf.invernadero_id;
  sectorId = conf.sector_id;
  seccionId = conf.seccion_id;
  mqtt_user = conf.mqtt_user;
  mqtt_pass = conf.mqtt_pass;
  clientID = conf.client_id;
  //admin_pass;
  updateTopics();
  client.disconnect();
  client.setServer(mqttBroker.c_str(), mqttPort);
  reconectarMQTT();

  WebServer_setActualizar(false);
  Serial.println("Actualizamos los datos");
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
