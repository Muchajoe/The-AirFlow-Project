#include <Arduino.h>
#include <esp32-hal-ledc.h>
#include <driver/pulse_cnt.h>
#include "driver/gpio.h"
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h> // You have to install the Websockets lib by Markus Sattler (ver 2.7.2) https://github.com/Links2004/arduinoWebSockets

const char* ap_ssid = "The Airflow Project";
const char* ap_pass = "TheAirFlowSetup";
bool new_wlan_data_recieved = false;
String pendingSSID = "";
String pendingPass = "";
const unsigned long WIFI_TIMEOUT_MS = 10000;
extern const char page_index[] PROGMEM;
extern const char page_config[] PROGMEM;

const int led_pin = 21;         
const int max_brightness = 50;  

volatile int current_r = 0;
volatile int current_g = 0;
volatile int current_b = 0;

const int enable_pin = 44;   
const int fan0_pwm_pin = 13; 
const int fan1_pwm_pin = 12; 
const int fan2_pwm_pin = 2;  
const int fan3_pwm_pin = 1;

const int fan_rpm_pins[4] = {11, 10, 9, 8};
volatile int current_rpm[4] = {0, 0, 0, 0};
pcnt_unit_handle_t pcnt_units[4];

const int pwm_frequency = 25000; 
const int pwm_resolution = 8; 

const int pinA = 5; 
const int pinB = 4; 

volatile long encoderValue = 0;
volatile long currentValue;
volatile bool moved;
volatile unsigned long lastInterruptTime = 0;

volatile bool encoderMoved = true; 

volatile unsigned long lastMoveTime = 0;
volatile bool needsFadeOut = false;

unsigned long lastUpdate = 0;


const int CS_PIN = 7;             
const float R_CS = 330.0;         
const float K_CS = 1000.0;          
const float SYSTEM_VOLTAGE = 12.0;
const float I_LIMIT = 2.466;      
float zero_offset_mv = 0.0;


WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void IRAM_ATTR isr_encoder() {
  unsigned long interruptTime = micros();
  if (interruptTime - lastInterruptTime > 2000) { 
    if (digitalRead(pinA) != digitalRead(pinB)) {
      if (encoderValue < 102) encoderValue += 2; 
    } else {
      if (encoderValue > -2) encoderValue -= 2;   
    }
    
    encoderMoved = true; 
  }
  lastInterruptTime = interruptTime;
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Client disconnected\n", num);
      break;
    case WStype_CONNECTED:
      Serial.printf("[%u] Client connected!\n", num);
      {
        String initMsg = "{\"slider\":" + String(encoderValue) + "}";
        webSocket.sendTXT(num, initMsg);
      }
      break;
    case WStype_TEXT:
      String msg = (char*)payload;
      Serial.printf("[%u] Message recieved: %s\n", num, msg.c_str());
      
      if (msg.indexOf("\"slider\":") > 0) {
        int index = msg.indexOf(":");
        String valStr = msg.substring(index + 1, msg.length() - 1);
        valStr.replace("\"", "");
        int sliderVal = valStr.toInt();

        Serial.print("New  Slider-Value recieved: ");
        Serial.println(sliderVal);

        encoderValue = sliderVal;
        encoderMoved = true; 
      }
      break;
  }
}

void fadeOutTask(void *parameter) {
  while (1) {
    if (needsFadeOut && (millis() - lastMoveTime > 1000)) {
      
      for (int i = max_brightness; i >= 0; i--) {
        if (millis() - lastMoveTime < 1000) {
          break;
        }
        
        int r = (current_r > 0) ? i : 0;
        int g = (current_g > 0) ? i : 0;
        int b = (current_b > 0) ? i : 0;
        
        rgbLedWrite(led_pin, r, g, b);
        vTaskDelay(15 / portTICK_PERIOD_MS); 
      }
      
      if (millis() - lastMoveTime > 1000) {
        needsFadeOut = false; 
        rgbLedWrite(led_pin, 0, 0, 0);
      }
    }
    vTaskDelay(50 / portTICK_PERIOD_MS); 
  }
}

