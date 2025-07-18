# The-AirFlow-Project

The AirFlowS3 mini is a compact and versatile control board specifically designed for precise control of up to four 12V fans using PWM. Featuring an ESP32S3 zero microcontroller, it offers flexible control options via an intuitive web user interface or a rotary encoder. A key feature is the integrated complete disconnection of the 12V supply to the fans at 0% PWM, implemented via a Solid State Relay (SSR), to save energy and minimize noise when the fans are not needed.

Features & Highlights
Four PWM-Controlled 12V Fan Outputs: Enables precise control over fan speed.

Intelligent 12V Shutdown: At 0% PWM, the 12V supply to the fans is completely disconnected via an LT218D SSR including a 390 Ohm series resistor.

RPM Signal Detection: Each fan connector (4-pin FAN Header) includes a 10 kOhm pull-up resistor for detecting the RPM signal.

PWM Level Shifting: A CD74HCT125M96 ensures correct PWM signal level adaptation.

Flexible Control:

Web User Interface: Allows for convenient configuration and control over a network.

Rotary Encoder: A dedicated 5-pin header enables direct connection of a rotary encoder for manual control.

Expansion Possibilities: A 4-pin header is available for connecting additional accessories, such as temperature sensors.

Efficient Power Supply:

K7805-1000R3 Fixed Voltage Regulator: Provides a stable 5V supply for the ESP32S3 zero.

Versatile 12V Input Options: The board can be powered by 12V via a DC005 jack, KF350-3.5-2P terminals, or B2P-VH-B(LF)(SN) connector.

Safety Features: A 1206L300SL16ARG fuse and an SS34 flyback diode protect the circuitry.

Extremely Compact Design: Measuring only 40mm x 40mm, the AirFlowS3 mini is ideal for projects with limited space.

Cost-Optimized: The design avoids vias to keep manufacturing costs low.

Mounting: Hole spacing of 35.4mm with 2mm diameter for easy mounting.

Use Cases
The AirFlowS3 mini is perfect for projects requiring intelligent and efficient fan control, such as:

PC Case Fan Control

3D Printer Cooling

Small Server Racks or Enclosures

Home Automation Projects

Cooling Modules for Electronic Devices
