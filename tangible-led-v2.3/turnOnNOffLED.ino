
//turnoff the led in a fading way
void turnOffLED(int endColor[3]) {
  bool finish = false;
  while (endColor[0] >= 0 && endColor[1] >= 0 && endColor[2] >= 0 && !finish) {

    //if the value is bigger than 0, decrease the value
    for (int i = 0; i < 3; i++) {
      if (endColor[i] > 0) {
        endColor[i]--;
      }
      if (endColor[i] == 0) {
        endColor[i] = 0;
      }
    }
    //setLEDColor(endColor[0], endColor[1], endColor[2]);
    setNeoColor(endColor[0], endColor[1], endColor[2]);
    /*
    Serial.print("currentRGB: ");
    Serial.print(endColor[0]);
    Serial.print(", ");
    Serial.print(endColor[1]);
    Serial.print(", ");
    Serial.print(endColor[2]);
    Serial.println(". ");
    */
    //finish "fading" turn off
    if (endColor[0] == 0 && endColor[1] == 0 && endColor[2] == 0) {
      finish = true;
    }
    // wait for 10 milliseconds to see the dimming effect
    delay(10);
  }
}

void turnOnLED(int beginColor[3]) {
  bool finish = false;
  while (currentColor[0] <= beginColor[0] && currentColor[1] <= beginColor[1] && currentColor[2] <= beginColor[2] && !finish) {
    //if the value is smaller than the color, increase the value
    //if the value reaches the color, keep the red/blue/green value and wait for others.
    //问题:这样不是同步加value了,可以map fadeValue in a certain points
    for (int i = 0; i < 3; i++) {
      if (currentColor[i] < beginColor[i]) {
        currentColor[i]++;
      }
      if (currentColor[i] == beginColor[i]) {
        currentColor[i] = beginColor[i];
      }

      //setLEDColor(currentColor[0], currentColor[1], currentColor[2]);
      setNeoColor(currentColor[0], currentColor[1], currentColor[2]);
      
      //finish "fading" turn off
      if (currentColor[0] == beginColor[0] && currentColor[1] == beginColor[1] && currentColor[2] == beginColor[2]) {
        finish = true;
      }
      // wait for 10 milliseconds to see the dimming effect
      delay(10);
    }
  }
}
