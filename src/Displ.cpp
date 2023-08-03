//#include <Arduino.h>
#include <Adafruit_SSD1306.h>
//void ip_show(String mod, IPAddress ip);
//void wifi_show(int i);
extern Adafruit_SSD1306 display;
double round1(double value);
extern float humiState;  //For sensor DHT22
extern float tempState;
extern unsigned long CO2;  // For SensAir S8
//void disp_setup();

/*
void start_show(){
  dispTicker.attach_ms(1000, disp_show); //инициализация прерывания дисплея (1 sec.)  
}
*/

//Работа с дисплеями

void ip_show(String mod, IPAddress ip) {

  display.clearDisplay();

  display.setCursor(2, 0);
  display.setTextSize(2);  
  display.println(mod);

  display.setCursor(64+32, 0);
  display.println("IP");  

  display.setCursor(2, 24);
  display.setTextSize(2);  
  String s = ".."+String(ip[2])+"."+String(ip[3]);
  display.println(s);
  display.display();
}

void wifi_show(int i) {

  display.clearDisplay();

  display.setCursor(2, 0);
  display.setTextSize(2);  
  display.println("ModSTA");

  display.setCursor(64+32, 0);
  display.println(String(i));  

  display.setCursor(2, 24);
  display.setTextSize(5);  
  display.println("WIFI");
  display.display();
}


void disp_show() {

  display.clearDisplay();

  display.setCursor(2, 0);
  display.setTextSize(2);  
  display.println(String(round1(tempState),1));

  display.setCursor(64+4, 0);
  display.println(String(round1(humiState),1)+"%");
  
  display.setCursor(2, 24);
  display.setTextSize(5);  
  display.println(CO2);
  display.display();

}

void disp_setup(){
  
  // Setup display SSD1306
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
 // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();

  display.setTextColor(WHITE);
  
}
