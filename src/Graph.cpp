
#include "Web.h"  //внешние функции

//вычислить uptime д/ч/м/с (вызывается с периодом 5 сек)
void get_uptime(){
    sUpTime+=5;  //прибавляем 5 сек
    auto n=sUpTime; //количество всех секунд
    isec = n % 60;  //остаток от деления на 60 (секунд в минуте)
    n /= 60; //количество всех минут (целая часть)  
    imin = n % 60;  //остаток от деления на 60 (минут в часе)
    n /= 60; //количество всех часов (целая часть)
    ihour = n % 24; //остаток от деления на 24 (часов в дне)
    iday = n/24; //количество всех дней (целая часть)
}

//сдвиг элементов массива влево с добавлением нового значения
//push_arr( arrCO2,elements, CO2 )
void push_arr( volatile unsigned int arr[], int elem, unsigned int n )
{
  for (int i=elem-1;i>0;i--)
    {
    //Serial.print(String(i)+",");
    arr[i] = arr[i-1];
    }
  arr[0] = n;
  //Serial.println(""); 
}

//получение данных для графиков, расчет д/ч/м UpTime поминутно
//функция вызывается с периодичностью  5 sec.
//по прерыванию аппаратным таймером.
void getGraphArr()
{

  if ( counter_10m > 0 ){counter_10m--;}
  else { 
    counter_10m = 215; //----------------119->10 min->5 hours---------------(215->18 min->9 hours)
    
    //график №3
    //Serial.println("10m_push-> "+String(CO2_round1)+"/120="+String(CO2_round1/120)); //DEBUG.........
    CO2_round1 = CO2_round1 / 216;
    int elem = sizeof(arrCO22) / sizeof(arrCO22[0]);//колич. точек X графика
    push_arr( arrCO22, elem, CO2_round1 );//сдвиг массива arrCO21 влево
    g3Time = sUpTime; //метка последнего обновления графика 3 
    CO2_round1 = 0; //новый цикл усреднения
  }


  if ( counter_2m > 0 ){counter_2m--;}
  else { 
    counter_2m = 23; //------------------------------------ 2 min period->1h--------------
    
    //график №2
    //Serial.println("2m_push-> "+String(CO2_round)+"/24="+String(CO2_round/24)); //DEBUG.........
    CO2_round = CO2_round / 24;
    int elem = sizeof(arrCO21) / sizeof(arrCO21[0]);//колич. точек X графика
    push_arr( arrCO21, elem, CO2_round );//сдвиг массива arrCO21 влево 
    g2Time = sUpTime; //метка последнего обновления графика 2 
    CO2_round = 0; //новый цикл усреднения
  }

  //график №1 (5 sec period)
  int elements = sizeof(arrCO2) / sizeof(arrCO2[0]);//колич. точек X графика
  push_arr( arrCO2, elements, CO2 );//сдвиг массива arrCO2 влево

  get_uptime(); //вычислить uptime д/ч/м/c

  CO2_round +=CO2; //сумматор для усреднения
  CO2_round1 +=CO2;//6666;//CO2; //сумматор для усреднения
   //Serial.println("CO2_round="+String(CO2_round)); //DEBUG.........
   //Serial.println("CO2_round1="+String(CO2_round1)); //DEBUG.........
}
