#include <Arduino.h>


void setup() {
  Serial.begin(921600);
}

void loop() {
  if (Serial.read() != 'r'){
    return;
  }

  Serial.println("START");
  Serial.println("END");
}
