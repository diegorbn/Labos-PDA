#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/******** Wi-Fi ********/
#define WLAN_SSID   "ARTEFACTOS"
#define WLAN_PASS   "ARTEFACTOS"

/******** Adafruit IO ********/
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "diegourbn"
#define AIO_KEY         ""

/******** MQTT ********/
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/******** Feed ********/
Adafruit_MQTT_Publish pubAlarma = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/fc-51");

/******** Sensor FC-51 ********/
const int PIN_FC51 = 25;

void conectarWiFi() {
  Serial.print("Conectando a WiFi...");
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 40) {
    delay(500); Serial.print(".");
    intentos++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado");
    Serial.print("IP asignada: "); Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nNo se pudo conectar a WiFi");
  }
}

void MQTT_connect() {
  if (mqtt.connected()) return;
  Serial.print("Conectando a Adafruit IO... ");
  int8_t ret; uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Reintento en 10 s...");
    mqtt.disconnect(); delay(10000);
    if (--retries == 0) while (1) delay(1);
  }
  Serial.println("¡Conectado!");
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_FC51, INPUT);
  conectarWiFi();
}

void loop() {
  MQTT_connect();
  mqtt.processPackets(10000);
  if (!mqtt.ping()) mqtt.disconnect();

  int estado = digitalRead(PIN_FC51);
  bool obstaculo = (estado == LOW);

  if (obstaculo) {
    Serial.println("Objeto detectado");
    pubAlarma.publish("Objeto detectado");
  } else {
    Serial.println("Libre");
    pubAlarma.publish("Libre");
  }

  delay(2000);
}