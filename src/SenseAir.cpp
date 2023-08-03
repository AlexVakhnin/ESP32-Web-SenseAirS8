//#include <Arduino.h>
//#include "SenseAir.h"
#include "Web.h"

//MODBAS: чтение регистра CO2
void CO2_read(){
  byte req[] = {0xFE, 0x04, 0x00, 0x03, 0x00, 0x01, 0xD5, 0xC5}; //8 байт
  if (modbus_command(req, mout, 7)){
    CO2 = CO2count_mout();
    //Serial.println(_modbus_hex_CO2());
  } else {
    Serial.println("Error exexuting CO2_command...");
    Serial.println(_modbus_hex_CO2());
  }
}

//MODBAS: чтение регистра STATUS (ошибки)
//в переменную status_CO2 
void status_read(){
  byte req[] = {0xFE, 0x04, 0x00, 0x00, 0x00, 0x01, 0x25, 0xC5}; //8 байт
  if (modbus_command(req, mstatus, 7)){
    status_CO2 = mstatus[4];
  } else {
    Serial.println("Error exexuting status_command...");
    Serial.println(_modbus_hex_status());
  }
}
//MODBAS: чтение регистра калибровки ABC
//в переменную ABC
void abc_read(){
  byte req[] = {0xFE, 0x03, 0x00, 0x1F, 0x00, 0x01, 0xA1, 0xC3}; //8 байт
  if (modbus_command(req, mabc, 7)){
    ABC = ABCcount_mabc();
  } else {
    Serial.println("Error exexuting abc_read_command...");
    Serial.println(_modbus_hex_abc());
  }
}
//MODBAS: чтение регистра состояния калибровки  ACK
//в переменную ack_reg 
void ack_read(){

  byte req[] = {0xFE, 0x03, 0x00, 0x00, 0x00, 0x01, 0x90, 0x05}; //8 байт
  if (modbus_command(req, mack, 7)){
    ack_reg = mack[4];  //должно быть 20h при успешной калибровке (бит5=1)
  } else {
    Serial.println("Error exexuting ack_read_command...");
    Serial.println(_modbus_hex_ack());
  }
}

//команды на установку регистров 
void calibration(){

  byte req[] = {0xFE, 0x06, 0x00, 0x01, 0x7C, 0x06, 0x6C, 0xC7}; //8 байт
  if (modbus_command(req, mcalibr,8)){
    Serial.println(_modbus_hex_calibr()+" Calibration Background..");
  } else {
    Serial.println("Error exexuting calibration_command...");
    Serial.println(_modbus_hex_calibr());
  }
}
void ack_reset(){

  byte req[] = {0xFE, 0x06, 0x00, 0x00, 0x00, 0x00, 0x9D, 0xC5}; //8 байт
  if (modbus_command(req, mcalibr, 8)){
    Serial.println(_modbus_hex_calibr()+" Ack_Reset command..");
  } else {
    Serial.println("Error exexuting ack_reset_command...");
    Serial.println(_modbus_hex_calibr());
  }
}
void abc_set180(){

  byte req[] = {0xFE, 0x06, 0x00, 0x1F, 0x00, 0xB4, 0xAC, 0x74}; //8 байт
  if (modbus_command(req, mcalibr, 8)){
    Serial.println(_modbus_hex_calibr()+" ABC_Set_180 command..");
  } else {
    Serial.println("Error exexuting abc_set_180_command...");
    Serial.println(_modbus_hex_calibr());
  }
}

void abc_reset(){

  byte req[] = {0xFE, 0x06, 0x00, 0x1F, 0x00, 0x00, 0xAC, 0x03}; //8 байт
  if (modbus_command(req, mcalibr, 8)){
    Serial.println(_modbus_hex_calibr()+" ABC_Reset command..");
  } else {
    Serial.println("Error exexuting abc_reset_command...");
    Serial.println(_modbus_hex_calibr());
  }
}

//результаты ответов modbus
//-------------------------------------------------------------
String _modbus_hex_calibr(){  //hex mcalibr[] string
   //if(sizeof(mcalibr) == 8){
      sprintf(hexChar,"%02X %02X %02X %02X %02X %02X %02X %02X ",mcalibr[0],mcalibr[1],mcalibr[2],mcalibr[3],mcalibr[4],mcalibr[5],mcalibr[6],mcalibr[7]);
      return hexChar;
   //} else {
    //  return String();
   //}
}
String _modbus_hex_CO2(){  //hex CO2 string
   //if(size_t(mout) == 7){
      sprintf(hexChar,"%02X %02X %02X %02X %02X %02X %02X ",mout[0],mout[1],mout[2],mout[3],mout[4],mout[5],mout[6]);
      return hexChar;
   //} else {
    //  return String();
   //}
}
String _modbus_hex_status(){  //hex mstatus[] string
   //if(size_t(mstatus) == 7){
      sprintf(hexChar,"%02X %02X %02X %02X %02X %02X %02X ",mstatus[0],mstatus[1],mstatus[2],mstatus[3],mstatus[4],mstatus[5],mstatus[6]);
      return hexChar;
  // } else {
   //   return String();
   //}
}
String _modbus_hex_abc(){  //hex mabc[] string
   //if(size_t(mabc) == 7){
      sprintf(hexChar,"%02X %02X %02X %02X %02X %02X %02X ",mabc[0],mabc[1],mabc[2],mabc[3],mabc[4],mabc[5],mabc[6]);
      return hexChar;
   //} else {
    //  return String();
   //}
}
String _modbus_hex_ack(){  //hex mack[] string
   //if(sizeof(mack) == 7){
      sprintf(hexChar,"%02X %02X %02X %02X %02X %02X %02X ",mack[0],mack[1],mack[2],mack[3],mack[4],mack[5],mack[6]);
      return hexChar;
   //} else {
   //  return String();
   //}
}
//-----------------------------------------------------------

void serial2_clear() {

      while (Serial2.available())  //очищаем буфер приема
      Serial2.read();  
      }

//выполняем modbus зарос, результат false - ошибка, передаем ссылку на массив-запрос 8 байт.
//результат - заполняем глобальный массив mout[7]

boolean modbus_command(byte mreq[], byte aout[], int count)
{
  boolean mresult = true;
  while (Serial2.available())  //очищаем буфер приема
     Serial2.read();
  Serial2.write(mreq, 8);   //посылаем команду 8 байт из массива
  int timeout = 0;
  while (Serial2.available() < count )  //ожидаем в течении 500мс появление 7(8) байт на прием
  {
    timeout++;
    if (timeout > 10)              //если превышен таймаут (500мс), а в буфере приема нет 7 байт
    {
      mresult = false;
      break;                       //и выходим из цикла ожидания 7 байт...типа сбой обмена
    }
    delay(50);
  }
  for (int i = 0; i < count; i++)      //читаем 7(8) байт в массив без проверки
  {
    aout[i] = Serial2.read();
  }
return mresult;  
}


//перевод в десятичный формат

unsigned int CO2count_mout()
{
  int high = mout[3];
  int low = mout[4];
  unsigned long val = high * 256 + low;
  return val * 1; // S8 = 1. K-30 3% = 3, K-33 ICB = 10
}
unsigned long ABCcount_mabc()
{
  int high = mabc[3];
  int low = mabc[4];
  unsigned long val = high * 256 + low;
  return val * 1; // S8 = 1. K-30 3% = 3, K-33 ICB = 10
}
