
// Import required libraries
#include <Arduino.h>
#include <WiFi.h>
//#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
//#include <ESPAsyncWebSrv.h>
#include "SPIFFS.h"
//#include "ArduinoJson.h"
#include <Ticker.h>
#include "DHTesp.h" 
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <AsyncElegantOTA.h>
//-----------------
#include "main.h"   //определяет функции во внешних файлах
//-----------------

#define DHT_SENSOR_PIN  19 // ESP32 pin GIOP19 connected to DHT22 sensor
#define DHT_SENSOR_TYPE DHT22

#define RXD2 16
#define TXD2 17

//SSD1306Wire  display(DPORT, DSDA, DSCL);
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Create dht_sensor object
DHTesp dht;

//byte switch_flag = 0; //положение переключателя на WEB странице 
//String  switch_state; //состояние переключателя в виде "g0" / "g1" / "g2"

float humiState; //влажность из DHT22
float tempState; //температура из DHT22

unsigned int CO2;  // значение CO2 из SensAir S8
unsigned long CO2_round = 0;  // используется для усреднения значения CO2
unsigned long CO2_round1 = 0;  // используется для усреднения значения CO2
byte status_CO2;    // ошибки из SensAir S8
unsigned long ABC;  // ABC period из SensAir S8
byte ack_reg;    // регистр состояния калибровки из SensAir S8

byte mout[7] = {0, 0, 0, 0, 0, 0, 0}; //MODBUS: чтение CO2: 7 байт
byte mstatus[7] = {0, 0, 0, 0, 0, 0, 0}; //MODBUS: чтение регистра ошибок: 7 байт
byte mabc[7] = {0, 0, 0, 0, 0, 0, 0}; //MODBUS: чтение регистра ABC calibration: 7 байт
byte mack[7] = {0, 0, 0, 0, 0, 0, 0}; //MODBUS: чтение состояни калибровки: 7 байт
byte mcalibr[8] = {0, 0, 0, 0, 0, 0, 0, 0}; //MODBUS: для выполнения команд калибровки: 8 байт
char hexChar[50]; //массив для sprintf() функции (50 - на строку)

unsigned long sFreeMem; //хранит колич. свободной RAM (куча)
unsigned long sUpTime = 0; //счетчик UpTime, в начале = 0 sec
unsigned long g3Time = 0; //время обновления графика 3
unsigned long g2Time = 0; //время обновления графика 2
unsigned long g1Time = 0; //время обновления графика 1

unsigned long isec = 0; //uptime: sec
unsigned long imin = 0; //uptime: min
unsigned long ihour = 0; //uptime: hour
unsigned long iday = 0; //uptime: day

int counter_2s = 0; //счетчик периода 2 сек. (опрос датчиков)
//int counter_1m = 11; //счетчик периода 1 мин.(UpTime)
int counter_2m = 0; //счетчик периода 2 мин.(форм. графикa 1h)
int counter_10m = 0; //счетчик периода 10 мин.(форм. графика 5h)

//Массивы сдвига для графиков (g0,g1,g2)
volatile unsigned int arrCO2[30] = {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0};
volatile unsigned int arrCO21[30] = {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0};
volatile unsigned int arrCO22[30] = {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0};

boolean flagAP = false; //признак, что работаем как Access Point

boolean flagCalibr = false; //признак калибровки
boolean flagABCset = false; //установка периода ABC калибровки
boolean flagABCres = false; //сброс периода ABC калибровки
boolean flagACKres = false; //сброс ACK регистра

// Search for parameter in HTTP POST request
const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pass";
const char* PARAM_REGABC = "regabc";
const char* PARAM_REGACK = "regack";
const char* PARAM_CALIBR = "calibr";

String ssid; //WIFI SSID
String pass; //WIFI PASS

const char* ssidPath = "/ssid.txt"; //SPIFFS file name "/ssid.txt"
const char* passPath = "/pass.txt"; //SPIFFS file name "/pass.txt"

