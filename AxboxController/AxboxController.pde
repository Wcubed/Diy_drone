import processing.opengl.*;
import processing.serial.*;

import procontroll.*;
import net.java.games.input.*;

ControllIO controllIO;
ControllDevice joypad;

ControllStick stick0;
ControllStick stick1;
ControllStick stick2;

float transX;
float transY;

float lastThrottle;
float lastX;
float lastY;

Serial serial;

void setup(){
  size(600,600);
  
  frameRate(100);
  
  println(Serial.list());
  serial = new Serial(this, Serial.list()[0], 57600);

  transX = width/2;
  transY = height/2;

  controllIO = ControllIO.getInstance(this);
  
  controllIO.printDevices();

  joypad = controllIO.getDevice(0);
  joypad.plug(this, "handleButton0Press", ControllIO.ON_PRESS, 0);
  joypad.plug(this, "handleButton0Release", ControllIO.ON_RELEASE, 0);
  joypad.plug(this, "handleButton1Press", ControllIO.ON_PRESS, 1);
  joypad.plug(this, "handleButton1Release", ControllIO.ON_RELEASE, 1);
  joypad.plug(this, "handleButton2Press", ControllIO.ON_PRESS, 2);
  joypad.plug(this, "handleButton2Release", ControllIO.ON_RELEASE, 2);
  joypad.plug(this, "handleButton3Press", ControllIO.ON_PRESS, 3);
  joypad.plug(this, "handleButton3Release", ControllIO.ON_RELEASE, 3);
  
  joypad.plug(this, "handleButtonXPress", ControllIO.ON_PRESS, 8);
  joypad.plug(this, "handleButtonXRelease", ControllIO.ON_RELEASE, 8);
  
  stick0 = joypad.getStick(0);

  stick1 = joypad.getStick(1);

  stick2 = joypad.getStick(2);
}

void handleButton0Press(){ // A
  fill(0,255,0);
}

void handleButton0Release(){ // A
  fill(255);
}

void handleButton1Press(){ // B
  fill(255,0,0);
}

void handleButton1Release(){ // B
  fill(255);
}

void handleButton2Press(){ // X
  fill(0,0,255);
}

void handleButton2Release(){ // X
  fill(255);
}

void handleButton3Press(){ // Y
  fill(255,255,0);
}

void handleButton3Release(){ // Y
  fill(255);
}

void handleButtonXPress(){ // Xbox
  fill(100);
}

void handleButtonXRelease(){ // Xbox
  fill(255);
}

void draw(){
  // Read serial
  while (serial.available() > 0) {
    String message = serial.readStringUntil(10);
    
    if (message != null) {
      //print(message);
    }
  }
  
  background(0);
  
  float xval = stick0.getX();
  float yval = stick0.getY();
  
  float turnval = stick1.getX();
  
  // Deadzone
  xval = map(applyDeadzone(xval, 0.2), -1, 1, -5, 5);
  yval = map(applyDeadzone(yval, 0.2), -1, 1, -5, 5);
  turnval = applyDeadzone(turnval, 0.2);
  
  float trigger1val = stick1.getY();
  float trigger2val = map(stick2.getX(), -1, 1, 55, 100);
  
  // X
  if (xval != lastX) {
    if (xval == 0) {
      sendX(0);
      sendX(0);
      sendX(0);
    }
    sendX(xval);
    lastX = xval;
  }
  
  // Y
  if (yval != lastY) {
    if (yval == 0) {
      sendY(0);
      sendY(0);
      sendY(0);
    }
    sendY(yval);
    lastY = yval;
  }
  
  // Throttle
  if (trigger2val != lastThrottle) {
    if (trigger2val < 60) {
      trigger2val = 200;
      sendThrottle(trigger2val);
      sendThrottle(trigger2val);
      sendThrottle(trigger2val);
      //println("Killswitch");
      lastThrottle = 0;
    } else {
      sendThrottle(trigger2val);
      //print(message);
      lastThrottle = trigger2val;
    }
  }
}

float applyDeadzone(float val, float zone) {
  if (val > 0 && val < zone) {
    val = 0;
  } else if (val < 0 && val > -zone) {
    val = 0;
  }
  
  if (val > zone) {
    val = map(val, zone, 1, 0, 1);
  } else if (val < -zone) {
    val = map(val, -zone, -1, 0, -1);
  }
  
  return val;
}

void sendThrottle(float t) {
  serial.write("t"+int(t)+"\n");
  println(t);
}

void sendX(float x) {
  serial.write("x"+int(x)+"\n");
  println(x);
}

void sendY(float y) {
  serial.write("y"+int(y)+"\n");
  println(y);
}
