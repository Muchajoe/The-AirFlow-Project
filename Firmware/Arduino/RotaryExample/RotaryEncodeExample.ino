#include <Arduino.h>
#include <esp32-hal-ledc.h>

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

const int pwm_frequency = 25000; 
const int pwm_resolution = 8; 

const int pinA = 3; 
const int pinB = 4; 


volatile long encoderValue = 0;
volatile unsigned long lastInterruptTime = 0;


volatile bool encoderMoved = true;

volatile unsigned long lastMoveTime = 0;
volatile bool needsFadeOut = false;


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


void setup() {
  Serial.begin(115200);

  rgbLedWrite(led_pin, 0, 0, 0);
  pinMode(enable_pin, OUTPUT); 

  ledcAttach(fan0_pwm_pin, pwm_frequency, pwm_resolution); 
  ledcAttach(fan1_pwm_pin, pwm_frequency, pwm_resolution);
  ledcAttach(fan2_pwm_pin, pwm_frequency, pwm_resolution);
  ledcAttach(fan3_pwm_pin, pwm_frequency, pwm_resolution);

  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinA), isr_encoder, CHANGE);

  xTaskCreate(fadeOutTask, "FadeTask", 2048, NULL, 1, NULL);
}


void loop() {
  long currentValue;
  bool moved;


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

  delay(50); 
}