void task_read_rpm(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(1000);

  while (true) {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    for (int i = 0; i < 4; i++) {
      int pulse_count = 0;
      
      pcnt_unit_get_count(pcnt_units[i], &pulse_count);
      pcnt_unit_clear_count(pcnt_units[i]);

      current_rpm[i] = pulse_count * 30;
    }
  }
}

void WlanConnect();
void WlanDelete();
void WlanAdd(String InputSSID, String InputPasswort);
void WebRoutes();
void init_fan_pcnt();
float AnalogReadOut();
float calibrateZero(int pin);

void setup() {
  pinMode(enable_pin, OUTPUT); 
  digitalWrite(enable_pin, LOW);

  Serial.begin(115200);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  delay(1000);
  zero_offset_mv = calibrateZero(CS_PIN);
  delay(1000);
  
  WlanConnect();
  WebRoutes();
  init_fan_pcnt();

  rgbLedWrite(led_pin, 0, 0, 0);

  ledcAttach(fan0_pwm_pin, pwm_frequency, pwm_resolution); 
  ledcAttach(fan1_pwm_pin, pwm_frequency, pwm_resolution);
  ledcAttach(fan2_pwm_pin, pwm_frequency, pwm_resolution);
  ledcAttach(fan3_pwm_pin, pwm_frequency, pwm_resolution);

  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinA), isr_encoder, CHANGE);

  xTaskCreate(fadeOutTask, "FadeTask", 2048, NULL, 1, NULL);
  xTaskCreatePinnedToCore(task_read_rpm, "RPM_Task",2048, NULL, 1, NULL, 0);

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  if (encoderMoved) {

    int currentValue = encoderValue;
    if (currentValue < 0) currentValue = 0;
    if (currentValue > 100) currentValue = 100;

    encoderValue = currentValue; 


    String jsonString = "{\"slider\":" + String(currentValue) + "}";
    

    webSocket.broadcastTXT(jsonString);
    
    Serial.print("Encoder changed to: ");
    Serial.println(currentValue);
  }

  noInterrupts();
  currentValue = encoderValue;
  moved = encoderMoved;
  encoderMoved = false;
  interrupts();

  if (moved) {
    
    lastMoveTime = millis();
    needsFadeOut = true;    

    int pwm_signal = map(currentValue, 0, 100, 0, 255);
    pwm_signal = constrain(pwm_signal, 0, 255);

    ledcWrite(fan0_pwm_pin, pwm_signal);
    ledcWrite(fan1_pwm_pin, pwm_signal);
    ledcWrite(fan2_pwm_pin, pwm_signal);
    ledcWrite(fan3_pwm_pin, pwm_signal);

    if (currentValue >= 100) { 
      current_r = max_brightness; current_g = 0; current_b = 0;
    } else if (currentValue <= 0) { 
      current_r = 0; current_g = max_brightness; current_b = 0;
    } else { 
      current_r = 0; current_g = 0; current_b = max_brightness;
    }
    
    rgbLedWrite(led_pin, current_r, current_g, current_b);
      
    if (pwm_signal == 0) {
      digitalWrite(enable_pin, LOW);
    } else {
      digitalWrite(enable_pin, HIGH);
    }

    Serial.print("Position: ");
    Serial.println(currentValue);
    Serial.println(pwm_signal);

  }

if (new_wlan_data_recieved) {
    new_wlan_data_recieved = false;
    delay(2000); 
    WlanAdd(pendingSSID, pendingPass);
  }

  if (millis() - lastUpdate > 1000) {
    lastUpdate = millis();

    unsigned long upSeconds = millis() / 1000;
    String uptimeStr = String(upSeconds / 60) + "m " + String(upSeconds % 60) + "s";

    String jsonString = "{";
    jsonString += "\"v1\":\"" + String(current_rpm[0]) + "\",";
    jsonString += "\"v2\":\"" + String(current_rpm[1]) + "\",";
    jsonString += "\"v3\":\"" + String(current_rpm[2]) + "\",";
    jsonString += "\"v4\":\"" + String(current_rpm[3]) + "\",";
    jsonString += "\"v5\":\"" + String(AnalogReadOut()) + "\",";
    jsonString += "\"up\":\"" + uptimeStr + "\"";
    jsonString += "}";

    webSocket.broadcastTXT(jsonString);
  }

  server.handleClient();
  webSocket.loop();
  delay(10); 
}

