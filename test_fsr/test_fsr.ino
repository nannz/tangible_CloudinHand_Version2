#include <Wire.h>
#include "Adafruit_DRV2605.h"
Adafruit_DRV2605 drv;
int motorLevel = 0;
int potenDriveEffect = 80;
int fsrClickDriveEffect = 30; //real time level of the motor.
int fsrPressDriveEffect = 80;
//the fsr sensor and states
int fsrPin = A5;
int fsrReading;
int curFsrState = LOW;
int prevFsrState = LOW;
long fsrDownTime = -1;
long fsrUpTime = -1;
int fsrHoldTime = 100; // ms hold period: how long to wait for press+hold event
int fsrLongHoldTime = 1000; // ms long hold period: how long to wait for press+hold event

int responseDelay = 70;

void setup() {
  Serial.begin(9600);
  //begin the motor drive
  drv.begin();
  drv.setMode(DRV2605_MODE_REALTIME);
}

void loop() {
  //reading the fsr value
  fsrReading = analogRead(fsrPin);
  Serial.println(fsrReading);
  if (fsrReading <= 500) {
    curFsrState = LOW;
  } else {
    curFsrState = HIGH;
  }

  //-----------------------------------------------------//

  if (curFsrState == HIGH && prevFsrState == LOW) {
    //press down
    fsrDownTime = millis();
//    Serial.print("fsrDownTime: ");
//    Serial.println(fsrDownTime);
  }
  if (curFsrState == LOW && prevFsrState == HIGH) {
    //press up
    fsrUpTime = millis();
//    Serial.print("fsrUpTime: ");
//    Serial.println(fsrUpTime);
  }
  //remind the click time is enough.
  if (curFsrState == HIGH && (millis() - fsrDownTime) >= fsrHoldTime) {
    //Serial.println("It's enough hold time.");
    motorLevel = fsrClickDriveEffect;
    //remind the press time is enough.
    if ((millis() - fsrDownTime) >= fsrLongHoldTime) {
      //Serial.println("It's enough press time.");
      motorLevel = fsrPressDriveEffect;
    }
  }
  //trigger the function
  if (curFsrState == LOW && prevFsrState == HIGH ) {
    if ((fsrUpTime - fsrDownTime) >= fsrHoldTime) {
      if ((fsrUpTime - fsrDownTime) >= fsrLongHoldTime) {
        Serial.println("trigger long pressed");
        motorLevel = 0;
      } else {
        Serial.println("trigger clicked");
        motorLevel = 0;
      }
    } else {
      motorLevel = 0;
    }
  }
  prevFsrState = curFsrState;

  //run the motor.
  drv.setRealtimeValue(motorLevel);
  delay(responseDelay);
}
