// #include "config.h"
// #include "wifi_setup.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include "mqtt.h"
#include <PubSubClient.h>
#include <EEPROM.h>
#include "webserver.h"

#include <SPI.h>

///--------------------Configuraciones básicas--------------------------///
// String MODULE_MODEL = "001";

// String clientID = "ESP-ModuleOne-" + MODULE_MODEL;
// String invernaderoId = "A";
// String sectorId = "1";
// String seccionId = "A";

String wifiAPssid = "Módulo Vacío 001";// + MODULE_MODEL;//"Module " + MODULE_MODEL + " - inv:" + invernaderoId + " - " + sectorId + " - " + seccionId;
String wifiAPpass = "";  //TODO - cambiar pass

const char* ssid = "Vacio";
const char* pass = "";

long actualTimeOut = 0;
long timeOut = 10000;

config_t conf;

DNSServer dnsServer;
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

// // WiFiClient wclient;
// PubSubClient client(WifiSetup_WiFiClient());

///--------------------No modificar a partir de aquí--------------------------///

bool updateFlag = false;

void setup(void) {
  EEPROM.begin(sizeof(config_t));
  // EEPROM.get(0, conf);

  Serial.begin(115200);
  WiFi.disconnect();
  updateConfig();
  // ssid = conf.wifi_ssid;
  // pass = conf.wifi_pass;

  iniciarWebServer();

  MQTT_setup();

  actualTimeOut = millis();
  printConfig();
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

  MQTT_loop(millis());
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

  MQTT_reconectar();
}

void updateConfig() {
  EEPROM.get(0, conf);
  ssid = conf.wifi_ssid;
  pass = conf.wifi_pass;

  MQTT_updateConfig(conf);
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
