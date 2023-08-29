#include <Arduino.h>
#include <ModbusAscii.h>

ModbusAscii modbus(Serial2, 9600);
uint8_t addresSlave = 1;
void setup() {
  Serial.begin(9600); 
  while (!Serial) {;}
  modbus.begin();
}

void loop() {
  /* read multiple register */
  int responMB[4];
  int16_t dataReg[4], alertReg[3], dataRegsingle;
  responMB[0] = modbus.ReadMultiReg(addresSlave, READ_HOLDING_REGISTERS, 1, sizeof(alertReg)/sizeof(alertReg[0]), alertReg);
  if(printResponseMultiReg(responMB[0])){
    Serial.println("Read multiple register success");
    for(int i=0; i<sizeof(alertReg)/sizeof(alertReg[0]); i++){
      Serial.print("Data:");
      Serial.println(alertReg[i]);
    }
  }
  delay(100);
  uint16_t randomData = random(1, 1000);
  responMB[1] = modbus.WriteMultiReg(addresSlave, PRESET_MULTIPLE_REGISTERS, 1, sizeof(alertReg)/sizeof(alertReg[0]), &randomData);
  if(printResponseMultiReg(responMB[1])){
    Serial.println("Write multiple register success");
  }
  delay(100);
  responMB[2] = modbus.ReadSingleReg(addresSlave, READ_HOLDING_REGISTERS, 12, &dataRegsingle);
  if(printResponseMultiReg(responMB[2])){
    Serial.println("read singgle register success");
    Serial.print("data single register:");
    Serial.println(dataRegsingle);
  }
  delay(100);
  responMB[3] = modbus.WriteSigleReg(addresSlave, WRITE_SINGLE_REGISTER, 12, randomData);
  if(printResponseMultiReg(responMB[1])){
    Serial.println("Write single register success");
  }
  delay(100);

}
bool printResponseMultiReg(int &expres_){
  switch (expres_)
  {
  case MB_SUKSES:
    return true;
    break;

  case MB_FRAME_ERROR:
    Serial.println("Modbus Frame error");
    return false;
    break;

  case MB_CHECKSUM_ERROR:
    Serial.println("Modbus Checksum error");
    return false;
    break;

  case MB_TIMEOUT:
    Serial.println("Modbus Time Out");
    return false;
    break;

  case MB_INVALID_DATA_ERROR:
    Serial.println("Modbus Error: Invalid data");
    return false;
    break;

  case MB_ADDRESS_ERROR:
    Serial.println("Modbus Addres/register error");
    return false;
    break;

  case MB_DATA_LENGTH_ERROR:
    Serial.println("Modbus Date length error");
    return false;
    break;

  case MB_COMMUNICATION_ERROR:
    Serial.println("Modbus Serial error communication");
    return false;
    break;

  default:
    Serial.println("?");
    return false;
    break;
  }
}