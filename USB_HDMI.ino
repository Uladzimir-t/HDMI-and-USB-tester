/*
Что-то подсмотрена у DVI-Service
OLED SSD1315 126x32, STM32F103C6
SHRT короткое
CUT обрыв
*/
#include <Wire.h>                  // Библестека для работы i2с
#include <Adafruit_GFX.h>          // Библиотека для работе; с графикой Adafruit 
#include <Adafruit_SSD1306.h>      // Библиотека для дисплея 0.91 OLED на контроллере SSD1306 
//int state;                       // Переменная состояния

#define SCREEN_WIDTH 128  // Количество  пикселей в  ширину
#define SCREEN_HEIGHT 32  // Количество  пикселей в  высоту  

#define OLED_RESET -1     // Пин сброса  диспля (он  у нас не  используется, но нужно  указать)

Adafruit_SSD1306 display(SCREEN_WIDTH,  SCREEN_HEIGHT, &Wire, OLED_RESET); //Отправка конфигурации дисплея в библиотеку

#define NUMFLAKES 10 //Параметр не обязательней, но нужен для корректной работе; дисплея

#define DP0        PA0    // Data+ 0, например USB 2.0 или HDMI
#define DM0        PA1    // Data- 0, например USB 2.0 или HDMI                                          
#define DP1        PA2    // Data+ 1, например USB 3.0 RX+ или HDMI
#define DM1        PA3    // Data- 1, например USB 3.0 RX- или HDMI
#define DP2        PA4    // Data+ 2, например USB 3.0 TX+ или HDMI                                          
#define DM2        PA5    // Data- 2, например USB 3.0 TX- или HDMI
#define Clock_P    PA6    // Clock+ HDMI
#define Clock_M    PA7    // Clock- HDMI 
#define VCC        PB0    // Напряжение АКБ


#define USB3       PB10   // Вход детекта  USB 3.0 (логическая 1)
#define HDMI       PB11   // Вход детекта  HDMI (логическая 1)

float datam0V;                           // Переменная  значения  D0-  в вольтах
float datap0V;                           // Переменная  значения  D0+  в вольтах
float datap1V;                           // Переменная  значения  D1+  в вольтах
float datam1V;                           // Переменная  значения  D1-  в вольтах

void setup()
{
   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);        //Запуск дисплея
   display.setRotation(1);              // Повернуть экран на 90 град 
   display.clearDisplay();              // Отчистить дисплей 
   //display.setTextSize(3);            // Установить размер шрифта 
   display.setTextSize(1);              // Установить размер шрифта
   display.setTextColor(SSD1306_WHITE); // Выбор цвета текста 
   display.setCursor(9, 10);            // Начальная точка на дисплее
   display.print(F("USB"));             // Пишем на дисплее текст USB
   display.setCursor(14, 38);           // Переводим на следующую строку
   display.print(F("&"));
   display.setCursor(6, 66);            // Переводим на следующую строку
   display.print(F("HDMI"));            // Пишем на дисплее текст HDMI
   display.setCursor(1, 100);
   display.print(F("ver.2"));
   display.display();                   // Отправить в дисплей 
   pinMode(DP0,INPUT_ANALOG);           // Настройка конфигурации порта 
   pinMode(DM0,INPUT_ANALOG);           // Настройка конфигурации порта
   pinMode(DP1,INPUT_ANALOG);           // Настройка конфигурации порта 
   pinMode(DM1,INPUT_ANALOG);           // Настройка конфигурации порта
   pinMode(DP2,INPUT_ANALOG);           // Настройка конфигурации порта 
   pinMode(DM2,INPUT_ANALOG);           // Настройка конфигурации порта
   pinMode(Clock_P,INPUT_ANALOG);       // Настройка конфигурации порта 
   pinMode(Clock_M,INPUT_ANALOG);       // Настройка конфигурации порта
   pinMode(VCC,INPUT_ANALOG);
   pinMode(USB3, INPUT);
   pinMode(HDMI, INPUT);
   
   delay(800);                          //Задержка для отображения заставки
}

void loop()
{     
   if ((digitalRead(USB3) == LOW)&&(digitalRead(HDMI) == LOW)) // Не подключены USB 3.0 и HDMI
      {displayUSB ();}
   else
      {
      if (digitalRead(USB3) == HIGH) {displayUSB3 ();} // Подключен USB 3.0
      else {displayHDMI ();}                           // Подключен HDMI
      } 
   
   delay(500);                                         // Задержка между замерами
}

