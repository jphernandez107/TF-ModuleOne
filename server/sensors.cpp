#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>
// #include <DHT_U.h>

#include "sensors.h"

#define DHTTYPE DHT22
const int DHTPin = 2;
DHT dht(DHTPin, DHTTYPE);

BH1750 lightMeter;

const int soilHumidityPin = A0;  // ESP8266 Analog Pin ADC0 = A0

sensor_t sensors[SENSOR_COUNT];

void Sensors_getRoomTemperature();
void Sensors_getRoomHumidity();
void Sensors_getRoomLux();
void Sensors_getSoilHumidity();

uint8_t getSoilHumidity(uint16_t adc_value);

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
void Sensors_getSoilHumidity() {
  String topic = "humedad-suelo";
  topic.toCharArray(sensors[3].topic_end, 50);
  sensors[3].value = getSoilHumidity(analogRead(soilHumidityPin));
}

void Sensors_getSensors(sensor_t getSensors[]) {
  Sensors_getRoomTemperature();
  Sensors_getRoomHumidity();
  Sensors_getRoomLux();
  Sensors_getSoilHumidity();
  for (int i=0; i<SENSOR_COUNT; i++) {
    getSensors[i] = sensors[i];
  }
}

#define INPUT_START 860.0
#define INPUT_END 445.0
#define OUTPUT_START 0.0
#define OUTPUT_END 100.0

uint8_t getSoilHumidity(uint16_t adc_value) {
    if (adc_value > INPUT_START) adc_value = INPUT_START;
    if (adc_value <= INPUT_END) adc_value = INPUT_END + 1;
    uint8_t value = OUTPUT_START + ((OUTPUT_END - OUTPUT_START) / (INPUT_END - INPUT_START)) * (adc_value - INPUT_START);
    if(value >= 100) value = 100;
    if(value <= 0) value = 0;
    return value;
}
