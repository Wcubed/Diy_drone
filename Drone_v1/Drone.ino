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

int inputThrottle[3];
int throttle[4] = {0, 0, 0, 0};

int presets[] = {0, 0, 5, 0};

// DEBUG
int startDelay = 110;
int startCount = 0;
int throttleDelay = 10;
int throttleCount = 0;

// Throttle variables
int globalThrottle = 0;
int maxThrottleDerivation = 20;

// Timekeeping
unsigned long lastCommandTime;
unsigned long commandTimeout = 1000;

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
    boolean done = false;
    
    while (!done) {
      done = radio.read(inputThrottle, sizeof(inputThrottle));
      stationLost = false;
      
      // 200 is kill
      if (inputThrottle[0] == 200) {
        globalThrottle = 0;
      } else if (inputThrottle[0] >= 60 && inputThrottle[0] < 180) {
        globalThrottle = inputThrottle[0];
      }
      
      lastCommandTime = millis();
      Serial.print("Received ");
      Serial.println(globalThrottle);
    }
  }
  
  if (globalThrottle >= 60) {
  
    float orientation[] = {yaw, pitch, roll};
    
    float throttlePitch = throttleCalculation(orientation[1], gyro[1]);
    float throttleRoll = throttleCalculation(orientation[2], gyro[0]);
    
    throttle[0] = presets[0] + globalThrottle + throttlePitch - throttleRoll + inputThrottle[1] - inputThrottle[2];
    throttle[1] = presets[1] + globalThrottle + throttlePitch + throttleRoll + inputThrottle[1] + inputThrottle[2];
    
    throttle[2] = presets[2] + globalThrottle - throttlePitch + throttleRoll - inputThrottle[1] + inputThrottle[2];
    throttle[3] = presets[3] + globalThrottle - throttlePitch - throttleRoll - inputThrottle[1] - inputThrottle[2];
    
    for (int i = 0; i < 4; i++) {
      if (throttle[i] < 60) {
        throttle[i] = 60;
      } else if (throttle[i] > 130) {
        throttle[i] = 130;
      }
    }
    
  } else {
    for (int i = 0; i < 4; i++) {
      throttle[i] = 0;
    }
  }
  
  for (int i = 0; i < 4; i++) {
    motors[i].write(throttle[i]);
  }
    
  // DEBUG
  Serial.print(throttle[0]);
  Serial.print(" ");
  Serial.print(throttle[1]);
  Serial.print(" ");
  Serial.print(throttle[2]);
  Serial.print(" ");
  Serial.print(throttle[3]);
  
  Serial.println("");
  
  unsigned long currentTime = millis();
  if (currentTime - lastCommandTime >= commandTimeout) {
    baseStationLost();
  }
}

void baseStationLost() {
  if (!stationLost) {
    Serial.println("Basestation lost!");
    
    globalThrottle = 0;
    
    stationLost = true;
  }
}

float throttleCalculation(float angle, float angleSpeed) {
  float output;
  angle = radians(int(degrees(angle)));
  
  output = 0.5 * sin(angle) * maxThrottleDerivation + 0.5 * (float)(angleSpeed/3000) * maxThrottleDerivation;
  
  return output;
}
