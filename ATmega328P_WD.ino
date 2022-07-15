#include "HX711.h"
#include <EEPROM.h>

HX711 cell1;
HX711 cell2;
HX711 cell3;
HX711 cell4;

char sym;
String buf;

void setup() {
  Serial.begin(115200);
  
  cell1.begin(3, 2);
  cell1.set_scale(readCalibration(1));
  
  cell2.begin(5, 4);
  cell2.set_scale(readCalibration(2));
  
  cell3.begin(7, 6);
  cell3.set_scale(readCalibration(3));
  
  cell4.begin(9, 8);
  cell4.set_scale(readCalibration(4));
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

void loop() {
  if (Serial.available()) {
    sym = Serial.read();
    buf += sym;
    if (buf.endsWith("END")) {
      // Process request
      if (buf.startsWith("AW")) {
        float average = 0.0;
        average += cell1.get_units(1);
        average += cell2.get_units(1);
        average += cell3.get_units(1);
        average += cell4.get_units(1);
        average /= 4;
        Serial.print("AW");
        Serial.print(average);
        Serial.print("END");
      } else if (buf.startsWith("W")) {
        Serial.print("WA");
        Serial.print(cell1.get_units(1));
        Serial.print("B");
        Serial.print(cell2.get_units(1));
        Serial.print("C");
        Serial.print(cell3.get_units(1));
        Serial.print("D");
        Serial.print(cell4.get_units(1));
        Serial.print("END");
      } else if (buf.startsWith("TARE")) {
        cell1.tare();
        cell2.tare();
        cell3.tare();
        cell4.tare();
        Serial.print("TAREEND");
      } else if (buf.startsWith("GETCAL")) {
        int index = buf[6] - '0';
        if (index < 1 || index > 4) {
          Serial.print("GETCALERREND");
        } else {
          Serial.print("GETCAL");
          Serial.print(index);
          Serial.print(readCalibration(index));
          Serial.print("END");
        }
      } else if (buf.startsWith("SETCAL")) {
        int index = buf[6] - '0';
        if (index < 1 || index > 4) {
          Serial.print("SETCALERREND");
        } else {
          int value = buf.substring(7, buf.indexOf("END")).toInt();
          writeCalibration(index, value);
          switch (index) {
          case 1:
            cell1.set_scale(value);
            break;
          case 2:
            cell2.set_scale(value);
            break;
          case 3:
            cell3.set_scale(value);
            break;
          case 4:
            cell4.set_scale(value);
            break;
          }
          Serial.print("SETCAL");
          Serial.print(index);
          Serial.print("END");
        }
      }
      buf = "";
    }
  }
}
