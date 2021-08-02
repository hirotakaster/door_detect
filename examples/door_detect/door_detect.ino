#include <TensorFlowLite.h>
#include <Wire.h>
#include "I2S.h"
#include "main_functions.h"

void setup() {
  Serial.begin(115200);
  Wire.begin();
  tf_initialize();
}

void loop() {
  tf_detect();
}
