# 💨 THE AIRFLOW PROJECT - ESP32-S3 Smart Fan Controller

An ultra-compact (40x40 mm), intelligent 4-channel fan controller built for PC modding, SFF (Small Form Factor) builds, and smart home integration. Designed with a focus on industrial-grade hardware protection. 

No proprietary desktop software required: just plug it in, connect to Wi-Fi, and take control immediately via Home Assistant or with your own Arduino code.

<img src="/Images/Hardware/Photo1.png" alt="Arduino step 1" width="800"/>

---

## ✨ Features at a Glance

* **The Brain:** Powered by a high-performance **ESP32-S3 Zero/Mini** (or optional ESP32C3 Zero/Mini or ESP32C6 Zero/Mini (NOT SuperMini)). Provides massive computing power for complex PID controllers, Wi-Fi, and Bluetooth.
* **4x Independent Channels:** Full PWM control and true RPM feedback (tacho signal) for up to four 12V fans.
* **Global Zero-RPM Mode:** Basic controllers often rely on a 0% PWM signal, which many fans ignore by dropping to a minimum idle speed. By acting as a master high-side switch, the eFuse allows the ESP32 to physically cut the 12V power to the entire fan rail at once – ensuring absolute 0dB silence for your whole setup when you don't need cooling.
* **Premium Power Delivery:** Instead of relying on cheap, heat-generating linear regulators (LDOs), this board features a high-end, fully encapsulated K7805-1000R3 switching regulator. This highly efficient (up to 96%) step-down converter guarantees a perfectly stable and icy-cool power supply for the ESP32-S3, ensuring rock-solid 24/7 reliability without any thermal issues. 
* **Overbuilt Power Delivery:** To ensure absolute thermal stability, the main 12V power traces are routed at a massive 1.9 mm width. Even running four heavy-duty fans at 100% capacity 24/7, the board remains completely cold, for a maximum lifespan of components.
* **Standardized Connectors:** Equipped with genuine Molex 4-pin headers (47053-1000) featuring friction locks. Fully compatible with all standard 4-pin PWM and 3-pin PC fans out of the box – no proprietary adapters required.
* **4x M2 Mounting Holes:** Despite the ultra-compact 40x40 mm footprint, the PCB features four precisely placed M2 mounting holes in the corners. Perfectly prepared for custom 3D-printed enclosures, brass standoffs, or direct integration into your custom PC or SFF chassis. Say goodbye to messy double-sided tape!
* **True Right to Repair (Self-Documenting PCB):** I believe hardware should be repairable. Instead of hiding component identities or forcing you to hunt through digital BOMs, the PCB silkscreen is exhaustively labeled. Every single component features its designator and, where applicable, its exact value printed right next to the pads. If you ever need to probe, modify, or repair the board, the PCB itself is your physical schematic.
* **One More Thing:** [Maybe there is another possible function with a bit of Tinkering](OneMoreThing.md)

## 🧰 The "Hacker Header" (DIY Expansion Port)

While the controller is designed to work perfectly right out of the box, I wanted to leave the door wide open for maker, hardware hackers and modders. 

The board features an exposed male pin header that grants direct access to the ESP32's remaining resources:
* **Power Delivery:** Dedicated **5V, 3.3V, and GND** pins. You can power your external modules directly through the board's premium onboard voltage regulator.
* **5x Free GPIOs:** Fully routable pins ready for your custom physical interfaces.
* **Limitless Expansion:** Perfect for adding physical rotary encoders for manual speed tuning, tactile buttons, I2C OLED displays, or custom environmental/air quality sensors to trigger your fans automatically.
* **⚠️ Firmware Note:** Please be aware that this expansion header is provided purely as a hardware playground for tinkerers. These extra pins are *not* implemented in the stock web-interface firmware. They are waiting for you to unleash them via your own custom ESPHome configurations or C++ code!

## 🧠 Modular "Bring Your Own Brain" Architecture

This board features a **socketed MCU design** based on the popular "Zero" form factor. You own the hardware, and you decide what runs on it.

