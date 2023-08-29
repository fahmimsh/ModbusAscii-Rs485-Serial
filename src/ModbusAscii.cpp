#include "ModbusAscii.h"
/* define class func */
ModbusAscii::ModbusAscii(Stream& stream, unsigned long baudrate, uint32_t config){
  _stream = &stream;
  _baudrate = baudrate;
  _config = config;
}
/* Initialization program for setting up the serial configuration, for example modbus.begin(); */
void ModbusAscii::begin(){
    static_cast<HardwareSerial*>(_stream)->begin(_baudrate, _config);
    while (!_stream) {;}
}
/* Function to read multiple registers, 
for example registers 0-124 with a quantity of 125, with the following arguments:
ReadMultiReg(slave address, read function, starting register is 0, 125, buffer to store data. */
int ModbusAscii::ReadMultiReg(uint8_t slaveid_, unsigned char func_, uint16_t startRegister_, uint8_t quantity_, int16_t* ResulReg) {
  SendAsciiCode(slaveid_, func_, startRegister_, quantity_); 
  return ReadTransaction(slaveid_, func_, quantity_, ResulReg);
}
/* Function to read single registers, 
for example registers 64-th, with the following arguments:
ReadSingleReg(slave address, read function, register is 64, buffer to store data. */
int ModbusAscii::ReadSingleReg(uint8_t slaveid_, unsigned char func_, uint16_t Register_, int16_t* ResulReg) {
  SendAsciiCode(slaveid_, func_, Register_, 1); 
  return ReadTransaction(slaveid_, func_, 1, ResulReg);
}
/* Function to write single registers, 
for example registers 64-th, with the following arguments:
ReadSingleReg(slave address, read function, register is 64, value to write data). */
int ModbusAscii::WriteSigleReg(uint8_t slaveid_, unsigned char func_, uint16_t Register_, uint16_t value_){
  SendAsciiCode(slaveid_, func_, Register_, value_);
  return WriteTransaction(slaveid_, func_, Register_, 1);
}
/* Function to write multiple registers, 
for example registers 0-124 with a quantity of 125, with the following arguments:
ReadMultiReg(slave address, read function, starting register is 0, 125, value to write all data. */
int ModbusAscii::WriteMultiReg(uint8_t slaveid_, unsigned char func_, uint16_t Register_, uint8_t quantity_, uint16_t* value_){
  char code[255];
  sprintf(code, "%02X%02X%04X%04X%02X", slaveid_, func_, Register_, quantity_, (quantity_*4));
  for(int i = 0; i < quantity_; i++){ sprintf(&code[(i*4)+14], "%04X", value_[i]);}
  sprintf(&code[14 + (quantity_*4)], "%02X\r\n", calculateLRC(code));
  while (_stream->read() != -1);
  _stream->printf(":%s", code);
  _stream->flush();
  return WriteTransaction(slaveid_, func_, Register_, quantity_);
}
void ModbusAscii::SendAsciiCode(unsigned char slaveID, unsigned char func, unsigned short startReg, unsigned short quantity){
  char code[17];
  sprintf(code, "%02X%02X%04X%04X", slaveID, func, startReg, quantity);
  sprintf(&code[12], "%02X\r\n", calculateLRC(code));
  while (_stream->read() != -1);
  _stream->printf(":%s", code);
  _stream->flush();
}
int ModbusAscii::ReadTransaction(uint8_t slaveid_, unsigned char func_, uint8_t quantity_, int16_t *ResulReg){
  bool flagkUMbSuccess = false; startTimeOut = millis(); int status_ = 1;
  while(!flagkUMbSuccess){
    if (_stream->available()) {
      char receivedData[255]; _stream->readBytesUntil('\n', receivedData, sizeof(receivedData));
      if (receivedData[0] == ':' || strlen(receivedData) == (quantity_ * 4 + 9)) {
        Serial.printf("%s\r\n", receivedData);
        if (slaveid_ == hexToByte(receivedData[1], receivedData[2])) {
            if (hexToByte(receivedData[3], receivedData[4]) == (uint8_t)func_) {
            uint8_t bytenum = hexToByte(receivedData[5], receivedData[6]);
            if (bytenum == quantity_ * 2) {
              char code[(bytenum * 2) + 8];
              strncpy(code, &receivedData[1], (bytenum * 2) + 6);
              code[(bytenum * 2) + 6] = '\0';
              if ((uint8_t)calculateLRC(code) == hexToByte(receivedData[(bytenum * 2) + 7], receivedData[(bytenum * 2) + 8])) {
                for (int i = 0; i < quantity_; i++) {
                  int16_t value = hexToWord(receivedData[(i * 4) + 7], receivedData[(i * 4) + 8], receivedData[(i * 4) + 9], receivedData[(i * 4) + 10]);
                  ResulReg[i] = value;
                }
                flagkUMbSuccess = true; status_ = 0;
              } else {status_ = 2; flagkUMbSuccess = true;}
            } else {status_ = 6; flagkUMbSuccess = true;}
          } else {status_ = 7; flagkUMbSuccess = true;}
        } else {status_ = 5; flagkUMbSuccess = true;}
      } else {status_ = 4; flagkUMbSuccess = true;}
    } else {status_ = 3;}
    if(millis() - startTimeOut > 1000){
      flagkUMbSuccess = true;
    }
  }
  return status_;
}
int ModbusAscii::WriteTransaction(uint8_t slaveid_, unsigned char func_, uint16_t Register_, uint8_t quantity_){
  bool flagkUMbSuccess = false; startTimeOut = millis(); int status_ = 1;
  while(!flagkUMbSuccess){
    if (_stream->available()) {
      char receivedData[255]; _stream->readBytesUntil('\n', receivedData, sizeof(receivedData));
      if (receivedData[0] == ':' || strlen(receivedData) == (quantity_ * 4 + 9)) {
        if (slaveid_ == hexToByte(receivedData[1], receivedData[2])) {
          if (hexToByte(receivedData[3], receivedData[4]) == (uint8_t)func_) {
            if(hexToWord(receivedData[5], receivedData[6], receivedData[7], receivedData[8]) == Register_ && hexToWord(receivedData[9], receivedData[10], receivedData[11], receivedData[12]) == quantity_){
              char code_[255]; strncpy(code_, &receivedData[1], 11); code_[11] = '\0';
              if ((uint8_t)calculateLRC(code_) == hexToByte(receivedData[12], receivedData[13])){
                status_ = 0; flagkUMbSuccess = true;
              } else {status_ = 2; flagkUMbSuccess = true;}
            } else {status_ = 6; flagkUMbSuccess = true;}
          } else {status_ = 7; flagkUMbSuccess = true;}
        } else {status_ = 5; flagkUMbSuccess = true;}
      } else {status_ = 4; flagkUMbSuccess = true;}
    } else {status_ = 3;}
    if(millis() - startTimeOut > 1000){
      flagkUMbSuccess = true;
    }
  }
  return status_;
}
unsigned char ModbusAscii::calculateLRC(const char* input) {
  int length = strlen(input);

  if (length % 2 != 0) {
    return 0xFF;
  }

  unsigned char LRC = 0;

  for (int i = 0; i < length / 2; i++) {
    unsigned char x = 0;

    if (input[i * 2] >= '0' && input[i * 2] <= '9') {
      x = input[i * 2] - '0';
    } else {
      x = input[i * 2] - 'A' + 10;
    }

    unsigned char y = 0;

    if (input[i * 2 + 1] >= '0' && input[i * 2 + 1] <= '9') {
      y = input[i * 2 + 1] - '0';
    } else {
      y = input[i * 2 + 1] - 'A' + 10;
    }

    LRC += x * 16 + y;
  }

  LRC = ~LRC + 1;

  return LRC;
}

uint8_t ModbusAscii::hexToByte(char high, char low) {
  uint8_t result = 0;

  if (high >= '0' && high <= '9') {
    result += (high - '0') * 16;
  } else {
    result += (high - 'A' + 10) * 16;
  }

  if (low >= '0' && low <= '9') {
    result += low - '0';
  } else {
    result += low - 'A' + 10;
  }

  return result;
}

uint16_t ModbusAscii::hexToWord(char high1, char low1, char high2, char low2) {
  return ((hexToByte(high1, low1) * 256) + hexToByte(high2, low2));
}
