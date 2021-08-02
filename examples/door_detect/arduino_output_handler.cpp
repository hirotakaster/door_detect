#include "output_handler.h"

#include "Arduino.h"
#include "constants.h"

#include <Wire.h>
#include "I2S.h"
#define PRESS_ADR 93


static int AVG_PRESS = 4162971;
static int NORM_PRESS = 0;
static int NORM_MAX = 4170112;
static int NORM_MIN = 4157760;

#define NORMALIZE_PRESS(val) (float)(val-NORM_MIN)/(4170112-NORM_MIN)
uint8_t i2c_mem_write(uint8_t reg, uint8_t addr, uint8_t data);
uint8_t i2c_mem_read(uint8_t count, uint8_t addr, uint8_t reg, uint8_t * buf);

void avgPressor() {
  int now = getPressor();
  NORM_PRESS = AVG_PRESS - now;
}

uint8_t i2c_mem_write(uint8_t data, uint8_t addr, uint8_t reg){
  uint8_t error;
  
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(data);
  error = Wire.endTransmission();
  return error;
}

uint8_t i2c_mem_read(uint8_t count, uint8_t addr, uint8_t reg, uint8_t * buf){
  uint8_t error;
  uint8_t n;
  
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(addr, count);
  delay(1);
  for(n = 0; n < count; n++){
    buf[n] = Wire.read();
  }
  error = Wire.endTransmission();
  return error;
}

// Animates a dot across the screen to represent the current x and y values
void HandleOutput(tflite::ErrorReporter* error_reporter, 
                  float y_label1, float y_label2, float y_label3) {
  Serial.print(y_label1);
  Serial.print(",");
  Serial.print(y_label2);
  Serial.print(",");
  Serial.println(y_label3);
}

#define LOOP_COUNTER 1
int getPressor() {
  int hpa_s = 0, measure_count = 0;
  uint8_t buf[5];
  
  for (int i = 0; i < LOOP_COUNTER; i++) {
    i2c_mem_write(0x20, PRESS_ADR, 0x10);
    i2c_mem_read(5, PRESS_ADR, 0x28, buf);
    hpa_s += (buf[2]*65536+buf[1]*256+buf[0]);
    delay(15);
  }
  hpa_s = hpa_s/LOOP_COUNTER;
  return (hpa_s);
}

float getNormPressor() {
  int hpa = getPressor() + NORM_PRESS;
  return NORMALIZE_PRESS(hpa);
}
