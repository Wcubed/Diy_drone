import processing.opengl.*;

import procontroll.*;
import net.java.games.input.*;

ControllIO controllIO;
ControllDevice joypad;
ControllStick stick1;
ControllStick stick2;

float transX;
float transY;

void setup(){
  size(600,600,OPENGL);

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

  stick1 = joypad.getStick(0);
  stick1.setMultiplier(PI);

  stick2 = joypad.getStick(2);
  stick2.setTolerance(0.06f);
  stick2.setMultiplier(0.05f);
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

void draw(){
  background(0);
  lights();
  translate(transX,transY,0);
  rotateX(stick2.getTotalY());
  rotateY(stick2.getTotalX());
  box(200);
}