const char* http_username = "admin"; //username for WEB access 
const char* http_password = "admin"; //password for WEB access


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

//Для графика
Ticker graphTicker;

/*
//Tекстовое знач. переключателя switch на web форме 
String getSwitchState() {
  String state = "3 min" ;
  if (switch_flag==1){state = "1 hour";}
  else if (switch_flag==2){state = "9 hours";} 
  return state;
}
*/

//RSSI as TEXT
String getRSSI() {
  long rssi = WiFi.RSSI();
  return String(rssi);//преобразование типов
}

// rounds a number to 2 decimal places
// example: round(3.14159) -> 3.14
double round2(double value) {
   return (int)(value * 100 + 0.5) / 100.0;
}

// rounds a number to 1 decimal places
// example: round(3.14159) -> 3.1
double round1(double value) {
   return (int)(value * 10 + 0.5) / 10.0;
}

void setup(){
  
  // Serial port for debugging purposes
  Serial.begin(115200);
  //Serial.setDebugOutput(true);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  serial2_clear();
  
  dht.setup(DHT_SENSOR_PIN, DHTesp::DHT22); // Connect DHT sensor to GPIO 19
  //dht_sensor.begin(); // initialize the DHT sensor BAD ?????

  //инициализация прерывания графиков (5 sec.)
  graphTicker.attach_ms(5000, getGraphArr);

  disp_setup(); //инициализация дисплея
  initSPIFFS(); //инициализацич SPIFFS
 
  //Читаем ssid.txt, pass.txt из файловой системы
  ssid = readFile(SPIFFS, ssidPath);
  pass = readFile(SPIFFS, passPath);
  //Serial.println(ssid);
  //Serial.println(pass);

  // Connect to Wi-Fi
  //WiFi.mode(WIFI_STA); //перевод в режим Station
  WiFi.begin(ssid.c_str(), pass.c_str());

  int count = 6;
  while (WiFi.status() != WL_CONNECTED) {
    wifi_show(count); //индикация
    count--;
    if(count <= 0){
      flagAP = true;
      WiFi.disconnect(true);
      delay(2000);
      break;  //выходим из WHILE
    }
    delay(1000);
    Serial.println("Connecting to WiFi: "+ssid+" "+pass+"..");
  }
    
if (flagAP){
  
  //создаем AP
  Serial.println("Creating Access Point..");
  //WiFi.mode(WIFI_AP); //перевод в режим AP  
  WiFi.softAP("Soft-AP-ESP32", "11111111"); // NULL sets an open Access Point
  //delay(500);
  //WiFi.softAPConfig(IP, gateway, subnet);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");Serial.println(IP);
  ip_show("ModAP",IP);
} else {
  Serial.println(WiFi.localIP()); // Print ESP32 Local IP Address
  Serial.println("WiFi.RSSI: "+String(WiFi.RSSI())); //Уровень сигнала 
  ip_show("ModSTA",WiFi.localIP());
}
  
  Serial.println("CpuFrequencyMhz: "+String(getCpuFrequencyMhz()));

  // Route for root / web page
  web_init();
  AsyncElegantOTA.begin(&server); // Start AsyncElegantOTA  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Start server
  server.begin();


  delay(5000);
  //seeTicker.once_ms(5000, start_show); //запускаем индикацию с задержкой 5с.
  
}//end of setup()

//==========================================================================================


//БЕСКОНЕЧНЫЙ ЦИКЛ ;) 
void loop(){
/*
    if ( counter_5s > 0 ){ counter_5s--;   }
  else { counter_5s = 4; handler_5s_job(); }  //событие через 5 сек
*/  

    if ( counter_2s > 0 ){ counter_2s--;   }
  else { counter_2s = 1; handler_2s_job(); }  //событие через 2 сек



  handler_1s_job();  // событие 1 раз в секунду

 // wait a 1 seconds between readings
  delay(1100);
  
//==========================================================================================

  
}
