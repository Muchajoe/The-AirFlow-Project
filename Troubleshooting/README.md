# Troubleshooting

you need a multimeter and your 12v Power supply. Take a look in the Pictures in this folder to find the measuring points. 

1. **Continuity test**
disconnect your power supply and set your multimeter to continuity test mode. One of your measuring tip stays on the red pont. With the other tip you go from one to another of the blue measuring points. It should have continuity.

[Get the measuring points (Continuity test)](/Troubleshooting/continuity_test.png)<br>
<img src="/Troubleshooting/continuity_test.png" alt="isolated" width="800"/>



2. **Voltage test** (do it only if you know what you are doing here)
Set your multimeter to DC voltage test. Connect your 12v Power suply to your PCB. One of your measuring tip stays on the red point. With the other tip you go from one to another of the blue measuring points.

[Get the measuring points (Voltage test)](/Troubleshooting/voltage_test.png)<br>
<img src="/Troubleshooting/voltage_test.png" alt="isolated" width="800"/>

| Point 1 | Point 2| Expected value| Possible failure|
| --------- |:-------------:|:-------------:|:------------:|
| GND      | 1     |12v |Probelm with your power supply|
| GND      | 2     |12v |Problem with your fuse F1|
| GND      | 3     |12v |Problem with Q1 (reverse polarity protection p-channel mosfet)|
| GND      | 4     |5v|Problem with your voltage regulator U3|
| GND      | 5     |5v|Problem with the pcb trace|
| GND      | 6     |5v|Problem with the pcb trace|
| GND      | 7     |3.3v|Problem with your ESP32 voltage regulator|

3. **Efuse Test**
Set your multimeter to DC voltage test. Connect your 12v Power suply to your PCB. Connect 3.3V (From H4 or H3) with a jumperWire to H2 PIN 44. That switches the Power supply for the Fans on. Than you can measure if the 12v reaches the FAN Connectors.

[Connect the Enable pin to 3.3v (eFuse test)](/Troubleshooting/eFuse_test_1.png)<br>
<img src="/Troubleshooting/eFuse_test_1.png" alt="isolated" width="800"/><br>

[Get the measuring points (eFuse test)](/Troubleshooting/eFuse_test_2.png)<br>
<img src="/Troubleshooting/eFuse_test_2.png" alt="isolated" width="800"/>


4. **Continoous overvoltage Case** If you accidentally connected a 16-24V power supply, it is highly likely that only the D6 SMF15A diode has blown.