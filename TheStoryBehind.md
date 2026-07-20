### The Story Behind the Board

It all started during an extremely hot summer when I desperately needed a fan. While standard fans were fine during the day, the nights were a different story. I am highly sensitive to noise, and the constant humming made it impossible to sleep. I happened to have four spare Noctua fans lying around, which are famous for their whisper-quiet operation and precise speed control.

At the time, I was heavily experimenting with Arduinos and had only just begun exploring the ESP32. Knowing that the fans required a 5V PWM signal and that the Arduino operated on a 5V logic level, I simply hooked it up. It worked even though, looking back, I probably wasn't hitting the exact required PWM frequency out of that Arduino. But it worked anyways

### The Catalyst: 3D Printing

Fast forward a few years. Noctua released their Airflow Amplifier (NV-AA1-12) and generously provided the 3D files online. This instantly brought my old summer project back into focus. I took the files, scaled the model up to fit my 140mm fan, and printed it.

However, I was not a fan of the original control setup, which required a external box sitting on my desk. I wanted an elegant, integrated solution.

### The First Custom PCB

I set out to design a custom circuit board small enough to fit perfectly into the center hub of the 3D-printed airflow amplifier. My vision was ambitious:

* Direct control via a physical rotary knob right on the device.
* Smart connectivity to control the fan via smartphone.
* Maximum flexibility regarding power supply options.
* Ultra-low power dissipation. After all, the goal was to cool the room, not to generate unnecessary extra heat through inefficient voltage converters.

This became the very first custom PCB I ever designed. I placed the order, and when the boards finally arrived, I quickly realized there were two major design flaws. Fortunately, I managed to get them running for my own use with a few creative workarounds.

### Evolution and Refinement

That first board sparked a steep learning curve. Over time, I kept iterating on the design, learning new things, and adding more functionality. The board grew in complexity, and features kept being added until quite literally every single millimeter of PCB space was utilized.

Today, this project has evolved into an extremely dense PCB that finally meets every single one of my initial requirements. It has been an incredible learning journey, and I am just thrilled to finally share the result with the community, hoping others will find it just as useful for their own setups.