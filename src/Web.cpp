//#include <Arduino.h>
#include "ArduinoJson.h"
//#include <ESPAsyncWebServer.h>
//#include "SPIFFS.h"
#include "Web.h"  //внешние функции

void onConnectBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{


StaticJsonDocument<200> doc;

auto error = deserializeJson(doc, data);
if (error) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    return;
}
JsonObject root = doc.as<JsonObject>(); //у нас не массив , а обьект

for (JsonPair keyValue : root) { //проход о всем парам обьекта
  if (keyValue.key() == PARAM_INPUT_1){
    ssid = keyValue.value().as<const char*>();
    Serial.print("SSID set to: ");
    Serial.println(ssid);
    writeFile(SPIFFS, ssidPath, ssid.c_str());    
  }
  if (keyValue.key() == PARAM_INPUT_2){
    pass = keyValue.value().as<const char*>();
    Serial.print("Password set to: ");
    Serial.println(pass);
    writeFile(SPIFFS, passPath, pass.c_str());    
  }
    if (keyValue.key() == PARAM_REGABC){
      String val = keyValue.value().as<const char*>();
      Serial.println("POST: RegABC ->"+val);
      if (val =="set") {
        flagABCset = true; 
        Serial.println("flagABCset = true");   
      } else if (val =="res"){
        flagABCres = true;
        Serial.println("flagABCres = true");
      } else {
        Serial.println("Error: Value Unknown");
      }
    }
    if (keyValue.key() == PARAM_REGACK){
      String val = keyValue.value().as<const char*>();
      Serial.println("POST: RegACK ->"+val);
      if (val =="res") {
        flagACKres = true; 
        Serial.println("flagACKres = true");   
      } else {
        Serial.println("Error: Value Unknown");
      }
    }
    if (keyValue.key() == PARAM_CALIBR){
      String val = keyValue.value().as<const char*>();
      Serial.println("POST: Calibr ->"+val);
      if (val =="start") {
        flagCalibr = true; 
        Serial.println("flagCalibr = true");   
      } else {
        Serial.println("Error: Value Unknown");
      }
    }

 }
 
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}


void web_init(){

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  server.on("/wifimanager.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/wifimanager.html", String(), false, processor_serv);
  });
  server.on("/calibration.html", HTTP_GET, [](AsyncWebServerRequest *request){
    //if(!request->authenticate(http_username, http_password))  // страница требует аутентификации !
    //  return request->requestAuthentication();                
    request->send(SPIFFS, "/calibration.html", String(), false, processor_calibr);
  });
  
//POST
//---------------------------------------------------------------------------

server.on("/posts", HTTP_POST, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain","POSTED SUCCESSFULLY");
}, NULL, onConnectBody);

//---------------------------------------------------------------------------
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to load Chart.min.js file
  server.on("/Chart.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/Chart.min.js", "text/javascript");
    response->addHeader("Cache-Control","max-age=3600"); /*very important !!!!!*/
    request->send(response);
  });

  //Это ответ сервера на AJAX запрос из WEB страницы "Sens"
  server.on("/jsonstate", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncResponseStream *response = request->beginResponseStream("application/json");    
    const int capacity = JSON_OBJECT_SIZE(11);//Количество живых параметров JSON
    StaticJsonDocument<capacity> doc;
    doc["rssi"] = WiFi.RSSI();
    doc["humis"] = round2(humiState);
    doc["temps"] = round2(tempState);
    doc["co2"] = CO2;
    doc["freemem"] = sFreeMem;  //или String().c_str();???????
    doc["iday"] = iday;
    doc["ihour"] = ihour;
    doc["imin"] = imin;
    doc["isec"] = isec;
    doc["g3diff"] = sUpTime - g3Time;
    doc["g2diff"] = sUpTime - g2Time;
    serializeJson(doc, *response);
    request->send(response);
  });

  //Это ответ сервера на AJAX запрос для калибровки
  server.on("/jsoncalibr", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncResponseStream *response = request->beginResponseStream("application/json");    
    const int capacity = JSON_OBJECT_SIZE(3);//Количество живых параметров = 3
    StaticJsonDocument<capacity> doc;
    doc["calibr"] = (flagCalibr) ? "ON" : "OFF" ;
    doc["ackreg"] = ack_reg;
    doc["abcper"] = ABC;
    serializeJson(doc, *response);
    request->send(response);
  });


  // переключатель графиков -> график №3
  server.on("/g2_on", HTTP_GET, [](AsyncWebServerRequest *request){
    //switch_flag = 2;
    AsyncResponseStream *response = request->beginResponseStream("application/json");    
    const int capacity = JSON_OBJECT_SIZE(30);//Количество живых параметров = 30
    StaticJsonDocument<capacity> doc;
        for (int i = 29; i >= 0; i--) {  //график №3 в JSON
          doc.add( arrCO22[i] ); //simply array !!!
        }
    serializeJson(doc, *response);
    request->send(response);
  });
  // переключатель графиков -> график №2
  server.on("/g1_on", HTTP_GET, [](AsyncWebServerRequest *request){
    //switch_flag = 1;
    AsyncResponseStream *response = request->beginResponseStream("application/json");    
    const int capacity = JSON_OBJECT_SIZE(30);//Количество живых параметров = 30
    StaticJsonDocument<capacity> doc;
        for (int i = 29; i >= 0; i--) {  //график №2 в JSON
          doc.add( arrCO21[i] ); //simply array !!!
        }
    serializeJson(doc, *response);
    request->send(response);
  });
  // переключатель графиков -> график №1
  server.on("/g0_on", HTTP_GET, [](AsyncWebServerRequest *request){
    //switch_flag = 0;
    AsyncResponseStream *response = request->beginResponseStream("application/json");    
    const int capacity = JSON_OBJECT_SIZE(30);//Количество живых параметров = 30
    StaticJsonDocument<capacity> doc;
        for (int i = 29; i >= 0; i--) {  //график №1 в JSON
          doc.add( arrCO2[i] ); //simply array !!!
        }
    serializeJson(doc, *response);
    request->send(response);    
  });

  //Для Графаны
  server.on("/metrics", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "esp32_freemem "+String(sFreeMem)+"\nesp32_uptime "+String(millis())+"\nesp32_co2dach "+String(CO2));//+String(millis());
  });

  
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/debuginfo.html", String(), false, processor_debug);
  });
  
  server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request){    
    request->send(200, "text/html", "<h1>SOFT RESET..</h1>");
    delay(3000);
    ESP.restart();
  });

server.onNotFound(notFound);
  
}
