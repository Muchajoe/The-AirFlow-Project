#include <ESP32Encoder.h>
#include <Adafruit_NeoPixel.h>
#include <driver/pcnt.h>
#include <esp32-hal-ledc.h>
#include <esp_system.h>
#include <rom/ets_sys.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <LittleFS.h>

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>


//WIFI AP Mode, HTTP GET REQ API, WEB UI, LittleFS config save, WIFI STA Mode, WIFI Credentials

//PINS
int encoder_pin0 = 7; //3
int encoder_pin1 = 6; //4
int encoder_switch = 5;
int led_pin = 21;         // GPIO 21 for ESP32-S3 Zero
int num_leds = 1;         // 1 LED
int enable_pin = 44;      // GPIO SSR PIN (TX-Pin) if not working try io 43,14,15 or wire and use IO10
int fan0_pwm_pin = 13;
int fan0_rpm_pin = 11;
//int fan0_channel = 1;
pcnt_unit_t fan0_pcnt_unit = PCNT_UNIT_0;
int fan1_pwm_pin = 12;
int fan1_rpm_pin = 10;
//int fan1_channel = 2;
int fan2_pwm_pin = 2;
int fan2_rpm_pin = 9;
//int fan2_channel = 3;
int fan3_pwm_pin = 1;
int fan3_rpm_pin = 8;
//int fan3_channel = 4;

int pwm_frequency = 25000;
int pwm_resolution = 8;

ESP32Encoder encoder;
Adafruit_NeoPixel led(num_leds, led_pin, NEO_GRB + NEO_KHZ800);

int fan0_pwm = 0;
int16_t fan0_rpm = 0;
bool fan_0 = true;

int fan1_pwm = 0;
int16_t fan1_rpm = 0;
bool fan_1 = true;

int fan2_pwm = 0;
int16_t fan2_rpm = 0;
bool fan_2 = true;

int fan3_pwm = 0;
int16_t fan3_rpm = 0;
bool fan_3 = true;

int mode = 4; // Mode counter
int value = 0;
int64_t pos = 0;

int old_encoder_value = 102;
int new_encoder_value;

String ap_password = "123456789";
String ap_ssid = "AirFlow";
String ap_ip;
String sta_password;
String sta_ssid;
String sta_ip;

unsigned long lastTime;

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

const int wdtTimeout = 6000;  // Watchdog time in ms to trigger the watchdog
hw_timer_t *timer = NULL;     // Watchdog

void webRoutes();
void sendLiveDataWS();
void init_pcnt();

void ARDUINO_ISR_ATTR resetModule() { // Watchdog
  ets_printf("reboot\n");
  esp_restart();
}

void setup() {
  Serial.begin(9600);

  timer = timerBegin(1000000);                     // Watchdog timer 1Mhz resolution
  timerAttachInterrupt(timer, &resetModule);       // Watchdog attach callback
  timerAlarm(timer, wdtTimeout * 1000, false, 0);  // Watchdog set time in us

  if (!LittleFS.begin()) {
    Serial.println("Failed to start LittleFS");
    return;
  }

  pinMode(enable_pin, OUTPUT);
  ledcAttach(fan0_pwm_pin, pwm_frequency, pwm_resolution);
  ledcAttach(fan1_pwm_pin, pwm_frequency, pwm_resolution);
  ledcAttach(fan2_pwm_pin, pwm_frequency, pwm_resolution);
  ledcAttach(fan3_pwm_pin, pwm_frequency, pwm_resolution);

  WiFi.softAP(ap_ssid, ap_password);
  webRoutes();
  server.begin();
  init_pcnt();

  encoder.attachHalfQuad(encoder_pin0, encoder_pin1);
  encoder.setCount(50);
}

