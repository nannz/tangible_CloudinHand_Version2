//#include <CurieIMU.h>
#include <MadgwickAHRS.h>
Madgwick filter;
unsigned long microsPerReading, microsPrevious;
float accelScale, gyroScale;

const int xpin = A2;
const int ypin = A1;
const int zpin = A0;

// Raw Ranges:
// initialize to mid-range and allow calibration to
// find the minimum and maximum for each axis
int xRawMin = 260;
int xRawMax = 420;
int yRawMin = 260;
int yRawMax = 420;
int zRawMin = 260;
int zRawMax = 420;
int xRaw = 0;
int yRaw = 0;
int zRaw = 0;

float accels[3] = {0.0,0.0,0.0};

// Take multiple samples to reduce noise
const int sampleSize = 10;

int gix=0;
int giy=0;
int giz=0;

void setup() {
  Serial.begin(9600);

  filter.begin(25);

  // initialize variables to pace updates to correct rate
  microsPerReading = 1000000 / 25;
  microsPrevious = micros();

}

void loop() {
  int aix, aiy, aiz;
//  int gix, giy, giz;
  float ax, ay, az;
  float gx, gy, gz;
  float roll, pitch, heading;
  unsigned long microsNow;

  //read data from the pin
//  xRaw = ReadAxis(xpin);
//  yRaw = ReadAxis(ypin);
//  zRaw = ReadAxis(zpin);

  xRaw = analogRead(xpin);
  yRaw = analogRead(ypin);
  zRaw = analogRead(zpin);
  //show the current x,y,z raw data
  //showRawData();
  //check the max and min of xyzAxises
  //AutoCalibrate(xRaw, yRaw, zRaw);
  // Convert raw values to 'milli-Gs"
  long xScaled = map(xRaw, xRawMin, xRawMax, -1000, 1000);
  long yScaled = map(yRaw, yRawMin, yRawMax, -1000, 1000);
  long zScaled = map(zRaw, zRawMin, zRawMax, -1000, 1000);
  // re-scale to fractional Gs
  float xAccel = xScaled / 1000.0;
  float yAccel = yScaled / 1000.0;
  float zAccel = zScaled / 1000.0;


  // check if it's time to read data and update the filter
  microsNow = micros();
  if (microsNow - microsPrevious >= microsPerReading) {

    // convert from raw data to gravity and degrees/second units
    ax = convertRawAcceleration(xRaw);
    ay = convertRawAcceleration(yRaw);
    az = convertRawAcceleration(zRaw);
    gx = convertRawGyro(gix);
    gy = convertRawGyro(giy);
    gz = convertRawGyro(giz);

    // update the filter, which computes orientation
    filter.updateIMU(gx, gy, gz, ax, ay, az);

    // print the heading, pitch and roll
    roll = filter.getRoll();
    pitch = filter.getPitch();
    heading = filter.getYaw();
    Serial.print("Orientation: ");
    Serial.print(heading);
    Serial.print(" ");
    Serial.print(pitch);
    Serial.print(" ");
    Serial.println(roll);

    // increment previous time, so we keep proper pace
    microsPrevious = microsPrevious + microsPerReading;
  }
  
  // delay before next reading:
 // delay(100);
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

float convertRawAcceleration(int aRaw) {
  // since we are using 2G range
  // -2g maps to a raw value of -32768
  // +2g maps to a raw value of 32767
  
  float a = (aRaw * 2.0) / 32768.0;
  return a;
}

float convertRawGyro(int gRaw) {
  // since we are using 250 degrees/seconds range
  // -250 maps to a raw value of -32768
  // +250 maps to a raw value of 32767
  
  float g = (gRaw * 250.0) / 32768.0;
  return g;
}

