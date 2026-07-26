//THIS IS THE SIMPLE EXAMPLE A VERY QUICK AND DIRTY START
//1. IT STARTS WITH A HARD ALL FAN TO THE MAX START (ALL FANS 100%) AS A TOURTURE TEST (INRUSH CURRENT) THEN WAIT 10 SECONDS
//2. SET THE FANS TO ZERO AND WAIT 10 SECONDS TO SEE IF YOUR FANS HAVE FAN STOP AND FOR POWER MEASURING
//3. CUT THE 12V POWER SUPPLY TO THE FANS TO MEASURE AGAIN WAIT 10 SECONDS AND REPEAT EVERYTJOMG
// !!! THIS ARE THE ESP32S3 ZERO PINS. IF YOU USE A C3 OR C6 YOU HAVE TO CHANGE THE PIN NUMBERS

#include <esp32-hal-ledc.h>
#include <Arduino.h>

const int enable_pin = 44;        // SSR PIN (to activate/cut the 12v fan power supply)
const int fan0_pwm_pin = 13;      // FAN PWM PINS ESP32S3
const int fan1_pwm_pin = 12; 
const int fan2_pwm_pin = 2;  
const int fan3_pwm_pin = 1;  

const int pwm_frequency = 25000; // SET THE PWM FREQUENCY TO RECOMMENDED 25KHZ
const int pwm_resolution = 8;    // SET PWM RESOLUTION


void setup() {

  pinMode(enable_pin, OUTPUT); // Set the SSR Relay pin to output
  ledcAttach(fan0_pwm_pin, pwm_frequency, pwm_resolution); // set the resolution and frequency to the GPIO (Fan PWM) output pin
  ledcAttach(fan1_pwm_pin, pwm_frequency, pwm_resolution);
  ledcAttach(fan2_pwm_pin, pwm_frequency, pwm_resolution);
  ledcAttach(fan3_pwm_pin, pwm_frequency, pwm_resolution);
}

void loop() {

    digitalWrite(enable_pin, HIGH); // Must be HIGH for 12v FAN power supply and LOW to cut the 12v fan power supply
    ledcWrite(fan0_pwm_pin, 255); // set the fans speed to max
    ledcWrite(fan1_pwm_pin, 255);
    ledcWrite(fan2_pwm_pin, 255);
    ledcWrite(fan3_pwm_pin, 255);

    delay (10000); //wait 10 seconds
  
    ledcWrite(fan0_pwm_pin, 0); // set the fan speed to zero
    ledcWrite(fan1_pwm_pin, 0);
    ledcWrite(fan2_pwm_pin, 0);
    ledcWrite(fan3_pwm_pin, 0);

    delay (10000); //wait 10 seconds

    digitalWrite(enable_pin, LOW); // cut the 12v Power

    delay (10000); //wait 10 seconds
}

