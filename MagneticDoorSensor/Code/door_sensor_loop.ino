#include <WiFi.h>
#include <time.h>
#include <ArduinoMqttClient.h>
#include <esp_sleep.h>

#define SENSOR_PIN A2

// WiFi crendentials
const char* ssid                = "Team4";
const char* password            = "12345678";   

// MQTT broker address
const char  mqtt_broker[]       = "team4.wlan";
int         mqtt_port           = 1883;

// MQTT credentials
const char  mqtt_username[]     = "Team4";
const char  mqtt_pwd[]          = "Team4";

// MQTT topics
const char  PubTopic[]          = "/door_sensor";

int pin_state;
int old_pin_state;

WiFiClient    wifiClient;
MqttClient    mqttClient(wifiClient);

void setup() {
  // put your setup code here, to run once:
  // Serial.begin(115200);
  // delay(500);
  // Serial.println("Connecting to WiFi");
  // Serial.println("");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  // Serial.println("Connecting to MQTT");
  // Serial.println("");
  mqttClient.setUsernamePassword(mqtt_username, mqtt_pwd);
  while (!mqttClient.connect(mqtt_broker, mqtt_port));

  old_pin_state = digitalRead(SENSOR_PIN);
}

void loop() {
  // put your main code here, to run repeatedly:
  mqttClient.poll();
  pin_state = digitalRead(SENSOR_PIN);

  // Serial.print("Pin State: ");
  // Serial.println(pin_state);
  // Serial.print("Old Pin State: ");
  // Serial.println(old_pin_state);
  // Serial.println("");

  if(WiFi.status() == WL_CONNECTED && mqttClient.connected()){

    if (pin_state != old_pin_state){

      // Serial.println("Publishing on /door_sensor topic");

      mqttClient.beginMessage(PubTopic);
      if (pin_state) {
        // Serial.println("Door Closed");
        mqttClient.print("Door Closed");
      }
      else {
        // Serial.println("Door Open");
        mqttClient.print("Door Open");
      }
      mqttClient.endMessage();
      delay(500);
      // Serial.println("");
      // Serial.println("Waiting for door state to change");
    }

  }
  else {
    // Serial.println("Reconnecting to WiFi");
    // Serial.println("");
    WiFi.begin(ssid, password);
  }

  old_pin_state = pin_state;
  delay(500);
}
