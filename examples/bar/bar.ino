#include <Wire.h>
#include "I2S.h"
#include "SdFat.h"

#define PRESS_ADR 93

uint8_t i2c_mem_write(uint8_t reg, uint8_t addr, uint8_t data);
uint8_t i2c_mem_read(uint8_t count, uint8_t addr, uint8_t reg, uint8_t * buf);
SdFatSdio sd;

void setup() {
  Serial.begin(115200);
  Wire.begin(); // start the I2C driver for codec register setup 
  sd.begin();
}

int LOOP_COUNTER = 5;

void loop() {
  static int hpa_s = 0, measure_count = 0;
  uint8_t buf[5];
  
  i2c_mem_write(0x20, PRESS_ADR, 0x10);
  i2c_mem_read(5, PRESS_ADR, 0x28, buf);
  hpa_s += (buf[2]*65536+buf[1]*256+buf[0]);
  measure_count += 1;

  if(measure_count == LOOP_COUNTER){
    Serial.println(hpa_s / LOOP_COUNTER);    

    File outputfile = sd.open("/data.csv", FILE_WRITE);
    outputfile.println(hpa_s/LOOP_COUNTER);
    outputfile.close();
    measure_count = 0;
    hpa_s = 0;
  }
  delay(5);
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
