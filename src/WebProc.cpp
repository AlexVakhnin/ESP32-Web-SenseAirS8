//#include "WebProc.h"
#include "SPIFFS.h"
#include "Web.h" //OK

//Динамическая замена всех параметров на WEB странице "Sens"
String processor(const String& var){
//  Serial.println(var);
  /*if(var == "LEDSTATE"){  //это текст под графиком 
      return  getSwitchState(); //чем менять 
  }
  else */
  if (var == "RSSI"){
      return getRSSI();
  }
/*
  else if (var == "STATE1"){
    if (switch_flag == 0) {return "checked";}
    else { return ""; }
  }
  else if (var == "STATE2"){
    if (switch_flag == 1) {return "checked";}
    else { return ""; }
  }
  else if (var == "STATE3"){
    if (switch_flag == 2) {return "checked";}
    else { return ""; }
  }
  */
  else if (var == "freemem"){
      return String(sFreeMem); 
  }
  else if (var == "humis"){
      return String(round1(humiState),1); 
  }
  else if (var == "temps"){
      return String(round1(tempState),1); 
  }
  else if (var == "co2s"){
      return String(CO2); 
  }
 return String();
}
//Динамическая замена всех параметров на WEB странице "Manag"
String processor_serv(const String& var){
//  Serial.println(var);
  if(var == "ssid"){  //что менять
      ssid = readFile(SPIFFS, ssidPath);
      return ssid; //чем менять 
  }
  else if (var == "pass"){
      pass = readFile(SPIFFS, passPath);
      return pass ;
  }
 return String();
}
//Динамическая замена всех параметров на WEB странице "Calibr"
String processor_calibr(const String& var){
//  Serial.println(var);
  if(var == "abcper"){  //что менять
      return String(ABC); //чем менять 
  }
  else if (var == "ackreg"){
      return String(ack_reg) ;
  }
  else if (var == "calibr"){
      String fcalibr = (flagCalibr) ? "ON" : "OFF" ;
      return fcalibr;
  }
 return String();
}
//Динамическая замена всех параметров на WEB странице "Debug"
String processor_debug(const String& var){
//  Serial.println(var);
  if(var == "val_abc"){  //что менять
      return String(ABC); //чем менять 
  }
  else if (var == "val_ack"){
      return String(ack_reg) ;
  }
  else if (var == "val_status"){
      return String(status_CO2) ;
  }
  else if (var == "val_co2"){
      return String(CO2) ;
  }
  else if (var == "sfreemem"){
      return String(sFreeMem) ;
  }
  else if (var == "suptime"){
      return String(sUpTime) ;
  }
  //----------------
  else if (var == "hex_co2"){
      return _modbus_hex_CO2();
  }
  else if (var == "hex_status"){
      return _modbus_hex_status();
  }
  else if (var == "hex_abc"){
      return _modbus_hex_abc();
  }
  else if (var == "hex_ack"){
      return _modbus_hex_ack();
  }
  else if (var == "hex_calibr"){
      return _modbus_hex_calibr();
  }
  
 return String();
}
