#include "config.h"
#include "wifi_setup.h"
#include "mqtt.h"
#include "sensors.h"
#include <EEPROM.h>
#include "webserver.h"
#include <SPI.h>

///--------------------Configuraciones básicas--------------------------///

long actualTimeOut = 0;
long timeOut = 10000;
long actualTimeSendData = 0;
long timeToSendData = 30000; // Intervalo de envio de información

config_t conf;

///--------------------No modificar a partir de aquí--------------------------///

bool updateFlag = false;

void setup(void) {
  EEPROM.begin(sizeof(config_t));

  Serial.begin(115200);
  WifiSetup_disconnect();
  
  MQTT_setup();
  updateConfig();

  iniciarWebServer();
  Sensors_setup();
  
  actualTimeOut = millis();
  printConfig();
}

void loop(void) {

  updateFlag = WebServer_getActualizar();
  // Verificamos si hubo cambio en los datos a traves del server.
  if (updateFlag) {
    Serial.println("Se actualizaron los datos UPDATEFLAG");
    updateConfig();
  }

  if((millis() - actualTimeOut) >= timeOut){
    wifiSetup();
    actualTimeOut = millis();
  }

  if (!WebServer_isRunning()) {
    iniciarWebServer();
  }
  //DNS
  WifiSetup_loop();
  //HTTP
  WebServer_loop();

  if ((millis() - actualTimeSendData) >= timeToSendData) {
    sensor_t sens[SENSOR_COUNT];
    Sensors_getSensors(sens);
    MQTT_publishData(sens);
    actualTimeSendData = millis();
  }
  delay(10);
}

void iniciarWebServer() {
  WifiSetup_startAP();
  WifiSetup_startDNS();
  WebServer_init();
}
void pararWebServer() {
  WebServer_stop();
  WifiSetup_stopAP();
}

// Conexion de wifi y conexion con el broker MQTT
void wifiSetup() {
  if (updateFlag) {
    WifiSetup_updateConfig();
    WebServer_setActualizar(false);
  }
  WifiSetup_configureWifi();
  MQTT_reconectar();
}

void updateConfig() {
  EEPROM.get(0, conf);
  WifiSetup_updateConfig();
  MQTT_updateConfig();
  WebServer_setActualizar(false);
  Serial.println("Actualizamos los datos");
}

void printConfig() {
  Serial.print("ssid: ");
  Serial.println(conf.wifi_ssid);
  Serial.print("pass: ");
  Serial.println(conf.wifi_pass);
  Serial.print("broker: ");
  Serial.println(conf.mqtt_broker);
  Serial.print("port: ");
  Serial.println(conf.broker_puerto);
  Serial.print("invId: ");
  Serial.println(conf.invernadero_id);
  Serial.print("mUser: ");
  Serial.println(conf.mqtt_user);
  Serial.print("mPass: ");
  Serial.println(conf.mqtt_pass);
}
