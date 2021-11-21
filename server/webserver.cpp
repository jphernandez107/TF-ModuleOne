#include "webserver.h"
#include "index.html.gz.h"
#include "config.h"
#include <EEPROM.h>

config_t newConfig;

bool isRunning = false;
bool actualizar = false;

ESP8266WebServer server(80);

void handleRoot() {
  config_t confi;
  EEPROM.get(0, confi);
  String pass = confi.admin_pass;
  String user = confi.admin_user;
  
  // if (pass.equals(".") && user.equals(".")) {
    confi.admin_protected = 0;
  // }
  EEPROM.put(0, confi);
  EEPROM.commit();
  EEPROM.get(0, confi);
  Serial.print(confi.admin_protected);
  if (confi.admin_protected == 1) { //TODO - verificar que funcione bien la auth (testear mucho)
    if (!server.authenticate(user.c_str(), pass.c_str())) {
       return server.requestAuthentication();
    }
  }
  server.sendHeader("Content-Encoding", "gzip");
  server.send_P(200, "text/html", index_html_gz, index_html_gz_len);
}

void handleConfig() {
  EEPROM.get(0, newConfig);

  String wifi_ssid(server.arg("wifi_ssid"));
  String wifi_pass(server.arg("wifi_pass"));
  String mqtt_broker(server.arg("mqtt_broker"));
  String mqtt_port(server.arg("mqtt_port"));
  String mqtt_user(server.arg("mqtt_user"));
  String mqtt_pass(server.arg("mqtt_pass"));
  String invernadero_id(server.arg("invernadero_id"));
  String sector_id(server.arg("sector_id"));
  String seccion_id(server.arg("seccion_id"));
  String admin_user(server.arg("admin_user"));
  String admin_pass(server.arg("admin_pass"));
  String client_id(server.arg("client_id"));
  String empty = "";

  if (server.arg("admin_pass") == "null" && server.arg("admin_user") == "null") {
    newConfig.admin_protected = 0;
    empty.toCharArray(newConfig.admin_user, 20);
    empty.toCharArray(newConfig.admin_pass, 20);
  }
  else if (server.arg("admin_pass") != "null" && server.arg("admin_user") != "null"){
    newConfig.admin_protected = 1;
  }

  if (wifi_ssid != empty) {
    wifi_ssid.toCharArray(newConfig.wifi_ssid, 50);
  }
  if (wifi_pass != empty && !wifi_pass.equals("null")) {
    wifi_pass.toCharArray(newConfig.wifi_pass, 50);
  } else if (wifi_pass.equals("null")) {
    empty.toCharArray(newConfig.wifi_pass, 20);
  }
  if (mqtt_broker != empty) {
    mqtt_broker.toCharArray(newConfig.mqtt_broker, 50);
  }
  if (mqtt_port != empty) {
    newConfig.broker_puerto = mqtt_port.toInt();
  }
  if (invernadero_id != empty) {
    invernadero_id.toCharArray(newConfig.invernadero_id, 100);
  }
  if (sector_id != empty) {
    sector_id.toCharArray(newConfig.sector_id, 100);
  }
  if (seccion_id != empty) {
    seccion_id.toCharArray(newConfig.seccion_id, 100);
  }
  if (mqtt_user != empty && !mqtt_user.equals("null")) {
    mqtt_user.toCharArray(newConfig.mqtt_user, 50);
  } else if (mqtt_user.equals("null")) {
    empty.toCharArray(newConfig.mqtt_user, 20);
  }
  if (mqtt_pass != empty && mqtt_pass != "null") {
    mqtt_pass.toCharArray(newConfig.mqtt_pass, 50);
  } else if (mqtt_pass == "null") {
    empty.toCharArray(newConfig.mqtt_pass, 20);
  }
  if (admin_pass != empty && admin_pass != "null") {
    admin_pass.toCharArray(newConfig.admin_pass, 20);
  } else if (admin_pass == "null") {
    admin_pass.toCharArray(".", 20);
  }
  
  if (admin_user != empty && admin_user != "null") {
    admin_user.toCharArray(newConfig.admin_user, 20);
  } else if (admin_user == "null") {
    admin_user.toCharArray(".", 20);
  }
  if (client_id != empty) {
    client_id.toCharArray(newConfig.client_id, 10);
  }
  EEPROM.put(0, newConfig);
  EEPROM.commit();

  server.send_P(200, "text/plain", "Datos cargados correctamente");

  actualizar = true;
}

void handleData() {

}

void handleNotFound() {
  String message = "Error 404 File Not Found\n\nURI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void WebServer_init() {
  server.on("/", handleRoot);
  server.on("/config", HTTP_POST, handleConfig);
  server.on("/data", handleData); // Tomamos los datos de config, y los enviamos al arduino.
  server.onNotFound(handleNotFound);
  server.begin();
  isRunning = true;
  Serial.println("Server iniciado");
}

void WebServer_stop() {
  Serial.println("Parando WebServer");
  server.stop();
  isRunning = false;
}

void WebServer_loop() {
  server.handleClient();
}

bool WebServer_isRunning() {
  return isRunning;
}

bool WebServer_getActualizar() {
  return actualizar;
}

void WebServer_setActualizar (bool newFlag) {
  actualizar = newFlag;
}
