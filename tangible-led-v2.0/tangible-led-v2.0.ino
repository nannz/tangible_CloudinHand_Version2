//the totalstate of the device of off/on
#include <Wire.h>
#include "Adafruit_DRV2605.h"
Adafruit_DRV2605 drv;
#include <CapacitiveSensor.h> //capacitive sensor: send pin attached to the resistor

//states
int totalState = LOW;
int acceState = LOW;

//capacitive Sensor
CapacitiveSensor   cs_4_2 = CapacitiveSensor(4, 2);       // 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired
int capaTouchHoldTime = 1000;
int capaMinRead = 10000;
long capacitiveStart = -1;
long capacitiveEnd = -1;

//the fsr sensor and states
int fsrPin = A5;
int fsrReading;
int curFsrState = LOW;
int prevFsrState = LOW;
long fsrDownTime = -1;
long fsrUpTime = -1;
int fsrHoldTime = 100; // ms hold period: how long to wait for press+hold event
int fsrLongHoldTime = 1000; // ms long hold period: how long to wait for press+hold event
int fsrTouchStartPoint = 250; //the minmum analog reading of the sensor to sense a pressure

//the motor drive setting
int motorLevel = 0;
int fsrClickDriveEffect = 50;
int fsrPressDriveEffect = 80;




int responseDelay = 30;

void setup() {
  //capacitive touch, calibration
  cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example

  Serial.begin(9600);
  //begin the motor drive
  drv.begin();
  drv.setMode(DRV2605_MODE_REALTIME);
}

void loop() {
  //reading the fsr value
  fsrReading = analogRead(fsrPin);
  //Serial.print("fsrReading: ");
  //Serial.println(fsrReading);
  if (fsrReading <= fsrTouchStartPoint) {
    curFsrState = LOW;
  } else {
    curFsrState = HIGH;
  }

  if (curFsrState == HIGH && prevFsrState == LOW) {
    //press down
    fsrDownTime = millis();
  }
  if (curFsrState == LOW && prevFsrState == HIGH) {
    //press up
    fsrUpTime = millis();
  }


  //remind the click time is enough using motor drive.
  if (curFsrState == HIGH && (millis() - fsrDownTime) >= fsrHoldTime) {
    Serial.println("It's enough hold time.");
    if (totalState == HIGH) {
      //only when the state is on, the click effect would be triggered
      motorLevel = fsrClickDriveEffect;
    }
    //remind the press time is enough.
    if ((millis() - fsrDownTime) >= fsrLongHoldTime) {
      Serial.println("It's enough press time.");
      motorLevel = fsrPressDriveEffect;
    }
  }

  //trigger the hold/press mode.
  if (curFsrState == LOW && prevFsrState == HIGH ) { //手抬起来
    if ((fsrUpTime - fsrDownTime) >= fsrHoldTime) {
      if ((fsrUpTime - fsrDownTime) >= fsrLongHoldTime) {
        Serial.println("trigger long hold | turn on/off");
        totalState = !totalState;
        motorLevel = 0;
      } else {
        if (totalState == HIGH) {
          Serial.println("trigger clicked | change led mode");
        }
        motorLevel = 0;
      }
    } else {
      motorLevel = 0;
    }
  }
  prevFsrState = curFsrState;

  //sensing capacitive touch
  if (totalState == HIGH ) {

    //long capacitiveStart = millis();
    long capacitiveRead =  cs_4_2.capacitiveSensor(30);
    if (capacitiveRead >= capaMinRead) {
      //capacitiveStart = millis();
      acceState = HIGH;
      Serial.print("capacitiveRead: ");
      Serial.print(capacitiveRead);
      Serial.println(" | start accelerometer mode.");
    } else {
      capacitiveEnd = millis();
      acceState = LOW;
    }

//    //有问题啊
//    if (millis() - capacitiveEnd > capaTouchHoldTime){
//      Serial.println("test");
//      if(capacitiveRead <capaMinRead){
//        acceState = LOW;     
//      }
//    }
  }

  Serial.print("acceState: ");
  Serial.println(acceState);
  //capacitiveEnd = 0;
  
  Serial.print("totalState: ");
  if (totalState == HIGH) {
    Serial.println("on");
  }
  if (totalState == LOW) {
    Serial.println("off");
  }

  //run the drive
  drv.setRealtimeValue(motorLevel);
  //delay(responseDelay);
}
