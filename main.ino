#include "CRC16.h"
#include "HX711.h"
#include <EEPROM.h>

#define SENSOR_A_DOUT 2
#define SENSOR_A_SCK 3

#define SENSOR_B_DOUT 4
#define SENSOR_B_SCK 5

#define SENSOR_C_DOUT 6
#define SENSOR_C_SCK 7

#define SENSOR_D_DOUT 8
#define SENSOR_D_SCK 9

#define CRC_POLYNOME 0x8005

HX711 sensorA;
HX711 sensorB;
HX711 sensorC;
HX711 sensorD;
String buf;
String reply;
char sym;

void setup() {
  Serial.begin(115200);
  
  sensorA.begin(SENSOR_A_DOUT, SENSOR_A_SCK);
  sensorA.set_scale(readCalibration(1));
  
  sensorB.begin(SENSOR_B_DOUT, SENSOR_B_SCK);
  sensorB.set_scale(readCalibration(2));
  
  sensorC.begin(SENSOR_C_DOUT, SENSOR_C_SCK);
  sensorC.set_scale(readCalibration(3));
  
  sensorD.begin(SENSOR_D_DOUT, SENSOR_D_SCK);
  sensorD.set_scale(readCalibration(4));
}

int readCalibration(int cell) {
  long first = EEPROM.read(0 + (cell - 1) * 2);
  long second = EEPROM.read(1 + (cell - 1) * 2);
  return (first << 8) + second;
}

void writeCalibration(int cell, int value) {
  EEPROM.write(0 + (cell - 1) * 2, (value >> 8) & 0xFF);
  EEPROM.write(1 + (cell - 1) * 2, value & 0xFF);
}

uint16_t replyCRC(const String source, int begin = 0, int end = 0) {
  CRC16 crc;
  crc.setPolynome(CRC_POLYNOME);
  int e = end ? end : source.length();
  for (int i = begin; i < e; ++i) {
    crc.add(source[i]);
  }
  return crc.getCRC();
}

void loop() {  
  if (!Serial.available()) {
    return;
  }

  sym = Serial.read();
  buf += sym;

  if (!buf.endsWith("ND")) {
    return;
  }

  // Process request
  if (buf.startsWith("W")) { // Weight request from all sensors
    reply = "W";
    reply += "A";
    reply += sensorA.get_units(1);
    reply += "B";
    reply += sensorB.get_units(1);
    reply += "C";
    reply += sensorC.get_units(1);
    reply += "D";
    reply += sensorD.get_units(1);
    reply += "H";
    reply += replyCRC(reply);
    Serial.print("ST" + reply + "ND");
  } else if (buf.startsWith("AW")) { // Average weight
    float average = 0.0;
    average += sensorA.get_units(1);
    average += sensorB.get_units(1);
    average += sensorC.get_units(1);
    average += sensorD.get_units(1);
    average /= 4;
    reply = "AW";
    reply += average;
    reply += "H";
    reply += replyCRC(reply);
    Serial.print("ST" + reply + "ND");
  } else if (buf.startsWith("TARE")) {
    sensorA.tare();
    sensorB.tare();
    sensorC.tare();
    sensorD.tare();
    Serial.print("STTAREND");
  } else if (buf.startsWith("GETCAL")) {
    int index = buf[6] - '0';
    if (index < 1 || index > 4) { // Invalid sensor index
      Serial.print("STGETCALERRND");
      return;
    }
    Serial.print("STGETCAL");
    Serial.print(index);
    Serial.print(readCalibration(index));
    Serial.print("ND");
  } else if (buf.startsWith("SETCAL")) {
    int index = buf[6] - '0';
    if (index < 1 || index > 4) { // Invalid sensor index
      Serial.print("STSETCALERRND");
      return;
    } 
    int value = buf.substring(7, buf.indexOf("ND")).toInt();
    writeCalibration(index, value);
    switch (index) {
    case 1:
      sensorA.set_scale(value);
      break;
    case 2:
      sensorB.set_scale(value);
      break;
    case 3:
      sensorC.set_scale(value);
      break;
    case 4:
      sensorD.set_scale(value);
      break;
    }
    Serial.print("STSETCAL");
    Serial.print(index);
    Serial.print("ND");
  }

  buf = "";
}