void displayUSB ()                     // Функциия вывода данных о линиях USB 2.0
{  
  checkDATAUSB();                      // Вызов функции проверки напряжения на портах
  display.clearDisplay();              // Отчистить дисплей 
  display.setTextSize(1);              // Установить размер шрифта
  display.setTextColor(BLACK, WHITE);  // Меняем цвета текста на инверсный
  display.setCursor(8, 1);             // Начальная точка на дисплее 
  display.print(datam1V);              // Пишем на дисплее текст напряжения на АКБ
  display.setTextColor(SSD1306_WHITE); // Возвращаем цвет текста
  display.drawLine(7, 0, 31, 0, 1);    // Рисуем линию, чтобы батарейка получилась из инверсного отображения напряжения
  display.drawLine(7, 0, 7, 8, 1);     // Рисуем линию
  display.drawLine(5, 3, 5, 5, 1);     // Рисуем линию, это пипка батарейки
  display.setCursor(9, 33);            // Начальная точка на дисплее 
  display.print(F("USB"));             // Пишем на дисплее текст USB
  display.setCursor(9, 43);            // Переводим на следующую строку
  display.print(F("2.0"));
  display.drawLine(0, 63, 31, 63, 1);  // Рисуем линию
  display.setCursor(6, 66);
  display.print(F("Data"));            // Пишем на дисплее текст
  display.setCursor(2, 76);            // Переводим на следующую строку
  display.print(F("-"));               // Пишем на дисплее текст -
  if (datam0V <= 3.29)
     {
     display.print(datam0V);           // Пишем на дисплее текст напряжеееия
     }
  else
     {
     display.print(F("CUT"));          // Пишем на дисплее ОБРЫВ
     }
  display.setCursor(2, 86);            // Переводим на следующую строку
  display.print(F("+"));               // Пишем на дисплее текст +
  if (datap0V <= 3.29)
     {
     display.print(datap0V);           // Пишем на дисплее текст напряжеееия
     }
  else
     {
     display.print(F("CUT"));          // Пишем на дисплее ОБРЫВ
     }
  display.drawLine(0, 96, 31, 96, 1);  // Рисуем линию 
  
  display.display();                   // Отправляем на дисплей всё что загрузили в память
}

void displayUSB3 ()                    // Функциия вывода данных об оставшихся линиях USB 3.0 и не только
{
  checkDATAUSB();                      // Вызов функции проверки напряжения на портах
  display.clearDisplay();              // Отчистить дисплей 
  display.setTextSize(1);              // Установить размер шрифта 
  display.setTextColor(SSD1306_WHITE); // Выбор цвета текста 
  display.setCursor(9, 0);             // Начальная точка на дисплее 
  display.print(F("USB"));             // Пишем на дисплее текст
  display.setCursor(9, 10);            // Переводим на следующую строку
  display.print(F("3.0"));             // Пишем на дисплее текст 
  display.drawLine(0, 30, 31, 30, 1);  // Рисуем линию
  display.setCursor(6, 33);            
  display.print(F("Data"));            // Пишем на дисплее текст
  display.setCursor(2, 43);            // Переводим на следующую строку
  display.print(F("-"));               // Пишем на дисплее текст -
  if (datam0V <= 3.29)
     {
     display.print(datam0V);           // Пишем на дисплее текст напряжеееия
     }
  else
     {
     display.print(F("CUT"));          // Пишем на дисплее ОБРЫВ
     } 
  display.setCursor(2, 53);            // Переводим на следующую строку
  display.print(F("+"));               // Пишем на дисплее текст +
  if (datap0V <= 3.29)
     {
     display.print(datap0V);           // Пишем на дисплее текст напряжеееия
     }
  else
     {
     display.print(F("CUT"));          // Пишем на дисплее ОБРЫВ
     }

  checkDATAUSB3();                     // Вызов функции проверки напряжения на оставшихся портах
  
  display.drawLine(0, 63, 31, 63, 1);  // Рисуем линию
  display.setCursor(12, 66);           // Начальная точка на дисплее 
  display.print(F("RX"));              // Пишем на дисплее текст
  display.setCursor(2, 76);            // Переводим на следующую строку
  display.print(F("-"));               // Пишем на дисплее текст D0- =
  if (datam0V <= 3.29)
     {
     display.print(datam0V);           // Пишем на дисплее текст напряжеееия
     }
  else
     {
     display.print(F("CUT"));          // Пишем на дисплее ОБРЫВ
     } 
  display.setCursor(2, 86);            // Переводим на следующую строку
  display.print(F("+"));               // Пишем на дисплее текст +
  if (datap0V <= 3.29)
     {
     display.print(datap0V);           // Пишем на дисплее текст напряжеееия
     }
  else
     {
     display.print(F("CUT"));          // Пишем на дисплее ОБРЫВ
     } 
  display.drawLine(0, 96, 31, 96, 1);  // Рисуем линию 
  display.setCursor(12, 99);
  display.print(F("TX"));              // Пишем на дисплее текст
  display.setCursor(2, 109);           // Переводим на следующую строку
  display.print(F("-"));               // Пишем на дисплее текст - 
  if (datam1V <= 3.29)
     {
     display.print(datam1V);           // Пишем на дисплее текст напряжеееия
     }
  else
     {
     display.print(F("CUT"));          // Пишем на дисплее ОБРЫВ
     }  
  display.setCursor(2, 119);           // Переводим на следующую строку
  display.print(F("+"));               // Пишем на дисплее текст +
  if (datap1V <= 3.29)
     {
     display.print(datap1V);           // Пишем на дисплее текст напряжеееия
     }
  else
     {
     display.print(F("CUT"));          // Пишем на дисплее ОБРЫВ
     }  
      
  display.display();                   // Отправляем на дисплей всё что загрузили в память 
}