* **The Default Powerhouse:** The board is designed around the **ESP32-S3 Zero**, providing massive computing power, Wi-Fi, and Bluetooth 5 (LE) for complex local web interfaces and advanced PID fan control.
* **Drop-In Upgrades:** The socket is fully pin-compatible with other microcontrollers in the same form factor, such as the **ESP32-C3 Zero** or **ESP32-C6 Zero**.
* **Next-Gen Smart Home Ready:** By simply swapping the default MCU for an ESP32-C6 Zero, your hardware immediately gains support for **IEEE 802.15.4** wireless standards. This unlocks the physical ability to integrate the controller directly into modern power saving mesh networks!

## 🌊 Advanced PWM Signal Integrity (True 5V Push-Pull)

The original Intel 4-pin fan specification from the early 2000s relies on an open-drain architecture: the controller pulls the PWM signal to Ground and lets it "float" back up, relying entirely on the fan's internal pull-up resistor. While adequate for basic setups, this floating approach often results in degraded, sloppy signal waves—especially when using longer cable runs.

To guarantee flawless motor control, this board utilizes a dedicated **CD74HCT125M96** logic buffer to deliver a strict **5V Push-Pull** PWM signal. 

* **Perfect Square Waves:** Instead of floating, the IC actively drives the signal high (5V) and aggressively pulls it low (GND). This ensures perfectly sharp signal edges and totally eliminates RPM fluctuations or motor clicking.
* **Modern Industry Standards:** This robust, active signaling approach is designed in accordance with modern high-end cooling standards, perfectly satisfying the strict electrical requirements detailed in [Noctua's PWM Specifications White Paper](https://cdn.noctua.at/media/Noctua_PWM_specifications_white_paper.pdf).

## 🔌 Flexible Power Input Options

No two projects are the same. Whether you are building a standalone desk fan, upgrading a 3D printer, or wiring a custom server rack, you can choose the 12V power input connector that fits your needs:

* **DC Barrel Jack (DC005) [Default]:** The classic 5.5x2.1mm plug. Perfect for standard 12V wall power adapters.
* **Screw Terminal (KF350-3.5-2P) [Optional]:** The tinkerer's choice. Ideal for bare wires, ferrules, and custom industrial setups.
* **JST-VH Latching Header (B2P-VH) [Optional]:** A highly secure, locking connector that won't come loose – perfect for custom PC wire harnesses or environments with vibration.

*Note: The board is engineered with a single-sided SMT layout. By keeping all high-end SMD components on the bottom  layer, production costs at JLCPCB are kept efficient, allowing me to make an industrial-grade, highly protected board at an accessible price.*


## 🛡️ Ultimate Hardware Protection

Most DIY controllers cut costs on protection. This board features a multi-layered, industrial-grade defense system to ensure absolute safety for your fans, the board, and your power supply:

* **Reverse Polarity Protection:** A dedicated AO4407A P-Channel MOSFET ensures that plugging the 12V input in backwards won't fry the board, while maintaining near-zero voltage drop and heat generation.
* **Industrial Protection (eFuse):** A dedicated Texas Instruments TPS1H100-Q1 chip protects your fans, the board traces, and your power supply. It features a strict hardware-level cutoff at exactly **2 Amps** to prevent any damage from shorted fan cables or blocked motors.
* **Dual Overcurrent Protection:** A secondary 1206 Resettable PTC Fuse acts as an ultimate fail-safe backup on the main power line.
* **Clean Power (Freewheeling):** An SS34 Schottky diode safely absorbs destructive inductive kickback (flyback voltage) generated by the fan motor coils during PWM switching.
* **ESD Spikes:** SMF15A TVS and BZT52C15 Zener diodes aggressively clamp down any sudden ESD voltage spikes from your power supply or static discharge.
* **3.3V Logic Clamping:** 1N5819W Schottky diodes on the RPM (tacho) input pins strictly prevent any stray 12V signals from the fans from reaching and destroying the ESP32's sensitive 3.3V logic.


## 📐 Mechanical Dimensions & Clearance

The board is designed for compact SFF (Small Form Factor) environments. Please note the following dimensions for your enclosure design:

* **Board Size (Outline):** 40 x 40 mm
   * The USB-C port of the ESP32 Zero extends approximately 1 mm beyond the PCB edge.
* **PCB Thickness:** 1.6 mm
* **Mounting Holes:** 4x M2 holes, spaced at 35.4 x 35.4 mm (measured from center to center).
* **Component Height (Front/Top):** 
    * The tallest component is ~18 mm.
    * *Note:* Please ensure extra vertical clearance to account for the height of the Molex 4-pin fan connectors (Fan side).
* **Component Height (Back/Bottom):** Maximum component profile is ~3 mm. Ensure adequate standoff distance (e.g., using 5mm standoffs) if mounting on a flat metal surface to avoid short circuits.

---

## 🚀 Quick Start

You have two ways to get the board up and running:

### Option A: ESPHome / Home Assistant SOON!
Perfect for smart home enthusiasts. Simply flash our pre-configured `.yaml` file to the board, and your fans will automatically appear in Home Assistant.<br>
👉 [Get the esphome.yaml here](/Firmware/ESPHome)

### Option B: The Arduino way
Use the code example to understand the logic and write your own code to get exactly what you need for your special demand.<br>
👉 [Arduino Examples](/Firmware/Arduino)

---

## 🔌 Hardware & Pinout

| Board PIN | ESP32S3 PIN| ESP32C3 PIN| ESP32C6 PIN|
| --------- |:-------------:|:-------------:|:------------:|
| Enable    | 44     |SOON|SOON|
| FAN0      | 13     |SOON|SOON|
| RPM0      | 11     |SOON|SOON|
| FAN1      | 12     |SOON|SOON|
| RPM1      | 10     |SOON|SOON|
| FAN2      | 2      |SOON|SOON|
| RPM2      | 9      |SOON|SOON|
| FAN3      | 1      |SOON|SOON|
| RPM3      | 8      |SOON|SOON|
| RGB LED   | 21     |SOON|SOON|
| H3 PIN6   | 6      |SOON|SOON|
| H3 PIN7   | 7      |SOON|SOON|
| H4 PIN5   | 5      |SOON|SOON|
| H4 PIN4   | 4      |SOON|SOON|
| H4 PIN3   | 3      |SOON|SOON|

You can also use a ESP32C6 XIAO. You must mount it upside down (5v, GND, 3.3V Pins must match the board labeled 5v, GND, 3.3v). And you dont have the full features (because of the missing pins obviously)

### The Solder Jumper (Optional Pin vs. Current Sense)
To maximize the limited GPIOs of the ESP32-S3, the board features a solder jumper (J1):
* **Position "Default":** H3 Pin7 is one of five Pins for optional hardware (Rotary encode, Button, Sensors).
* **Position "CS" (bridged):** Sacrifices the H3 Pin7, but connects the ESP32 GPIO7 directly to the analog current-sense output of the eFuse for live telemetry and software-based short-circuit detection. IMPORTANT do not connect anything to H3 Pin7 anymore.

### ⚠️ Power Supply Safety Note
Please use a high quality 12v 4a Power supply.
The board is equipped with a strict **2.5A (FAN CURRENT) hardware limit**. This is more than enough for four standard 120/140mm PC fans running at full speed. Do **not** connect water cooling pumps (like a D5), as their high startup current will immediately trigger the eFuse's 2.5A limit.

### Troubleshooting
Is something not working? Take a look here
👉 [Troubleshooting](Troubleshooting)

**!!! IMORTANT: UNMOUNT THE ESP32 Before USING USB !!!**
If you are using the USB from the mounted ESP32 there could be a Backfeeding. That means in this moment your USB will power up the whole pcb and that could possibly overload your USB Port or damages the ESP32 Hardware. It is strictly recommended to physically unmount the ESP32 from the PCB before using the USB for flashing. A OTA update function could be a good solution for this. I am working on that for future revisions to fix this behaivior. For newer boards (Rev 2.1) at least disconnect the 12v Power supply.

---

**If you have any questions or feedback, feel free to open an issue!**