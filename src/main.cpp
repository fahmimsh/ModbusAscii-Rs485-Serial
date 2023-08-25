#include <Arduino.h>
#include <ModbusAscii.h>
ModbusAscii modbus(Serial2, 9600);
int responMB[2];
int16_t dataReg[4], alertReg[3];
void setup() {
  Serial.begin(9600); 
  while (!Serial) {;}
  modbus.begin();
}

void loop() {

}