void displayHDMI ()                     // Функциия вывода данных о линиях HDMI
{ 
  checkDATAUSB();                       // Вызов функции проверки напряжения на портах
  display.clearDisplay();               // Отчистить дисплей 
  display.setTextSize(1);               // Установить размер шрифта 
  display.setTextColor(SSD1306_WHITE);  // Выбор цвета текста 
  display.setCursor(2, 0);              // Начальная точка на дисплее 
  display.print(F("Data0"));            // Пишем на дисплее текст
  display.setCursor(2, 10);             // Переводим на следующую строку
  display.print(F("-"));                // Пишем на дисплее текст -
  if (datam0V <= 3.29)
     {
     display.print(datam0V);            // Пишем на дисплее текст напряжеееия
     }
  else
     {
     display.print(F("CUT"));           // Пишем на дисплее ОБРЫВ
     }  
  display.setCursor(2, 20);             // Переводим на следующую строку
  display.print(F("+"));                // Пишем на дисплее текст +
  if (datap0V <= 3.29)
     {
     display.print(datap0V);            // Пишем на дисплее текст напряжеееия
     }
  else
     {
     display.print(F("CUT"));           // Пишем на дисплее ОБРЫВ
     } 
  
  checkDATAUSB3();                      // Функциия вывода данных о следующих линиях HDMI

  display.drawLine(0, 30, 31, 30, 1);   // Рисуем линию
  display.setCursor(2, 33);            
  display.print(F("Data1"));            // Пишем на дисплее текст
  display.setCursor(2, 43);             // Переводим на следующую строку
  display.print(F("-"));                // Пишем на дисплее текст - 
  if (datam0V <= 3.29)
     {
     display.print(datam0V);            // Пишем на дисплее текст напряжеееия
     }
  else
     {
     display.print(F("CUT"));           // Пишем на дисплее ОБРЫВ
     }       
  display.setCursor(2, 53);             // Переводим на следующую строку
  display.print(F("+"));                // Пишем на дисплее текст + 
  if (datap0V <= 3.29)
     {
     display.print(datap0V);            // Пишем на дисплее текст напряжеееия
     }
  else
     {
     display.print(F("CUT"));           // Пишем на дисплее ОБРЫВ
     }  
      
  display.drawLine(0, 63, 31, 63, 1);   // Рисуем линию
  display.setCursor(2, 66);             // Начальная точка на дисплее 
  display.print(F("Data2"));            // Пишем на дисплее текст
  display.setCursor(2, 76);             // Переводим на следующую строку
  display.print(F("-"));                // Пишем на дисплее текст -
  if (datam1V <= 3.29)
     {
     display.print(datam1V);            // Пишем на дисплее текст напряжеееия
     }
  else
     {
     display.print(F("CUT"));           // Пишем на дисплее ОБРЫВ
     }        
  display.setCursor(2, 86);             // Переводим на следующую строку
  display.print(F("+"));                // Пишем на дисплее текст +
  if (datap1V <= 3.29)
     {
     display.print(datap1V);            // Пишем на дисплее текст напряжеееия
     }
  else
     {
     display.print(F("CUT"));           // Пишем на дисплее ОБРЫВ
     }  

  checkDATAHDMI();                      // Вызов функции проверки напряжения на последних линиях

  display.drawLine(0, 96, 31, 96, 1);   // Рисуем линию 
  display.setCursor(2, 99);
  display.print(F("Clock"));            // Пишем на дисплее текст
  display.setCursor(2, 109);            // Переводим на следующую строку
  display.print(F("-"));                // Пишем на дисплее текст - 
  if (datam0V <= 3.29)
     {
     display.print(datam0V);            // Пишем на дисплее текст напряжеееия
     }
  else
     {
     display.print(F("CUT"));           // Пишем на дисплее ОБРЫВ
     }   
  display.setCursor(2, 119);            // Переводим на следующую строку
  display.print(F("+"));                // Пишем на дисплее текст +
  if (datap0V <= 3.29)
     {
     display.print(datap0V);            // Пишем на дисплее текст напряжеееия
     }
  else
     {
     display.print(F("CUT"));           // Пишем на дисплее ОБРЫВ
     }  
     
  display.display();                    // Отправляем на дисплей всё что загрузили в память 
  
}

