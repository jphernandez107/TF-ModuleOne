#include "config.h"
#include <WiFiClient.h>
#include "sensor_t.h"

void MQTT_setup();
void MQTT_publishData(sensor_t sensors[]);
void MQTT_reconectar();
void MQTT_updateConfig();