# Smart Pen Air Mouse & Presentation Controller 

## Intro 
bluetooth air mouse that you control by waving your hand around. built it for presentations cuz clicking through slides like a caveman got old lol

---

# Schematic 
<img width="271" height="293" alt="Image" src="https://github.com/user-attachments/assets/0df15df1-4917-449b-aec2-ad8614fab569" />

---
#Features 

- **air mouse** - wave hand = cursor moves
- **4 buttons** - click, scroll, zoom modes
- **scroll mode** - hold buttons to scroll up/down
- **zoom mode** - zoom in/out with buttons (ctrl + scroll)
- **gesture** - flick 3x up/down = ESC (exits powerpoint)
- **auto-sleep** - sleeps after 10 min, wake with any button
- **battery LEDs** - green = good, red blink = charge me

---
#How the button works:-

**normal mode** (default)
- btn 1: left click
- btn 2: right click
- btn 3: short press → toggle scroll mode
- btn 4: short press → toggle zoom mode

**scroll mode** (LED blinks once)
- btn 1: scroll UP
- btn 2: scroll DOWN

**zoom mode** (LED blinks twice)
- btn 1: zoom IN
- btn 2: zoom OUT

---

## Hardware Components/BOM

| Component | Quantity |
|---------|---------|
| ESP32 (38-pin Dev Board) | 1 |
| MPU6050 Accelerometer + Gyroscope | 1 |
| 18650 Li-Ion Battery (3.7V, 2000mAh) | 1 |
| 18650 Battery Holder | 1 |
| TP4056 Li-ion Charging Module (with protection) | 1 |
| Push Buttons | 4 |
| Slide Switch (ON/OFF) | 1 |
| LED  | 3 |
| 220Ω Resistor | 3 |
| 10kΩ Resistor | 2 |
| Vero Board | 1 |

---

##  Pin Configuration

### MPU6050
| MPU6050 | ESP32 |
|---------|-------|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

### buttons
| button | GPIO | function |
|--------|------|----------|
| btn 1 | 25 | left click / scroll up / zoom in |
| btn 2 | 26 | right click / scroll down / zoom out |
| btn 3 | 27 | toggle scroll mode |
| btn 4 | 33 | toggle zoom mode |

### LEDs
| LED | GPIO |
|-----|------|
| red | 18 |
| green | 17 |
| calibration | 16 |

### battery
- GPIO 34 for voltage monitoring
- use voltage divider (2x 10kΩ resistors)

---

Buttons functions:
- Button 1 → Left Click / Next Slide
- Button 2 → Right Click / Previous Slide

---
 # Final thoughts 
 - I was always fascinated by how TV remotes work. One day, I came across this sensor called the MPU6050, and honestly, it felt like finding a hidden treasure. That’s when I started tinkering with it and things just took off from there.

