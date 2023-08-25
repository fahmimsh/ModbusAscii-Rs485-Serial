#ifndef ModbusAscii_h
#define ModbusAscii_h

#include <Arduino.h>
#include <Stream.h>
#include <stdio.h>
/* variable untuk membaca dan menulis modbus */
#define COIL_OFF 0x0000 // Function 5 OFF request is 0x0000
#define COIL_ON 0xFF00 // Function 5 ON request is 0xFF00
#define READ_COIL 1 // Reads the ON/OFF status of discrete outputs (0X references, coils) in the slave.
#define READ_DISCRETE_INPUT 2 // Reads the ON/OFF status of discrete inputs (1X references) in the slave.
#define READ_HOLDING_REGISTERS 3 // Reads the binary contents of holding registers (4X references) in the slave.
#define READ_INPUT_REGISTERS 4 // Reads the binary contents of input registers (3X references) in the slave. Not writable.
#define WRITE_SINGLE_COIL 5 // Forces a single coil (0X reference) to either ON (0xFF00) or OFF (0x0000).
#define WRITE_SINGLE_REGISTER 6 // Presets a value into a single holding register (4X reference).
#define WRITE_MULTIPLE_COILS 15 // Forces each coil (0X reference) in a sequence of coils to either ON or OFF.
#define PRESET_MULTIPLE_REGISTERS 16 // Presets values into a sequence of holding registers (4X references).

/* variable untuk keterangan error pada modbus */
#define MB_SUKSES 0
#define MB_FRAME_ERROR 1
#define MB_CHECKSUM_ERROR 2
#define MB_TIMEOUT 3
#define MB_INVALID_DATA_ERROR 4
#define MB_ADDRESS_ERROR 5
#define MB_DATA_LENGTH_ERROR 6
#define MB_COMMUNICATION_ERROR 7

class ModbusAscii {
  public:
    ModbusAscii(Stream& stream, unsigned long baudrate, uint32_t config = SERIAL_8N1);
    void begin();
    int ReadMultiReg(uint8_t slaveid_, unsigned char func_, uint16_t startRegister_, uint8_t quantity_, int16_t* ResulReg);
    int ReadSingleReg(uint8_t slaveid_, unsigned char func_, uint16_t Register_, int16_t* ResulReg);
    int WriteSigleReg(uint8_t slaveid_, unsigned char func_, uint16_t Register_, uint16_t value_);
    int WriteMultiReg(uint8_t slaveid_, unsigned char func_, uint16_t Register_, uint8_t quantity_, uint16_t* value_);

  private:
    Stream* _stream;
    unsigned long _baudrate;
    uint32_t _config, startTimeOut;
    unsigned char calculateLRC(const char* input);
    uint8_t hexToByte(char high, char low);
    uint16_t hexToWord(char high1, char low1, char high2, char low2);
    void SendAsciiCode(unsigned char slaveID, unsigned char func, unsigned short startReg, unsigned short quantity);
    int ReadTransaction(uint8_t slaveid_, unsigned char func_, uint8_t quantity_, int16_t *ResulReg);
    int WriteTransaction(uint8_t slaveid_, unsigned char func_, uint16_t Register_, uint8_t quantity_);
};

#endif
