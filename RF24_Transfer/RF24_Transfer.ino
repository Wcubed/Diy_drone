#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 9
#define CSN_PIN 10
#define throttle_X A0
#define throttle_Y A1

// Pinconfig
//
// <-- Antenna
//
// x    12
// 11   13
// 10   9
// 3.3  GND
//

const uint64_t pipe = 0xE8E8F0F0E1LL;

RF24 radio(CE_PIN, CSN_PIN);
int throttle[1];

unsigned long delayTime = 100;
unsigned long lastTime;

void setup()
{
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(pipe);
  
  throttle[0] = 0;
}

void loop()
{
  while (Serial.available() > 0) {
    String message = Serial.readStringUntil('\n');
    
    if (message != NULL) {
      if (message.substring(0, 2) == "00") {
        throttle[0] = 0;
        Serial.println("Killswitch");
      } else {
        int b = message.toInt();
        
        Serial.println(b);
        
        if (b >= 0 && b <= 130) {
          throttle[0] = b;    
        }
      }
      radio.write( throttle, sizeof(throttle) );
    }
  }
  
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= delayTime) {
    Serial.println("Send");
    radio.write( throttle, sizeof(throttle) );
    lastTime = currentTime;
  }
}

