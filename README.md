# Smart Pen Air Mouse & Presentation Controller (ESP32)

A **Bluetooth-based Smart Pen** that works as an **Air Mouse and Presentation Controller**, designed for teaching, presentations, and human–computer interaction applications.  
The device uses **hand motion** to control the cursor and **physical buttons** for mouse clicks and slide navigation.

---

## Features

-  Air Mouse – Control cursor movement using hand tilt  
-  Presentation Control – Next / Previous slide using buttons  
-  Dual Mode – Mouse Mode & Presentation Mode  
-  Rechargeable Battery with safe charging  
-  Bluetooth Status LED  
-  Auto Sleep Mode for power saving  
-  Auto Bluetooth Reconnect  
-  Pen-style portable design  

---

## Working Principle

1. The **MPU6050** detects hand movement using accelerometer data.  
2. The **ESP32** processes motion data and converts it into mouse movement.  
3. Mouse commands are transmitted wirelessly using **Bluetooth HID**.  
4. Push buttons are used for mouse clicks and slide navigation.  
5. A charging module safely manages the Li-ion battery.

---

## Hardware Components

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

##  Power System
```text
18650 Battery
   ↓
TP4056 Charging Module
   ↓
ON / OFF Switch
   ↓
ESP32 VIN
```

- Charging via TP4056 Micro-USB
- Built-in charging & full-charge indication
- Auto sleep reduces power consumption

---

 # Software & Libraries
- Arduino IDE
- ESP32 Board Package
- Libraries: ESP32 BLE Mouse (by T-vK), MPU6050, Wire

---

Buttons:
- Button 1 → Left Click / Next Slide
- Button 2 → Right Click / Previous Slide

---

# Applications
- Classroom teaching
- Technical presentations
- Smart classrooms

---