void WlanConnect(){
  WiFi.mode(WIFI_STA);
  String savedSSID = WiFi.SSID();
  
  if (savedSSID.length() > 0) {
    Serial.print("NVS-Data Found. Try to connect: ");
    Serial.println(savedSSID);
    WiFi.begin(); 
    
    unsigned long startAttemptTime = millis();
    
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS) {
      Serial.print(".");
      delay(500);
    }
    Serial.println();
  } else {
    Serial.println("NVS is empty: No Wi-Fi Credentials found.");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✅ WIFI CONNECTED");
    Serial.print("IP-Adress: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("❌ WIFI NOT CONNECTED");
    Serial.println("Start Fallback: Access Point (AP)");

    delay(100);
    WiFi.disconnect(true, false);
    delay(100);
    WiFi.mode(WIFI_AP);
    delay(100);
    
    if (WiFi.softAP(ap_ssid, ap_pass)) {
      Serial.println("✅ Access Point started");
      Serial.print("AP SSID: ");
      Serial.println(ap_ssid);
      Serial.print("AP IP: ");
      Serial.println(WiFi.softAPIP());
    } 
  }
}

void WlanDelete(){
delay(2000);
WiFi.disconnect(true, true);
delay(500);
ESP.restart();

}

void WlanAdd(String InputSSID, String InputPasswort) {
    Serial.println("\n--- Change WLAN mode ---");
    
    WiFi.mode(WIFI_STA);
    delay(100);
    WiFi.disconnect(true, false);
    delay(100);
    
    Serial.print("Connect to WLAN: ");
    Serial.println(InputSSID);
    

    WiFi.begin(InputSSID.c_str(), InputPasswort.c_str());
    
    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 20) {
        delay(500);
        Serial.print(".");
        retries++;
    }
    Serial.println();
    

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("✅ WIFI CONNECTED");
        delay(1000);
        
        Serial.println("Restart ESP");
        ESP.restart();
    } else {
        Serial.println("❌ WIFI NOT CONNECTED");
        Serial.println("Restart AP");
        WiFi.mode(WIFI_AP);

    }
}

void WebRoutes() {
server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", page_index);
  });

  server.on("/config", HTTP_GET, []() {
    server.send_P(200, "text/html", page_config);
  });

  server.on("/freset", HTTP_GET, []() {
    server.send(200, "text/plain", "Factory reset");
    WlanDelete();
  });

  server.onNotFound([]() {
    server.send(404, "text/plain", "404: NOT FOUND");
  });

  server.on("/save", HTTP_POST, []() {
    if (!server.hasArg("ssid") || !server.hasArg("password")) {
      server.send(400, "text/plain", "SSID or Password");
      return;
    }

    pendingSSID = server.arg("ssid");
    pendingPass = server.arg("password");

    Serial.println("New WLAN-Data recieved:");
    Serial.print("SSID: ");
    Serial.println(pendingSSID);
    Serial.print("Password: ");
    Serial.println(pendingPass);

    String htmlResponse = "<!DOCTYPE HTML><html><head><meta charset=\"UTF-8\"></head>";
    htmlResponse += "<body><h1>Saved!!</h1><p>THE ESP RESARTS AND RECONNECTS NOW</p></body></html>";
    server.send(200, "text/html", htmlResponse);

    new_wlan_data_recieved = true;
  });

  server.begin();
  Serial.println("Webserver started");
}

