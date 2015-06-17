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
int input[3]; // Throttle, x-axis, y-axis.

String message;

unsigned long delayTime = 100;
unsigned long lastTime;

void setup()
{
  Serial.begin(57600);  
  radio.begin();
  radio.openWritingPipe(pipe);
  
  input[0] = 0;
  input[1] = 0;
  input[2] = 0;
}

void loop()
{
  while (Serial.available() > 0) {
    message = Serial.readStringUntil(10);
    
    if (message[0] == 't') {
      message = message.substring(1);
      int b = message.toInt();
      
      if (b == 200) {
        input[0] = 200;
      }
      
      if (b >= 60 && b <= 130) {
        input[0] = b;    
      }
    } else if (message[0] == 'x') {
      message = message.substring(1);
      input[1] = message.toInt();
    } else if (message[0] == 'y') {
      message = message.substring(1);
      input[2] = message.toInt();
    }

    radio.write( input, sizeof(input) );
    Serial.print(input[0]);
    Serial.print(' ');
    Serial.print(input[1]);
    Serial.print(' ');
    Serial.println(input[2]);
  }
  
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= delayTime) {
    Serial.println(input[0]);
    radio.write( input, sizeof(input) );
    lastTime = currentTime;
  }
}

