#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "defines.h"
#include "mqtt.h"

// Timer interrupt for ADC sampling
void IRAM_ATTR Timer0_ISR() { 
  if (audio_buffer_full_a != 1 && use_buffer_a == 1) {
    adc_val = (analogRead(analogInPin));
    audio_buffer_a[audio_buffer_cnt_a] = highByte(adc_val);
    audio_buffer_cnt_a++;
    audio_buffer_a[audio_buffer_cnt_a] = lowByte(adc_val);
    audio_buffer_cnt_a++;

  } else if (audio_buffer_full_b != 1 && use_buffer_a == 0) {
    adc_val = (analogRead(analogInPin)); 
    audio_buffer_b[audio_buffer_cnt_b] = highByte(adc_val);
    audio_buffer_cnt_b++;
    audio_buffer_b[audio_buffer_cnt_b] = lowByte(adc_val);
    audio_buffer_cnt_b++;
  }
  if (audio_buffer_cnt_a >= audio_buffer_size) {
    audio_buffer_full_a = 1;
  }
  if (audio_buffer_cnt_b >= audio_buffer_size) {
    audio_buffer_full_b = 1;
  }
}

// Button interrupt with debounce
void IRAM_ATTR isr_button() { 
  if (button_pressed_once != true && millis() - second_press_time > button_press_threshold) {
    rec_start_sent = false;
    button_pressed_once = true;
    first_press_time = millis();
    digitalWrite(LED, 1);
  } else if (button_pressed_once == true && millis() - first_press_time > button_press_threshold) {
    rec_stop_sent = false;
    second_press_time = millis();
    button_pressed_once = false;
    button_pressed_twice = true;
    digitalWrite(LED, 0);
  }
}

void setup() {
  setCpuFrequencyMhz(cpu_clock / 1000000);
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(Button, INPUT_PULLDOWN);
  pinMode(LED, OUTPUT);
  attachInterrupt(Button, isr_button, RISING);
  delay(1000);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  client.setServer(mqttBroker, 1883);

  // Timer interrupt to read ADC channel
  Timer0_Cfg = timerBegin(0, timer_prescale, true);  //
  timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, false);
  timerAlarmWrite(Timer0_Cfg, timer_ticks, true);
  timerAlarmDisable(Timer0_Cfg);

}

void loop() {
  // Connect to mqtt if not connected
  if (!client.connected()) {
    mqttConnect();
  }
  timerAlarmEnable(Timer0_Cfg);

  // Sending start and stop over mqtt when button pressed once and twice
  if (button_pressed_once && !rec_start_sent) {
    mqtt_res = client.publish(audio_alert, audio_alert_start);
    if (mqtt_res) {
      rec_start_sent = true;
    }
  }
  if (button_pressed_twice && !rec_stop_sent) {
    mqtt_res = client.publish(audio_alert, audio_alert_stop);
    if (mqtt_res) {
      button_pressed_twice = false;
      rec_stop_sent = true;
    }
  }
  // Transmit buffer a or buffer b if it is full
  if (audio_buffer_full_a == 1) {
    use_buffer_a = 0;
    udp.beginPacket(udpAddress, udpPort);
    udp.write(audio_buffer_a, audio_buffer_size);
    udp.endPacket();
    audio_buffer_full_a = 0;
    audio_buffer_cnt_a = 0;
  } else if (audio_buffer_full_b == 1) {
    use_buffer_a = 1;
    udp.beginPacket(udpAddress, udpPort);
    udp.write(audio_buffer_b, audio_buffer_size);
    udp.endPacket();
    audio_buffer_full_b = 0;
    audio_buffer_cnt_b = 0;
  }
}
