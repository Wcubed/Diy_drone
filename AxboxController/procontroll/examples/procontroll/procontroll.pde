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
  joypad.plug(this, "handleButton1Press", ControllIO.ON_PRESS, 1);
  joypad.plug(this, "handleButton1Release", ControllIO.ON_RELEASE, 1);

  stick1 = joypad.getStick(0);
  stick1.setMultiplier(PI);

  stick2 = joypad.getStick(2);
  stick2.setTolerance(0.06f);
  stick2.setMultiplier(0.05f);
}

void handleButton1Press(){
  fill(255,0,0);
  joypad.rumble(1);
}

void handleButton1Release(){
  fill(255);
  joypad.rumble(0);
}

void draw(){
  background(0);
  lights();
  translate(transX,transY,0);
  rotateX(stick2.getTotalY());
  rotateY(stick2.getTotalX());
  box(200);
}
