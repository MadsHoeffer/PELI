#define Button 21
#define LED 33
// RPI TEAM4:
const char* ssid ="Team4";
const char* password = "12345678";
WiFiClient wifiClient;

const char* udpAddress = "team4.wlan";
const int udpPort = 44444;
WiFiUDP udp;

PubSubClient client(wifiClient);

// RPI team4:
const char* mqttBroker = "team4.wlan";  //PC
const char* mqttUser = "Team4";      // MQTT User Authentification
const char* mqttPass = "Team4";  // MQTT Password Authentification


const char* mqttClientName = "esp32";
const char* audio_alert = "audio/alert";

// Audio defines
const int analogInPin = A3;
const int audio_buffer_size = 4000;
uint8_t audio_buffer_a[audio_buffer_size];
uint8_t audio_buffer_b[audio_buffer_size];
volatile uint32_t audio_buffer_cnt_a = 0;
volatile uint32_t audio_buffer_cnt_b = 0;
bool audio_buffer_full_a = 0;
bool audio_buffer_full_b = 0;
bool use_buffer_a = 1;
uint16_t adc_val = 0;

// Timer configuration
hw_timer_t* Timer0_Cfg = NULL;
const int cpu_clock = 240000000;
const int timer_clock = 80000000;
const int timer_freq = 1000000;
const int timer_prescale = timer_clock / timer_freq;
const int interrupt_freq = 10000;
const int timer_ticks = timer_clock / (interrupt_freq * timer_prescale);


bool button_pressed_once = false;
bool button_pressed_twice = false;
int first_press_time = 0;
int second_press_time = 0;
int button_press_threshold = 1000;
bool rec_start_sent = false;
bool rec_stop_sent = false;
const char* audio_alert_start = "Start";
const char* audio_alert_stop = "Stop";
bool mqtt_res;
