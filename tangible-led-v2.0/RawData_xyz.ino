void showRawData(){
  // print the sensor values:
  Serial.print("x: ");
  Serial.print(xRaw);
//  Serial.print(analogRead(xpin));
  Serial.print("/");
  Serial.print(xRawMin);
  Serial.print(",");
  Serial.print(xRawMax);
  // print a tab between values:
  Serial.print("\t");
  Serial.print("y: ");
  //Serial.print(analogRead(ypin));
  Serial.print(yRaw);
  Serial.print("/");
  Serial.print(yRawMin);
  Serial.print(",");
  Serial.print(yRawMax);
  // print a tab between values:
  Serial.print("\t");
  Serial.print("z: ");
  //Serial.print(analogRead(zpin));
  Serial.print(zRaw);
  Serial.print("/");
  Serial.print(zRawMin);
  Serial.print(",");
  Serial.print(zRawMax);
  Serial.println();
}
