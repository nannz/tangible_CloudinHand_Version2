
const int xpin = A2;
const int ypin = A1;
const int zpin = A0;

// Raw Ranges:
// initialize to mid-range and allow calibration to
// find the minimum and maximum for each axis
int xRawMin = 410;
int xRawMax = 590;
int yRawMin = 395;
int yRawMax = 620;
int zRawMin = 430;
int zRawMax = 675;
int xRaw = 0;
int yRaw = 0;
int zRaw = 0;

//float accels[3] = {0.0,0.0,0.0};

// Take multiple samples to reduce noise
const int sampleSize = 10;

int hue = 0;
int saturation = 255;
int value = 255;
int rgbColor[]={0,0,0};

void setup() {
  Serial.begin(9600);
}

void loop() {
  xRaw = ReadAxis(xpin);
  yRaw = ReadAxis(ypin);
  zRaw = ReadAxis(zpin);

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

  //get the value of saturation
  long saturationFloat = map(zScaled,-1000,1000,0,255);
  saturation = (int)saturationFloat;
  Serial.print("saturation: ");
  Serial.print(saturation);
  Serial.print("|  ");

  //get the hue value
  float angle = atan2(yAccel,xAccel) * 180/M_PI;
  float hueValue = map(angle, -180.0, 180.0, 0, 259);
  hue = (int)hueValue;
  //Serial.print("angle1: ");
  //Serial.print(angle);
  Serial.print("hue: ");
  Serial.print(hue);
  Serial.print("|  ");

  //get RGB value from the hue value and saturation value.
  getRGB(hue,saturation,value,rgbColor);
  Serial.print("rgb: ");
  Serial.print(rgbColor[0]);
  Serial.print(", ");
  Serial.print(rgbColor[1]);
  Serial.print(", ");
  Serial.print(rgbColor[2]);
  Serial.println(" ");
  
  // delay before next reading:
  delay(100);
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



