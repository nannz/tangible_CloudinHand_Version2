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
long capacitiveRead = 0;
int capaTouchHoldTime = 1000;
int capaMinRead = 2000;
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

//the accelerometer
const int xpin = A2;
const int ypin = A1;
const int zpin = A0;
// Raw Ranges:
int xRawMin = 410;
int xRawMax = 625;
int yRawMin = 395;
int yRawMax = 620;
int zRawMin = 430;
int zRawMax = 655;
int xRaw = 0;
int yRaw = 0;
int zRaw = 0;
// Take multiple samples to reduce noise
const int sampleSize = 10;
int hue = 0;
int saturation = 255;
int value = 255;
int rgbColor[] = {0, 0, 0};


//for turning on and turning off
int currentColor[] = {0, 0, 0};
bool turnOff = false;
bool turnOn = false;
int beginColor[]  = {255, 255, 255}; //the color when you turn on the led


int responseDelay = 30;

// the follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers

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
  capacitiveRead =  cs_4_2.capacitiveSensor(10);
  Serial.println(capacitiveRead);
  if (totalState == LOW){
    ledState = LOW;
    acceState = LOW;
    motorLevel = 0;
  }


  if (capacitiveRead >= capaMinRead ) {  
      Serial.println("test");
      if ( totalState == LOW ) { //如果现在是关机状态,就打开它
        totalState = HIGH;
        turnOn = true;
        turnOff = false;     
      }
      if (totalState == HIGH){
        acceState = HIGH;
        ledState = LOW;
        motorLevel = 0;
      }
  }
  //time = millis();

 //----------fsr-------// 
  //Serial.print("fsrReading: ");
  //Serial.println(fsrReading);
  if (fsrReading <= fsrTouchStartPoint) {
    curFsrState = LOW;
  } else {
    curFsrState = HIGH;
  }
  //record the time for drive effect
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
      motorLevel = fsrClickDriveEffect;
      drv.setRealtimeValue(fsrClickDriveEffect);
    }
    //remind the press time is enough.
    if ((millis() - fsrDownTime) >= fsrLongHoldTime) {
      Serial.println("It's enough press time.");
      motorLevel = fsrPressDriveEffect;
      drv.setRealtimeValue(fsrPressDriveEffect);
    }
  }
//提醒你手抬起来,然后你手抬起来以后
  if (curFsrState == LOW && prevFsrState == HIGH ) { //手抬起来
    motorLevel = 0;
    if ((fsrUpTime - fsrDownTime) >= fsrHoldTime) { //click
      //turn on the cloud just by clicking
      if(totalState == LOW){
        totalState = HIGH;
        turnOn = true;
        turnOff = false;
        ledState = HIGH;
        acceState = LOW;
      }else{
        //it's already turn on
        ledState = HIGH;
        ledMode += 1;
        acceState = LOW;
      }
      if((fsrUpTime - fsrDownTime) >= fsrLongHoldTime) {
        if(totalState == HIGH){ // long hold to turn off
          totalState = LOW;
          turnOn = false;
          turnOff = true;
        }else{//已关机,就开机,但其实没什么用
          totalState = HIGH;
          turnOn = true;
          turnOff = false;
        }
      }      
    }
  }
  //-------fsr end -------//


  //-----turning on and off effect ----//
  if (turnOff == true) {
    Serial.println("turnning off");
    motorLevel = 0;
    drv.setRealtimeValue(0);
    turnOffLED(currentColor);
    ledState = LOW;
    turnOff = false;
  }
  if (turnOn == true) {
    Serial.println("turnning on");
    //motorLevel = 0;
    turnOnLED(beginColor);
    ledState = HIGH;
    ledMode = 0;
    turnOn = false;
  }

  if (totalState == HIGH && turnOn == false){
    
  }
  
  prevFsrState = curFsrState;
  drv.setRealtimeValue(motorLevel);
  delay(responseDelay);
}


void setNeoColor(int r, int g, int b) {
  for (int i = 0; i < NUMPIXELS; i++) {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(r, g, b)); // Moderately bright green color.
    pixels.show(); // This sends the updated pixel color to the hardware.
    //delay(delayLed); // Delay for a period of time (in milliseconds).
  }
  //update the current color
  currentColor[0] = r;
  currentColor[1] = g;
  currentColor[2] = b;
}
void rainbow(uint8_t wait) {
  uint16_t i, j;
  for (j = 0; j < 256; j++) {
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    //update the current color
    currentColor[0] = 255 - WheelPos * 3;
    currentColor[1] = 0;
    currentColor[2] = WheelPos * 3;
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    currentColor[0] = 0;
    currentColor[1] = WheelPos * 3;
    currentColor[2] = 255 - WheelPos * 3;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  currentColor[0] = WheelPos * 3;
  currentColor[1] = 255 - WheelPos * 3;
  currentColor[2] = 0;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

int ReadAxis(int axisPin)
{
  long reading = 0;
  analogRead(axisPin);
  delay(1);
  for (int i = 0; i < sampleSize; i++)
  {
    reading += analogRead(axisPin);
  }
  return reading / sampleSize;
}


void getAcceColor(int xRaw, int yRaw, int zRaw, int rgbColor[3]) {
  long xAngle = map(xRaw, xRawMin, xRawMax, -180, 180);
  long yAngle = map(yRaw, yRawMin, yRawMax, -180, 180);
  long zAngle = map(zRaw, zRawMin, zRawMax, -180, 180);
  //get the value of saturation
  long saturationFloat;
  if (zAngle < 0) {
    saturationFloat = map(zAngle, -180, 0, 100, 255);
  } else {
    saturationFloat = map(zAngle, 0, 180, 255, 100);
  }
  saturation = (int)saturationFloat;
  //get the hue value
  float angle = atan2(yAngle, xAngle) * (180 / PI);
  if (angle < 0) angle = 180 + angle;
  float hueValue = map(angle, 0, 180.0, 0, 259);
  hue = (int)hueValue;
  getRGB(hue, saturation, value, rgbColor);
}
