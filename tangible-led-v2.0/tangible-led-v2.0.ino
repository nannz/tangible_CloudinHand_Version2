//the totalstate of the device of off/on
#include <Wire.h>
#include "Adafruit_DRV2605.h"
Adafruit_DRV2605 drv;
#include <CapacitiveSensor.h> //capacitive sensor: send pin attached to the resistor
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif


//states
int totalState = LOW;
int acceState = LOW;
int ledState = LOW;
int ledMode;

//led colors
int colorMode0[] = {255, 255, 255}; //white
int colorMode1[] = {242, 214, 73}; //yellow
int colorMode2[] = {83, 26, 232}; //blue
//colorMode3 is rainbow

//neopixel set up
#define PIN            6
#define NUMPIXELS      4
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int delayLed = 500; // delay for half a second

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
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.show();
  
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
        if (ledState == HIGH) {
          ledState = LOW;//turn off the led
          ledMode = 0;
        } else {
          ledState = HIGH; // turn on the led
          ledMode = 0; //led starts with mode 0
        }
        motorLevel = 0;
      } else {
        if (totalState == HIGH) {
          Serial.println("trigger clicked | change led mode");
          ledState = HIGH;
          ledMode += 1;
        } else {
          ledState = LOW;
        }
        motorLevel = 0;
      }
    } else {
      motorLevel = 0;
    }
  }
  prevFsrState = curFsrState;

  //sensing capacitive touch only when the total state is high
  if (totalState == HIGH ) {

    //long capacitiveStart = millis();
    long capacitiveRead =  cs_4_2.capacitiveSensor(30);
    if (capacitiveRead >= capaMinRead) {
      //capacitiveStart = millis();
      acceState = HIGH;
      ledState = LOW;
      Serial.print("capacitiveRead: ");
      Serial.print(capacitiveRead);
      Serial.println(" | start accelerometer mode.");
    } else {
      capacitiveEnd = millis();
      acceState = LOW;
      ledState = HIGH;
    }

    //    //有问题啊
    //    if (millis() - capacitiveEnd > capaTouchHoldTime){
    //      Serial.println("test");
    //      if(capacitiveRead <capaMinRead){
    //        acceState = LOW;
    //      }
    //    }

    if (ledState == HIGH) {
      if (ledMode % 4 == 0) {
        Serial.println("Color mode: 0");
        setNeoColor(colorMode0[0],colorMode0[1],colorMode0[2]);
      } else if (ledMode % 4 == 1) {
        Serial.println("Color mode: 1");
        setNeoColor(colorMode1[0],colorMode1[1],colorMode1[2]);
      } else if (ledMode % 4 == 2) {
        Serial.println("Color mode: 2");
        setNeoColor(colorMode2[0],colorMode2[1],colorMode2[2]);
      } else if (ledMode % 4 == 3) {
        Serial.println("Color mode: 3");
        setNeoColor(0,0,255);
        //rainbow(10);
      }
    }else{
      setNeoColor(0,0,0);
    }
  }else{
    ledState = LOW;
    setNeoColor(0,0,0);
  }

  Serial.print("acceState: ");
  Serial.print(acceState);
  Serial.print(" | ledState: ");
  Serial.print(ledState);
  Serial.print(" | ledMode: ");
  Serial.println(ledMode);
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


void setNeoColor(int r, int g, int b){
  for(int i=0;i<NUMPIXELS;i++){
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(r,g,b)); // Moderately bright green color.
    pixels.show(); // This sends the updated pixel color to the hardware.
    //delay(delayLed); // Delay for a period of time (in milliseconds).
  }
}
void rainbow(uint8_t wait) {
  uint16_t i, j;
  for(j=0; j<256; j++) {
    for(i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i+j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
