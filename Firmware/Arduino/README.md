# Example Overview
## The Quick and Simple
Is a easy code to make it move. It simply starts the Fans at 100% and waits 10 seconds. Then it set the PWM signal to 0. Some Fans will stop spinning here. Others will continue spinning at the lowest possible speed of the fan. After 1ß seconds the PCB will cut the 12V Power supply to the FANs. That will stop every FAN.

## Rotary Example
In this example you need a rotary encoder. By Spin the rotary you can set a fan speed. At Zero PWM the logic will cut the 12v automaticly. It also uses the RGB onboard led to indicate the state. Red means that the maximum is arrived. Green means the minimum is arrived. Blue is all inbetween.

## Rotary and WebUI Example (proof of concept)
That includes the Rotary example and adds a WebUi. After flashing this you should be able to connect to a wlan called **"The Airflow Project"** with a password of **"123456789"**. It will also read out the RPM and it will also try to readout the Wattage (that only works if you have bridged the J1 Jumper on the PCB). If you want to use a rotary encoder:
| Encoder Pin | Airflow Pin| 
| --------- |:-------------:|
| GND    | GND     |
| + (3.3 or VIN)    | 3.3V     |
| SW    | 3     |
| DT    | 4     |
| CLK   | 5     |

<img src="/Images/Arduino_Tutorial/example3.png" alt="Arduino step 1" width="800"/>


## Arduino Quickstart
1. You need the Arduino IDE. Open The board Manager (left side). Type in ESP and Download the ESP32 Core.
<img src="/Images/Arduino_Tutorial/Arduino_step_1.png" alt="Arduino step 1" width="800"/>

2. Fot the Rotary and WebUI example you need to install the Websockets Library by Markus Sattler You will find it by typing in Websockets in the library manager
<img src="/Images/Arduino_Tutorial/Arduino_step_2.png" alt="Arduino step 2" width="800"/>

3. Choose your ESP32 board. I like to use one of the pre configured ESP32S3 Zero or Mini variant. And Enable USB CDC to activate the Serial Monitor funxionallity. And choose the right com port.
<img src="/Images/Arduino_Tutorial/Arduino_step_3.png" alt="Arduino step 3" width="800"/>

4. Copy one of the source code of the examples and click on compile and upload botton.
<img src="/Images/Arduino_Tutorial/Arduino_step_4.png" alt="Arduino step 4" width="800"/>