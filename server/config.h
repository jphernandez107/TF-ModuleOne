typedef struct {
  // Accesspoint 
  char wifi_ssid[50] = "Vacio";
  char wifi_pass[50] = "";

  // Broker
  char mqtt_broker[50] = "192.168.0.100";
  int broker_puerto = 1883;

  // Ubicación
  char invernadero_id[100] = "A";
  char sector_id[100] = "1";
  char seccion_id[100] = "A";
  
  char mqtt_user[50] = "";
  char mqtt_pass[50] = "";

  //extras
  int admin_protected = 0;
  char admin_pass[20] = "";
  char admin_user[20] = "";

  char client_id[30] = "ESP-ModuleOne-001";

} config_t;
