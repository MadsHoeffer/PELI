void mqttConnect() {

  while (!client.connected()) {

    // Serial.print("MQTT Connecting\n");

    if (client.connect(mqttClientName, mqttUser, mqttPass)) {

      // Serial.println("connected");
      // client.publish("hello", mqttClientName);
    } else {
      // Serial.print("failed, rc=");
      // Serial.print(client.state());
      // Serial.println(" try again in 2 seconds");
      delay(1);
    }
  }
}