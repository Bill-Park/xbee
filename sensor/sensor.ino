/*
 * Alex Lima (alexheitorlima@gmail.com)
 * Example using BMA250 class with TinyDuino.
 **/

#include <Wire.h>
#include "BoschBMA250.h"
#include <String.h>
#include <SoftwareSerial.h>


SoftwareSerial xbee(3, 4) ;
BoschBMA250 BMA250;

byte nega = 0x00 ;
long t = 0 ;

void setup() 
{
  xbee.begin(57600) ;
  BMA250.begin();
  while(1) {
    if (xbee.available()) {
      if (xbee.read() == 0xBB) {
        delay(30) ;
        t = millis() ;
        break ;
      }
    }
  }
}

void loop() 
{
  if (t + 40 < millis()) {
    t = millis() ;
    BMA250.read();
    send_data(0x0D) ;
  }
}


void send_data(byte who) 
{
  double tem[3] = {
    BMA250.getAccelerationX(), BMA250.getAccelerationY(), BMA250.getAccelerationZ()                           } 
  ;
  byte check = who ;
  byte first, second ;
  xbee.write(who) ;
  for (int i = 0; i < 3; i++) {
    int up = tem[i] / 1 ;
    int down1 = (tem[i] - up) * 10 / 1 ;
    int down2 = (tem[i] - up) * 100 - (down1 * 10) ;
    if (down1 < 0 || down2 < 0) {
      nega = 0x01 ;
      down1 = abs(down1) ;
      down2 = abs(down2) ;
    }
    else nega = 0x02 ;
    
    first = up << 4 ;
    first |= down1 ;
    check ^= first ;

    second = down2 << 4 ;
    second |= nega ;
    check ^= second ;

    xbee.write(first) ;
    xbee.write(second) ;
  }
  xbee.write(check) ;
  xbee.flush() ;
}



