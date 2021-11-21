#include "wifi_setup.h"
#include "config.h"
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>

const char* ssid = "Vacio";
const char* pass = "";
String wifiAPssid = "Módulo Vacío 005";// + MODULE_MODEL;//"Module " + MODULE_MODEL + " - inv:" + invernaderoId + " - " + sectorId + " - " + seccionId;
String wifiAPpass = "";  //TODO - cambiar pass

DNSServer dnsServer;
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

IPAddress deviceIP(192, 168, 0, 43);
IPAddress deviceGateway(192, 168, 0, 1);
IPAddress deviceDNS(8, 8, 8, 8);

void WifiSetup_disconnect() {
  WiFi.disconnect();
}

void WifiSetup_loop() {
  dnsServer.processNextRequest();
}

void WifiSetup_startAP(){
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(wifiAPssid, wifiAPpass);
  Serial.print("WIFI AP - IP address: ");
  Serial.println(WiFi.softAPIP());
}

void WifiSetup_stopAP() {
  WiFi.softAPdisconnect(true);
}

void WifiSetup_startDNS() {
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  } else {
    Serial.println("mDNS responder started");
    // Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);
  }

  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(53, "*", apIP);

  Serial.println("HTTP server started");
}

void WifiSetup_configureWifi() {
  if (WiFi.status() != WL_CONNECTED) {
    WifiSetup_updateConfig();
    Serial.print("Conectando a ");
    Serial.print(ssid);
    Serial.println("...");
    Serial.println("IP de Conf: 192.168.4.1");

    WiFi.config(deviceIP, deviceDNS, deviceGateway, netMsk);
    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      return;
    }
    Serial.println("");
    Serial.println("WiFi conectado");
    Serial.print("Direccion IP: ");
    Serial.println(WiFi.localIP());
  }
}

void WifiSetup_updateConfig() {
  config_t newConfig;
  EEPROM.get(0, newConfig);
  if (ssid != newConfig.wifi_ssid || pass != newConfig.wifi_pass) {
    ssid = newConfig.wifi_ssid;
    pass = newConfig.wifi_pass;
    WifiSetup_disconnect();
  }
}

