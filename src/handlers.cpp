//#include <Arduino.h>
#include "DHTesp.h" 
#include "Web.h"  //внешние функции
//void abc_reset();
//void abc_set180();
//void ack_reset();
//void CO2_read();
//void abc_read();
//void ack_read();
//void status_read();
//void disp_show();

//ОБРАБОТЧИК РАЗ В 1 СЕК 
void handler_1s_job() {
  //Serial.println("Handle job -> 1 sec");
  sFreeMem = ESP.getFreeHeap();  //покажем память  
}

//ОБРАБОТЧИК РАЗ В 2 СЕК (ОСНОВНОЙ)
void handler_2s_job() {
//Serial.println("Handle job -> 2 sec");

  // Get Data from the Sensor
if (flagABCset) {
  abc_set180();  //установка регистра ABC
  delay(1000);
  flagABCset = false;
}
if (flagABCres) {
  abc_reset();  //сброс регистра ABC
  delay(1000);
  flagABCres = false;
}
if (flagACKres) {
  ack_reset();  //сброс регистра ACK
  flagACKres = false;
}
  
if (flagCalibr){
  //была команда калибровки фоном
  ack_reset();
  calibration(); //debug -> (в реале разремить..)
  delay(2000);
  flagCalibr = false;
  //ack_reg = 32;  //debug -> (в реале заремить..)
  
} else {
  //штатный режим опроса
  CO2_read();
  status_read();
  abc_read();
  ack_read();
}

//--------------------------------- ТУТ ЧИТАЕМ DHT22--------------------

  TempAndHumidity newValues = dht.getTempAndHumidity();
  
  if (dht.getStatus() == 0) {
      humiState = newValues.humidity;
      tempState = newValues.temperature;
  } else {
      humiState = 0; //0-значит ошибка чтения DHT22
      tempState = 0;
  }
  
//----------------------------------------------------------------------

  disp_show();  //отображение на дисплее

}
