#include "BluetoothSerial.h"
#include <ESP32Servo.h>

BluetoothSerial SerialBT;
Servo esc;

#define ESC_PIN 13 // CHANGED: ESC pin is now set to 13
#define ESC_STOP 1000
#define ESC_FULL_SPEED 2000 
#define RAMP_DURATION 1000

bool isRunning = false;
unsigned long rampStartTime = 0;

// ADDED: Variable to store the current target speed
int targetThrottle = ESC_FULL_SPEED; 

void setup() {
  Serial.begin(115200);
  SerialBT.begin("BlueToothESP");
  
  esc.setPeriodHertz(50);
  esc.attach(ESC_PIN, 1000, 2000);
  esc.writeMicroseconds(ESC_STOP);
  
  Serial.println("Bluetooth Ready");
  Serial.println("ESC initializing...");
  delay(3000);
  
  Serial.println("ESC Ready");
  Serial.println("F = Forward");
  Serial.println("S = Stop");
}

void loop() {
  if (SerialBT.available()) {
    char cmd = SerialBT.read();
    Serial.print("Received: ");
    Serial.println(cmd);

    switch (cmd) {
      case 'F':
        if (!isRunning) {
          isRunning = true;
          rampStartTime = millis();
          Serial.println("Motor starting... Ramping up.");
        }
        break;
      case 'S':
        stopMotor();
        break;
      case 'B':
        stopMotor();
        Serial.println("Backward disabled");
        break;

      // ADDED: Set motor speed using single characters
      // Maps the 0-100% app slider to the 1000-2000 ESC range
      case '0': targetThrottle = 1000; break;
      case '1': targetThrottle = 1110; break;
      case '2': targetThrottle = 1220; break;
      case '3': targetThrottle = 1330; break;
      case '4': targetThrottle = 1440; break;
      case '5': targetThrottle = 1550; break;
      case '6': targetThrottle = 1660; break;
      case '7': targetThrottle = 1770; break;
      case '8': targetThrottle = 1880; break;
      case '9': targetThrottle = 2000; break;
      case 'q': targetThrottle = 2000; break; // 'q' is often 100% in BT apps

      default:
        break;
    }
  }

  if (!SerialBT.hasClient() && isRunning) {
    Serial.println("Bluetooth disconnected! Stopping motor.");
    stopMotor();
  }

  if (isRunning) {
    unsigned long elapsed = millis() - rampStartTime;
    int currentThrottle;

    if (elapsed < RAMP_DURATION) {
      // ADDED: Ramps up to targetThrottle instead of ESC_FULL_SPEED
      currentThrottle = map(elapsed, 0, RAMP_DURATION, ESC_STOP, targetThrottle);
    } else {
      currentThrottle = targetThrottle;
    }

    esc.writeMicroseconds(currentThrottle);
  }
}

void stopMotor() {
  if (isRunning) {
    Serial.println("STOP");
  }
  isRunning = false;
  esc.writeMicroseconds(ESC_STOP);
}
