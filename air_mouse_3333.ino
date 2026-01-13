#include <Wire.h>
#include <MPU6050.h>
#include <BleMouse.h>

MPU6050 mpu;
BleMouse bleMouse("Smart Pen Air Mouse", "ESP32", 100);

// ---------------- PINS ----------------
#define LED_RED     18
#define LED_GREEN   17
#define LED_CALIB   16

#define BTN_LEFT    25    // Button 1: Left Click + Wake
#define BTN_RIGHT   26    // Button 2: Right Click + Wake
#define BTN_SCROLL  27    // Button 3: Scroll Mode Toggle
#define BTN_ZOOM    33    // Button 4: Zoom Mode Toggle (NEW)

#define BAT_ADC     34

// ---------------- VARIABLES ----------------
float roll = 0, pitch = 0;
float rollAcc, pitchAcc;
float dt;

unsigned long lastTime = 0;
unsigned long lastActivityTime = 0;

// Gyro offsets
int16_t gx_off = 0, gy_off = 0;

// Cursor control
bool cursorLocked = false;

// Mode control
enum MouseMode { MODE_NORMAL, MODE_SCROLL, MODE_ZOOM };
MouseMode currentMode = MODE_NORMAL;

unsigned long scrollPressTime = 0;
unsigned long zoomPressTime = 0;

// Sleep mode
bool sleepMode = false;
#define SLEEP_TIMEOUT 600000  // 10 minutes in milliseconds

// Flick gesture (ESC)
unsigned long lastFlickTime = 0;
int flickCount = 0;

#define FLICK_THRESHOLD 1200
#define FLICK_TIMEOUT   350
#define FLICK_REQUIRED  3

// Battery
unsigned long lastBatteryCheck = 0;
bool lowBatteryBlink = false;

// Button debounce
#define DEBOUNCE_DELAY 50
unsigned long lastDebounceTime[4] = {0, 0, 0, 0};
bool buttonState[4] = {HIGH, HIGH, HIGH, HIGH};
bool lastButtonState[4] = {HIGH, HIGH, HIGH, HIGH};

// ---------------- BATTERY FUNCTIONS ----------------
float readBatteryVoltage() {
  int raw = analogRead(BAT_ADC);
  return (raw / 4095.0) * 3.3 * 2.0;
}

int batteryPercentage(float v) {
  if (v >= 4.2) return 100;
  if (v <= 3.3) return 0;
  return (int)((v - 3.3) / (4.2 - 3.3) * 100);
}

void handleBatteryLED() {
  if (millis() - lastBatteryCheck < 5000) return;
  lastBatteryCheck = millis();

  int percent = batteryPercentage(readBatteryVoltage());

  if (percent <= 5) {
    lowBatteryBlink = !lowBatteryBlink;
    digitalWrite(LED_RED, lowBatteryBlink);
    digitalWrite(LED_GREEN, LOW);
  }
  else if (percent >= 10) {
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED, LOW);
  }
  else {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, LOW);
  }
}

// ---------------- SLEEP FUNCTIONS ----------------
void enterSleepMode() {
  sleepMode = true;
  
  // Turn off LEDs
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_CALIB, LOW);
  
  // Disconnect BLE
  bleMouse.end();
  
  // Configure wake-up buttons
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_25, LOW); // BTN_LEFT
  esp_sleep_enable_ext1_wakeup(1ULL << BTN_RIGHT, ESP_EXT1_WAKEUP_ANY_LOW); // BTN_RIGHT
  
  Serial.println("Entering sleep mode...");
  delay(100);
  
  // Enter light sleep (preserves RAM)
  esp_light_sleep_start();
  
  // Wake up here
  wakeFromSleep();
}

void wakeFromSleep() {
  sleepMode = false;
  lastActivityTime = millis();
  
  // Blink green LED to indicate wake
  digitalWrite(LED_GREEN, HIGH);
  delay(500);
  digitalWrite(LED_GREEN, LOW);
  
  // Restart BLE
  bleMouse.begin();
  
  Serial.println("Woke from sleep!");
}

// ---------------- GYRO CALIBRATION ----------------
void calibrateGyro() {
  digitalWrite(LED_CALIB, HIGH);
  long sx = 0, sy = 0;

  for (int i = 0; i < 500; i++) {
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    sx += gx;
    sy += gy;
    delay(5);
  }

  gx_off = sx / 500;
  gy_off = sy / 500;

  delay(2000);
  digitalWrite(LED_CALIB, LOW);
}

// ---------------- MULTI-FLICK ESC ----------------
void detectMultiFlickESC(int gy) {
  if (abs(gy) > FLICK_THRESHOLD) {
    unsigned long now = millis();

    if (now - lastFlickTime < FLICK_TIMEOUT)
      flickCount++;
    else
      flickCount = 1;

    lastFlickTime = now;

    if (flickCount >= FLICK_REQUIRED) {
      bleMouse.press(KEY_ESC);
      delay(50);
      bleMouse.release(KEY_ESC);
      flickCount = 0;
      lastActivityTime = millis();
    }
  }
}

// ---------------- DEBOUNCED BUTTON READ ----------------
bool readDebouncedButton(int pin, int index) {
  bool reading = digitalRead(pin);
  
  if (reading != lastButtonState[index]) {
    lastDebounceTime[index] = millis();
  }
  
  if ((millis() - lastDebounceTime[index]) > DEBOUNCE_DELAY) {
    if (reading != buttonState[index]) {
      buttonState[index] = reading;
    }
  }
  
  lastButtonState[index] = reading;
  return !buttonState[index]; // Return true when pressed (active LOW)
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);
  delay(1500);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_CALIB, OUTPUT);

  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_SCROLL, INPUT_PULLUP);
  pinMode(BTN_ZOOM, INPUT_PULLUP);

  digitalWrite(LED_RED, HIGH);

  Wire.begin(21, 22);
  mpu.initialize();
  calibrateGyro();

  lastTime = millis();
  lastActivityTime = millis();
  bleMouse.begin();
  
  Serial.println("Air Mouse Ready!");
}

