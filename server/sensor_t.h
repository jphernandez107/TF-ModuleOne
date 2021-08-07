#ifndef SENSORS_T
#define SENSORS_T

#define SENSOR_COUNT 4

typedef struct {
  char topic_end[50] = "Vacio";
  float value = 0.0;
} sensor_t;
#endif