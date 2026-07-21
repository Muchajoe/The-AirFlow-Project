# Troubleshooting

you need a multimeter and your 12v Power supply. Take a look in the Pictures in this folder to find the measuring points. 

1. Continuity test
disconnect your power supply and set your multimeter to continuity test mode. One of your measuring tip stays on the red pont. With the other tip you go from one to another of the blue measuring points. It should have continuity.
[Get the measuring points (Continuity test)](/continuity_test.png)

2. Voltage test (do it only if you know what you are doing here)
Set your multimeter to DC voltage test. Connect your 12v Power suply to your PCB. One of your measuring tip stays on the red pont. With the other tip you go from one to another of the blue measuring points.
[Get the measuring points (Voltage test)](/continuity_test.png)

| Point 1 | Point 2| Expected value| Possible failure|
| --------- |:-------------:|:-------------:|:------------:|
| GND      | 1     |12v |Probelm with your power supply|
| GND      | 2     |12v |Problem with your fuse F1|
| GND      | 3     |12v |Problem with Q1 (reverse polarity protection p-channel mosfet)|
| GND      | 4     |5v|Problem with your voltage regulator U3|
| GND      | 5     |5v|Problem with the pcb trace|
| GND      | 6     |5v|Problem with the pcb trace|
| GND      | 7     |3.3v|Problem with your ESP32 voltage regulator|