void init_fan_pcnt() {
  for (int i = 0; i < 4; i++) {
    pcnt_unit_config_t unit_config = {};
    unit_config.low_limit = -1; 
    unit_config.high_limit = 32767; 
    
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_units[i]));

    pcnt_glitch_filter_config_t filter_config = {};
    filter_config.max_glitch_ns = 1000;
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_units[i], &filter_config));

    pcnt_chan_config_t chan_config = {};
    chan_config.edge_gpio_num = fan_rpm_pins[i];
    chan_config.level_gpio_num = -1;
    
    pcnt_channel_handle_t pcnt_chan = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_units[i], &chan_config, &pcnt_chan));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan, 
                    PCNT_CHANNEL_EDGE_ACTION_HOLD,      
                    PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    gpio_set_pull_mode((gpio_num_t)fan_rpm_pins[i], GPIO_PULLUP_ONLY);

    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_units[i]));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_units[i]));
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_units[i]));
  }
}

float AnalogReadOut(){
  uint32_t sum_mv = 0;
  const int NUM_SAMPLES = 10;
  
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum_mv += analogReadMilliVolts(CS_PIN);
    delay(2);
  }
  
  float raw_mv = (sum_mv / (float)NUM_SAMPLES);

  float corrected_mv = raw_mv - zero_offset_mv;
  
  if (corrected_mv < 0.0) {
    corrected_mv = 0.0;
  }
  
  float v_cs = corrected_mv / 1000.0;
  float current_A = (v_cs / R_CS) * K_CS;
  float power_W = current_A * SYSTEM_VOLTAGE;

  if (current_A >= (I_LIMIT - 0.05)) {
    Serial.println("⚠️ CURRENT LIMIT ACTICE");
  }

  return power_W;
}

float calibrateZero(int pin) {
  Serial.print("Calibrate zero");
  uint32_t sum = 0;
  
  for (int i = 0; i < 50; i++) {
    sum += analogReadMilliVolts(pin);
    delay(5); 
  }
  
  float offset = sum / 50.0;
  Serial.printf(" Done %.1f mV\n", offset);
  return offset;
}


