#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>
// #include <DHT_U.h>

#include "sensors.h"

#define DHTTYPE DHT22
const int DHTPin = 2;
DHT dht(DHTPin, DHTTYPE);

BH1750 lightMeter;

sensor_t sensors[SENSOR_COUNT];

void Sensors_getRoomTemperature();
void Sensors_getRoomHumidity();
void Sensors_getRoomLux();

void Sensors_setup() {
  pinMode(DHTPin, INPUT);
  dht.begin();

  Wire.begin();
  lightMeter.begin();
}

void Sensors_getRoomTemperature() {
  String topic = "temperatura-ambiente";
  topic.toCharArray(sensors[0].topic_end, 50);
  sensors[0].value = 0.0;//dht.readTemperature();
}
void Sensors_getRoomHumidity() {
  String topic = "humedad-ambiente";
  topic.toCharArray(sensors[1].topic_end, 50);
  sensors[1].value = 0.0;//dht.readHumidity();
}
void Sensors_getRoomLux() {
  String topic = "luz";
  topic.toCharArray(sensors[2].topic_end, 50);
  sensors[2].value = lightMeter.readLightLevel();
}

void Sensors_getSensors(sensor_t getSensors[]) {
  Sensors_getRoomTemperature();
  Sensors_getRoomHumidity();
  Sensors_getRoomLux();
  for (int i=0; i<SENSOR_COUNT; i++) {
    getSensors[i] = sensors[i];
  }
}