// ---------------- LOOP ----------------
void loop() {

  // Check for sleep timeout
  if (millis() - lastActivityTime > SLEEP_TIMEOUT) {
    enterSleepMode();
    return;
  }

  handleBatteryLED();

  static bool blePrev = false;
  if (!bleMouse.isConnected()) {
    blePrev = false;
    return;
  }

  if (!blePrev) {
    digitalWrite(LED_GREEN, HIGH);
    delay(2000);
    digitalWrite(LED_GREEN, LOW);
    blePrev = true;
  }

  unsigned long now = millis();
  dt = (now - lastTime) / 1000.0;
  lastTime = now;

  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  gx -= gx_off;
  gy -= gy_off;

  rollAcc  = atan2(ay, az) * RAD_TO_DEG;
  pitchAcc = atan2(-ax, sqrt(ay * ay + az * az)) * RAD_TO_DEG;

  roll  = 0.98 * (roll + gx * dt / 131.0) + 0.02 * rollAcc;
  pitch = 0.98 * (pitch + gy * dt / 131.0) + 0.02 * pitchAcc;

  detectMultiFlickESC(gy);

  // Read all buttons with debouncing
  bool btnLeft = readDebouncedButton(BTN_LEFT, 0);
  bool btnRight = readDebouncedButton(BTN_RIGHT, 1);
  bool btnScroll = readDebouncedButton(BTN_SCROLL, 2);
  bool btnZoom = readDebouncedButton(BTN_ZOOM, 3);

  // ---------- BUTTON 3: SCROLL MODE ----------
  if (btnScroll) {
    if (scrollPressTime == 0) {
      scrollPressTime = millis();
    }
  } else {
    if (scrollPressTime > 0) {
      // Short press: Toggle scroll mode
      if (millis() - scrollPressTime < 300) {
        if (currentMode == MODE_SCROLL) {
          currentMode = MODE_NORMAL;
          Serial.println("Normal Mode");
        } else {
          currentMode = MODE_SCROLL;
          Serial.println("Scroll Mode");
        }
        // Quick LED blink for feedback
        digitalWrite(LED_CALIB, HIGH);
        delay(100);
        digitalWrite(LED_CALIB, LOW);
      }
      scrollPressTime = 0;
    }
  }

  // ---------- BUTTON 4: ZOOM MODE ----------
  if (btnZoom) {
    if (zoomPressTime == 0) {
      zoomPressTime = millis();
    }
  } else {
    if (zoomPressTime > 0) {
      // Short press: Toggle zoom mode
      if (millis() - zoomPressTime < 300) {
        if (currentMode == MODE_ZOOM) {
          currentMode = MODE_NORMAL;
          Serial.println("Normal Mode");
        } else {
          currentMode = MODE_ZOOM;
          Serial.println("Zoom Mode");
        }
        // Double LED blink for feedback
        digitalWrite(LED_CALIB, HIGH);
        delay(100);
        digitalWrite(LED_CALIB, LOW);
        delay(100);
        digitalWrite(LED_CALIB, HIGH);
        delay(100);
        digitalWrite(LED_CALIB, LOW);
      }
      zoomPressTime = 0;
    }
  }

  // ---------- MODE-BASED MOVEMENT ----------
  if (currentMode == MODE_SCROLL) {
    // SCROLL MODE
    // Button 1 (Left) = Scroll Up
    // Button 2 (Right) = Scroll Down
    if (btnLeft) {
      bleMouse.move(0, 0, 3);  // Scroll up
      lastActivityTime = millis();
      delay(50);
    }
    if (btnRight) {
      bleMouse.move(0, 0, -3);  // Scroll down
      lastActivityTime = millis();
      delay(50);
    }
    
  } else if (currentMode == MODE_ZOOM) {
    // ZOOM MODE
    // Button 1 (Left) = Zoom In (Ctrl + Scroll Up)
    // Button 2 (Right) = Zoom Out (Ctrl + Scroll Down)
    if (btnLeft) {
      bleMouse.press(KEY_LEFT_CTRL);
      bleMouse.move(0, 0, 3);  // Zoom in
      delay(50);
      bleMouse.release(KEY_LEFT_CTRL);
      lastActivityTime = millis();
      delay(50);
    }
    if (btnRight) {
      bleMouse.press(KEY_LEFT_CTRL);
      bleMouse.move(0, 0, -3);  // Zoom out
      delay(50);
      bleMouse.release(KEY_LEFT_CTRL);
      lastActivityTime = millis();
      delay(50);
    }
    
  } else {
    // NORMAL MODE
    // Regular mouse movement
    int moveX = roll / 4;
    int moveY = -pitch / 4;

    if (abs(moveX) < 1) moveX = 0;
    if (abs(moveY) < 1) moveY = 0;

    if (moveX || moveY) {
      bleMouse.move(moveX, moveY);
      lastActivityTime = millis();
    }

    // Button 1: Left Click
    if (btnLeft) {
      bleMouse.click(MOUSE_LEFT);
      lastActivityTime = millis();
      delay(200);
    }

    // Button 2: Right Click
    if (btnRight) {
      bleMouse.click(MOUSE_RIGHT);
      lastActivityTime = millis();
      delay(200);
    }
  }
}