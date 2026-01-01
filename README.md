# Smart Pen Air Mouse & Presentation Controller (ESP32)

A **Bluetooth-based Smart Pen** that works as an **Air Mouse and Presentation Controller**, designed for teaching, presentations, and human–computer interaction applications.  
The device uses **hand motion** to control the cursor and **physical buttons** for mouse clicks and slide navigation.

---

# Schematic 
<img width="532" height="670" alt="Image" src="https://github.com/user-attachments/assets/ec020224-32df-4547-9763-a3067c70868a" />


## Hardware Components/BOM

| Component | Quantity |
|---------|---------|
| ESP32 (38-pin Dev Board) | 1 |
| MPU6050 Accelerometer + Gyroscope | 1 |
| 18650 Li-Ion Battery (3.7V, 2000mAh) | 1 |
| 18650 Battery Holder | 1 |
| TP4056 Li-ion Charging Module (with protection) | 1 |
| Push Buttons | 2 |
| Slide Switch (ON/OFF) | 1 |
| LED (Bluetooth Status) | 1 |
| 220Ω Resistor | 1 |
| Vero Board | 1 |

---

##  Pin Configuration

### MPU6050 → ESP32
| MPU6050 | ESP32 |
|-------|-------|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

### Buttons
| Function | GPIO |
|--------|------|
| Left Click / Next Slide | GPIO 18 |
| Right Click / Previous Slide | GPIO 19 |

### LED
| Function | GPIO |
|--------|------|
| Bluetooth Status LED | GPIO 25 |

---

Buttons functions:
- Button 1 → Left Click / Next Slide
- Button 2 → Right Click / Previous Slide

---
 # Final thoughts 
 - I was always fascinated by how TV remotes work. One day, I came across this sensor called the MPU6050, and honestly, it felt like finding a hidden treasure. That’s when I started tinkering with it and things just took off from there.

