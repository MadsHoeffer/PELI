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

WiFiClient    wifiClient;
MqttClient    mqttClient(wifiClient);

void setup() {
  Serial.begin(115200);
  //delay(500);
  //Serial.println("");
  //Serial.print("Connecting to ");
  //Serial.println(ssid);

  WiFi.begin(ssid, password);
  //delay(500);
  // int wifi_connect_attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
    //wifi_connect_attempts++;
    // This is just for robustness
    //if (wifi_connect_attempts > 10) {
      //Serial.println("");
      //Serial.println("Retry WiFi connect");
      //WiFi.begin(ssid, password);
      //wifi_connect_attempts = 0;
    //}
  }

  // Serial.println("");
  // Serial.print("WiFi connected - IP address: ");
  // Serial.println(WiFi.localIP());
  // Serial.println("");

  mqttClient.setUsernamePassword(mqtt_username, mqtt_pwd);

  // Serial.println("Connecting to Mqtt");
  while (!mqttClient.connect(mqtt_broker, mqtt_port)) {
    // Serial.print(".");
  }
  // Serial.println("Connected to MQTT broker!");
  // Serial.println();

  pin_state = digitalRead(SENSOR_PIN);

  // Serial.println("Publishing on /door_sensor topic");
  // Serial.print("Message: ");

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

  // Serial.print("Enabling gpio wakeup on pin state: ");
  // Serial.println(!pin_state);
  if (pin_state) {
    esp_deep_sleep_enable_gpio_wakeup(bit(D2), ESP_GPIO_WAKEUP_GPIO_LOW);
  }
  else {
    esp_deep_sleep_enable_gpio_wakeup(bit(D2), ESP_GPIO_WAKEUP_GPIO_HIGH);
  }
  // Serial.println("");

  // Serial.print("Pin state: ");
  // Serial.println(pin_state);
  // Serial.println("Disconnect WiFi before going to sleep");
  WiFi.disconnect();

  // Serial.print("Going to sleep now");
  // Serial.println("");
  esp_deep_sleep_start();
  // Serial.print("You won't see this");
  
}

void loop() {
  // put your main code here, to run repeatedly:
}