const char page_index[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Dashboard</title>
    <style>
        body { font-family: Arial, Helvetica, sans-serif; padding: 20px; max-width: 600px; margin: 0 auto; color: #333; }
        h1 { text-align: center; }
        
        /* Slider Styling */
        .slider-card { background: #f4f4f4; padding: 20px; border-radius: 8px; margin-bottom: 25px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }
        .slider-label { display: flex; justify-content: space-between; font-weight: bold; margin-bottom: 10px; }
        input[type=range] { width: 100%; }
        
        /* Grid */
        .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(130px, 1fr)); gap: 15px; margin-bottom: 25px; }
        .value-box { background: #f4f4f4; padding: 15px; border-radius: 8px; text-align: center; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }
        .value-box .label { font-size: 14px; color: #666; margin-bottom: 5px; }
        .value-box .value { font-size: 22px; font-weight: bold; color: #007bff; }
        
        /* Button */
        .btn { display: block; text-align: center; background-color: #6c757d; color: white; padding: 12px; text-decoration: none; border-radius: 4px; font-size: 16px; }
        .btn:hover { background-color: #5a6268; }
    </style>
</head>
<body>
    <h1>System Dashboard</h1>
    
    <div class="slider-card">
        <div class="slider-label">
            <span>PWM Control</span>
            <span id="sliderVal">50 %</span>
        </div>
        <input type="range" min="0" max="100" value="50" id="pwmSlider">
    </div>

    <div class="grid">
        <div class="value-box">
            <div class="label">RPM FAN 0</div>
            <div class="value" id="val1">--</div>
        </div>
        <div class="value-box">
            <div class="label">RPM FAN 1</div>
            <div class="value" id="val2">--</div>
        </div>
        <div class="value-box">
            <div class="label">RPM FAN 2</div>
            <div class="value" id="val3">--</div>
        </div>
        <div class="value-box">
            <div class="label">RPM FAN 3</div>
            <div class="value" id="val4">--</div>
        </div>
        <div class="value-box">
            <div class="label">Watt</div>
            <div class="value" id="val5">--</div>
        </div>
        <div class="value-box">
            <div class="label">Uptime</div>
            <div class="value" id="uptime">0s</div>
        </div>
    </div>

    <a href="/config" class="btn">WLAN configuration</a>

    <script>
        // WebSocket Port 81 (Standard ESP32 WebSockets)
        var gateway = `ws://${window.location.hostname}:81/`;
        var websocket;
        
        window.addEventListener('load', initWebSocket);

        function initWebSocket() {
            console.log('Connect  WebSocket...');
            websocket = new WebSocket(gateway);
            websocket.onopen    = onOpen;
            websocket.onclose   = onClose;
            websocket.onmessage = onMessage;
        }

        function onOpen(event) {
            console.log('WebSocket connected');
        }

        function onClose(event) {
            console.log('WebSocket disconnected. Retry in 2 sec');
            setTimeout(initWebSocket, 2000); // Auto reconnect
        }

        function onMessage(event) {
            // Expected format: {"v1":"23.5", "v2":"40", "v3":"12", "v4":"8.5", "v5":"99", "up":"12m 30s"}
            try {
                var data = JSON.parse(event.data);
                if(data.v1 !== undefined) document.getElementById('val1').innerText = data.v1;
                if(data.v2 !== undefined) document.getElementById('val2').innerText = data.v2;
                if(data.v3 !== undefined) document.getElementById('val3').innerText = data.v3;
                if(data.v4 !== undefined) document.getElementById('val4').innerText = data.v4;
                if(data.v5 !== undefined) document.getElementById('val5').innerText = data.v5;
                if(data.up !== undefined) document.getElementById('uptime').innerText = data.up;
                if(data.slider !== undefined) {
                    document.getElementById('pwmSlider').value = data.slider;
                    document.getElementById('sliderVal').innerText = data.slider + ' %';
                }
            } catch (e) {
                console.error("Failed to parse the WebSocket-Data:", e);
            }
        }

        var slider = document.getElementById('pwmSlider');
        var sliderText = document.getElementById('sliderVal');
        
        slider.addEventListener('input', function() {
            sliderText.innerText = this.value + ' %';
            if (websocket.readyState === WebSocket.OPEN) {
                // Sends JSON to ESP32: {"slider": 75}
                websocket.send(JSON.stringify({slider: this.value}));
            }
        });
    </script>
</body>
</html>
)rawliteral";

const char page_config[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WLAN Setup</title>
    <style>
        body { font-family: Arial, Helvetica, sans-serif; padding: 20px; max-width: 400px; margin: 0 auto; }
        h1 { color: #333; }
        .form-group { margin-bottom: 15px; }
        label { display: block; margin-bottom: 5px; font-weight: bold; }
        input { width: 100%; padding: 10px; box-sizing: border-box; border: 1px solid #ccc; border-radius: 4px; }
        button { width: 100%; padding: 12px; background-color: #007bff; color: white; border: none; border-radius: 4px; font-size: 16px; cursor: pointer; margin-top: 10px; }
        button:hover { background-color: #0056b3; }
        .back-link { display: inline-block; margin-top: 20px; text-decoration: none; color: #007bff; }
    </style>
</head>
<body>
    <h1>WLAN Setup</h1>
    
    <!-- POST to route /save -->
    <form action="/save" method="POST">
        <div class="form-group">
            <label for="ssid">SSID (WLAN-Name):</label>
            <input type="text" id="ssid" name="ssid" placeholder="My Network" required>
        </div>
        
        <div class="form-group">
            <label for="password">Password:</label>
            <input type="password" id="password" name="password" placeholder="********" required>
        </div>
        
        <button type="submit">Save & Connect</button>
    </form>
    
    <a href="/" class="back-link">&laquo; Back to home</a><br><br><br>
    <a href="/freset" class="back-link">&laquo; Factory Reset/a>
</body>
</html>
)rawliteral";