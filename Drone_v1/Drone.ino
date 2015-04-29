#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <Servo.h>

// ---------- Drone variables ---------
#define CE_PIN 9
#define CSN_PIN 10

const uint64_t pipe = 0xE8E8F0F0E1LL;
const int motorPins[] = {3, 5, 6, A0};

RF24 radio(CE_PIN, CSN_PIN);

Servo motors[4];

int throttle[4];

// DEBUG
int startDelay = 110;
int startCount = 0;
int throttleDelay = 10;
int throttleCount = 0;

// Throttle variables
int inputThrottle = 90;
int globalThrottle = 0;
int maxThrottleDerivation = 40;

// Timekeeping
unsigned long lastCommandTime;
unsigned long commandTimeout = 7000;

boolean stationLost = false;

void droneSetup() {
  Serial.println("Nrf24L01 Receiver Starting");
  
  for (int i = 0; i < 4; i++) {
    motors[i].attach(motorPins[i]);
    motors[i].write(0);
  }
  
  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();
}

void droneLoop() {
    while (radio.available() > 0) {
    bool done = false;
    
    while (!done) {
      done = radio.read(throttle, sizeof(throttle));
      stationLost = false;
      
      for (int i = 0; i < 4; i++) {
        
        if (throttle[i] == 0) {
          motors[i].write(0);
        } else if (throttle[i] > 0 && throttle[i] < 180) {
          motors[i].write(throttle[i]);
        }
      }      
      lastCommandTime = millis();
    }
  }
  
  
  // DEBUG
  if (startCount < startDelay) {
    startCount++;
  } else {
    if (globalThrottle == 0) {
      globalThrottle = 60;
    }
    if (throttleCount < throttleDelay) {
      if (globalThrottle < inputThrottle) {
        globalThrottle++;
        throttleCount = 0;
      }
    } else {
      throttleCount++;
    }
  }
  
  float orientation[] = {yaw, pitch, roll};
  
  Serial.print(TO_DEG(orientation[2]));
  Serial.print(" ");
  Serial.print(gyro[2]);
  Serial.print(" ");
  
  float throttlePitch = throttleCalculation(orientation[1], gyro[1]);
  float throttleRoll = throttleCalculation(orientation[2], gyro[0]);
  
  Serial.print(throttleRoll);
  Serial.print(" ");
  
  throttle[0] = globalThrottle + throttlePitch - throttleRoll;
  throttle[1] = globalThrottle + throttlePitch + throttleRoll;
  
  throttle[2] = globalThrottle - throttlePitch + throttleRoll;
  throttle[3] = globalThrottle - throttlePitch - throttleRoll;
  
  for (int i = 0; i < 4; i++) {
    if (throttle[i] < 60) {
      throttle[i] = 60;
    } else if (throttle[i] > 130) {
      throttle[i] = 130;
    }
  }
  
    
  // DEBUG
  if (startCount >= startDelay) {
    Serial.print(throttle[0]);
    Serial.print(" ");
    Serial.print(throttle[1]);
    Serial.print(" ");
    Serial.print(throttle[2]);
    Serial.print(" ");
    Serial.print(throttle[3]);
  
    for (int i = 0; i < 4; i++) {
      motors[i].write(throttle[i]);
    }
  }
  
  /*
  Serial.print(throttle[0]);
  Serial.print(" ");
  Serial.print(throttle[1]);
  Serial.print(" ");
  Serial.print(throttle[2]);
  Serial.print(" ");
  Serial.print(throttle[3]);
  */
  
  Serial.println("");
  
  unsigned long currentTime = millis();
  if (currentTime - lastCommandTime >= commandTimeout) {
    baseStationLost();
  }
}

void baseStationLost() {
  if (!stationLost) {
    Serial.println("Basestation lost!");
    for (int i = 0; i < 4; i++) {
      motors[i].write(0);
    }
    
    stationLost = true;
  }
}

float throttleCalculation(float angle, float angleSpeed) {
  float output;
  
  if (angle > 0) {
    output = angle * maxThrottleDerivation + (float)(angleSpeed/3000) * maxThrottleDerivation;
  } else {
    output = angle * maxThrottleDerivation + (float)(angleSpeed/3000) * maxThrottleDerivation;
  }
  
  return output;
}