void loop() {
  timerWrite(timer, 0);  //reset timer (feed watchdog)
  unsigned long currentTime = millis();

  pos = encoder.getCount();
  new_encoder_value = pos;
  if (new_encoder_value != old_encoder_value){
    value = new_encoder_value;
    old_encoder_value = new_encoder_value;
    mode = 4;
    }


  if (currentTime - lastTime >= 2000) {  // intervall 30000
      lastTime = currentTime;

  pcnt_counter_pause(PCNT_UNIT_1);
  //pcnt_counter_pause(PCNT_UNIT_2);
  //pcnt_counter_pause(PCNT_UNIT_3);
  pcnt_get_counter_value(PCNT_UNIT_1, &fan0_rpm);
  //pcnt_get_counter_value(PCNT_UNIT_2, &fan1_rpm);
  //pcnt_get_counter_value(PCNT_UNIT_3, &fan2_rpm);
  pcnt_counter_clear(PCNT_UNIT_1);
    //pcnt_counter_clear(PCNT_UNIT_2);
    //pcnt_counter_clear(PCNT_UNIT_3);
  pcnt_counter_resume(PCNT_UNIT_1);
    //pcnt_counter_resume(PCNT_UNIT_2);
    //pcnt_counter_resume(PCNT_UNIT_3);
    }
    fan0_rpm = (fan0_rpm / 2.0) * 30.0;
    fan1_rpm = (fan1_rpm / 2.0) * 30.0;
    fan2_rpm = (fan2_rpm / 2.0) * 30.0;

  if (pos < 1) {
    encoder.setCount(0);
    //value = 0;
    neopixelWrite(led_pin, 0, 0, 50);
    }

  else if (pos > 99) {
    encoder.setCount(100);
    //value = 100;
    neopixelWrite(led_pin, 50, 0, 0);
    }

  else {
    neopixelWrite(led_pin, 0, 50, 0);
    }

  if (mode == 4) { // mode 4 changes all fans
    fan0_pwm = map(value, 0, 100, 0, 255);
    fan1_pwm = map(value, 0, 100, 0, 255);
    fan2_pwm = map(value, 0, 100, 0, 255);
    fan3_pwm = map(value, 0, 100, 0, 255);
  }

  if (fan0_pwm == 0 && fan1_pwm == 0 && fan2_pwm == 0 && fan3_pwm == 0) { // turns of the voltage via SSR if all fans are set to 0
    digitalWrite(enable_pin, LOW);
    Serial.println(" EN PIN LOW");
  }
  else {
    digitalWrite(enable_pin, HIGH);
    Serial.println(" EN PIN HIGH");

    ledcWrite(fan0_pwm_pin, fan0_pwm);
    ledcWrite(fan1_pwm_pin, fan1_pwm);
    ledcWrite(fan2_pwm_pin, fan2_pwm);
    ledcWrite(fan3_pwm_pin, fan3_pwm);
  }

  Serial.print(pos);
  Serial.print(" - ");
  Serial.print(value);
  Serial.print(" - ");
  Serial.print(fan0_pwm);
  Serial.print(" - ");
  Serial.print(fan1_pwm);
  Serial.print(" - ");
  Serial.print(fan2_pwm);
  Serial.print(" - ");
  Serial.print(fan3_pwm);
  Serial.print(" - ");
  Serial.print(fan0_rpm);
  Serial.print(" - ");
  Serial.print(mode);

  server.handleClient();
}

void webRoutes() {

  server.on("/", HTTP_GET, []() {
    File file = LittleFS.open("/index.html", "r");
    if (!file) {
      server.send(404, "text/plain", "index not found");
      return;
    }
    server.streamFile(file, "text/html");
    file.close();
  });

  server.on("/static/bootstrap.bundle.min.js", HTTP_GET, []() {
    //File file = LittleFS.open("/static/bootstrap.bundle.min.js", "r");
    //if (!file) {
    //  server.send(404, "text/plain", "bootstrapjs not found");
    //  return;
    //}
    //server.streamFile(file, "text/html");
    //file.close();
  });

  server.on("/static/bootstrap.min.css", HTTP_GET, []() {
    //File file = LittleFS.open("/static/bootstrap.min.css", "r");
    //if (!file) {
    //  server.send(404, "text/plain", "css not found");
    //  return;
    //}
    //server.streamFile(file, "text/css");
    //file.close();
  });

    server.on("/set", HTTP_GET, []() {
    if (server.hasArg("value")) {
      //value = server.arg("value").toInt();
      encoder.setCount(server.arg("value").toInt());
      mode = 4;
    }
    if (server.hasArg("fan0")) {
      fan0_pwm = server.arg("fan0").toInt();
      fan0_pwm = map(fan0_pwm, 0, 100, 0, 255);
      mode = 3;
    }
    if (server.hasArg("fan1")) {
      fan1_pwm = server.arg("fan1").toInt();
      fan1_pwm = map(fan1_pwm, 0, 100, 0, 255);
      mode = 3;
    }
    if (server.hasArg("fan2")) {
      fan2_pwm = server.arg("fan2").toInt();
      fan2_pwm = map(fan2_pwm, 0, 100, 0, 255);
      mode = 3;
    }
    if (server.hasArg("fan3")) {
      fan3_pwm = server.arg("fan3").toInt();
      fan3_pwm = map(fan3_pwm, 0, 100, 0, 255);
      mode = 3;
    }
    server.send(200, "text/plain", "Parameters updated successfully!");
    });
  }

void init_pcnt() {

    pcnt_config_t pcnt_config_0 = {
        .pulse_gpio_num = 11,
        .ctrl_gpio_num = -1,
        .lctrl_mode = PCNT_MODE_KEEP,  // Control mode when control signal is low
        .hctrl_mode = PCNT_MODE_KEEP,  // Control mode when control signal is high
        .pos_mode = PCNT_COUNT_INC,  // Count up on the positive edge
        .neg_mode = PCNT_COUNT_DIS,  // INC, DIS, KEEP
        .counter_h_lim = 1024,  // Maximum count value
        .counter_l_lim = -1024, // Minimum count value
        .unit = PCNT_UNIT_1 ,  // PCNT unit
        .channel = PCNT_CHANNEL_0
    };

    pcnt_unit_config(&pcnt_config_0);     // Initialize PCNT unit
    pcnt_set_filter_value(PCNT_UNIT_1 , 10);
    pcnt_filter_enable(PCNT_UNIT_1);
    pcnt_event_enable(PCNT_UNIT_1 , PCNT_EVT_H_LIM);
    //pcnt_event_enable(PCNT_UNIT, PCNT_EVT_L_LIM);
    pcnt_event_enable(PCNT_UNIT_1 , PCNT_EVT_ZERO);
    pcnt_intr_enable(PCNT_UNIT_1);
    pcnt_counter_pause(PCNT_UNIT_1);
    pcnt_counter_clear(PCNT_UNIT_1);
    pcnt_counter_resume(PCNT_UNIT_1);

}