void checkDATAUSB()                 // Функция вычисления напряжения на выводах USB 2.0
{
int data;  
data=analogRead(DM0);               // Получение значения АЦП на пине Data0- и сохранение в переменную datam0 
datam0V=0.033*(data/40.95);         // Рассчет полученного значения АЦП с пина в вольты 
if (datam0V > 3.30) datam0V = 3.30; // Чтобы дальше не было траблов
datam0V = 3.30 - datam0V;           // Такие показания дает обычный тестер +/-
data=analogRead(DP0);               // Получение значения АЦП на пине Data0+ и сохранение в переменную datap0 
datap0V=0.033*(data/40.95);         // Рассчет полученного значения АЦП с пина в вольты
if (datap0V > 3.30) datap0V = 3.30;
datap0V = 3.30 - datap0V;
data=analogRead(VCC);               // Получение значения АЦП на пине VCC и сохранение в переменную datam1 
datam1V=0.0665*(data/40.95);        // Рассчет полученного значения АЦП с пина в вольты, делитель 1/2
}

void checkDATAUSB3()                // Функция вычисления напряжения на оставшихся выводах USB 3.0
{
int data;  
data=analogRead(DM1);               // Получение значения АЦП на пине Data1- и сохранение в переменную datam0 
datam0V=0.033*(data/40.95);         // Рассчет полученного значения АЦП с пина в вольты 
if (datam0V > 3.30) datam0V = 3.30;
datam0V = 3.30 - datam0V;
data=analogRead(DP1);               // Получение значения АЦП на пине Data1+ и сохранение в переменную datap0
datap0V=0.033*(data/40.95);         // Рассчет полученного значения АЦП с пина в вольты
if (datap0V > 3.30) datap0V = 3.30;
datap0V = 3.30 - datap0V;
data=analogRead(DM2);               // Получение значения АЦП на пине Data2- и сохранение в переменную datam0 
datam1V=0.033*(data/40.95);         // Рассчет полученного значения АЦП с пина в вольты
if (datam1V > 3.30) datam1V = 3.30;
datam1V = 3.30 - datam1V;
data=analogRead(DP2);               // Получение значения АЦП на пине Data2+ и сохранение в переменную datap0 
datap1V=0.033*(data/40.95);         // Рассчет полученного значения АЦП с пина в вольты
if (datap1V > 3.30) datap1V = 3.30;
datap1V = 3.30 - datap1V;
}

void checkDATAHDMI()                // Функция вычисления напряжения на оставшихся выводах HDMI
{
int data;
data=analogRead(Clock_M);           // Получение значения АЦП на пине
datam0V=0.033*(data/40.95);         // Рассчет полученного значения АЦП с пина в вольты 
if (datam0V > 3.30) datam0V = 3.30;
datam0V = 3.30 - datam0V;
data=analogRead(Clock_P);           // Получение значения АЦП на пине
datap0V=0.033*(data/40.95);         // Рассчет полученного значения АЦП с пина в вольты
if (datap0V > 3.30) datap0V = 3.30;
datap0V = 3.30 - datap0V;
}

