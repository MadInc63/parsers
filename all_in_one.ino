//Подключаемые библиотеки
#include "LiquidCrystal_I2C.h"        // подключаем библиотеку для работы с LCD дисплеем
#include "Wire.h"                     // подключаем библиотеку для работы с шиной I2C
#include "DHT.h"                      // подключаем библиотеку для работы с DHT модулем
#include "iarduino_RTC.h"             // подключаем библиотеку для работы с RTC модулем
#include "EEPROM.h"                   // подключаем библиотеку для работы с EEPROM модулем
#include "LCDMenuLib.h"               // подключаем библиотеку Меню

#define _LCDML_DISP_cfg_button_press_time   200     // задержка нажатие кнопки в мс
#define _LCDML_DISP_cfg_scrollbar           1       // показывать полосу прокрутки
#define _LCDML_DISP_cfg_cursor              0x7E    // символ курсора
#define _LCDML_DISP_cols                    20      // количество столбцов экрана
#define _LCDML_DISP_rows                    4       // количество строк экрана
#define _LCDML_CONTROL_digital_low_active   1       // 0 = LOW active (кнопка притянута реистором к минусу), 1 = HIGH active (кнопка притянута резистором к плюсу)
#define _LCDML_CONTROL_digital_enable_quit  1       // 0 = Кнопка Выход неиспользуеться, 1 = Кнопка Выход используеться
#define _LCDML_CONTROL_digital_enable_lr    1       // 0 = Кнопка Лево и Право не ипользуються, 1= Кнопки Лево и Право используються
#define _LCDML_CONTROL_digital_enter        6       // указываем номер вывода arduino, к которому подключена кнопка Вход
#define _LCDML_CONTROL_digital_up           3       // указываем номер вывода arduino, к которому подключена кнопка Вверх
#define _LCDML_CONTROL_digital_down         4       // указываем номер вывода arduino, к которому подключена кнопка Вниз
#define _LCDML_CONTROL_digital_quit         7       // указываем номер вывода arduino, к которому подключена кнопка Выход
#define _LCDML_CONTROL_digital_left         2       // указываем номер вывода arduino, к которому подключена кнопка Лево
#define _LCDML_CONTROL_digital_right        5       // указываем номер вывода arduino, к которому подключена кнопка Право
#define RTC_DAT                             8       // указываем номер вывода DAT модуля часов DS1302 
#define RTC_CLK                             10      // указываем номер вывода CLK модуля часов DS1302
#define RTC_RST                             9       // указываем номер вывода RST модуля часов DS1302
#define DHTPIN                              11      // указываем номер вывода DHT модуля датчика DHT22

const uint8_t scroll_bar[5][8] =
 {
    {B10001, B10001, B10001, B10001, B10001, B10001, B10001, B10001}, // scrollbar top
    {B11111, B11111, B10001, B10001, B10001, B10001, B10001, B10001}, // scroll state 1 
    {B10001, B10001, B11111, B11111, B10001, B10001, B10001, B10001}, // scroll state 2
    {B10001, B10001, B10001, B10001, B11111, B11111, B10001, B10001}, // scroll state 3
    {B10001, B10001, B10001, B10001, B10001, B10001, B11111, B11111}  // scrollbar bottom
  }; 

uint8_t temp_cel[8] =
{
  B00111,B00101,B00111,B00000,B00000,B00000,B00000
};
// create menu
// menu element count - last element id
// this value must be the same as the last menu element
#define _LCDML_DISP_cnt    15

// LCDML_root        => layer 0 
// LCDML_root_X      => layer 1 
// LCDML_root_X_X    => layer 2 
// LCDML_root_X_X_X  => layer 3 
// LCDML_root_...    => layer ... 
  
// LCDMenuLib_add(id, group, prev_layer_element, new_element_num, lang_char_array, callback_function)
LCDML_DISP_init(_LCDML_DISP_cnt);
LCDML_DISP_add      (0  , _LCDML_G1  , LCDML_root        , 1  , "Information"        , LCDML_FUNC_information);
LCDML_DISP_add      (1  , _LCDML_G1  , LCDML_root        , 2  , "Manual"             , LCDML_FUNC);
LCDML_DISP_add      (2  , _LCDML_G1  , LCDML_root_2      , 1  , "Watering"           , LCDML_FUNC_set_watering_time);
LCDML_DISP_add      (3  , _LCDML_G1  , LCDML_root_2      , 2  , "Filling"            , LCDML_FUNC_set_filling_time);
LCDML_DISP_add      (4  , _LCDML_G1  , LCDML_root        , 3  , "Auto"               , LCDML_FUNC);
LCDML_DISP_add      (5  , _LCDML_G1  , LCDML_root        , 4  , "Settings"           , LCDML_FUNC);
LCDML_DISP_add      (6  , _LCDML_G1  , LCDML_root_3      , 1  , "Change value"       , LCDML_FUNC);
LCDML_DISP_add      (7  , _LCDML_G1  , LCDML_root_3      , 2  , "Something"          , LCDML_FUNC);
LCDML_DISP_add      (8  , _LCDML_G1  , LCDML_root        , 5  , "Program"            , LCDML_FUNC);
LCDML_DISP_add      (9  , _LCDML_G1  , LCDML_root_4      , 1  , "Program 1"          , LCDML_FUNC);
LCDML_DISP_add      (10 , _LCDML_G1  , LCDML_root_4_1    , 1  , "P1 start"           , LCDML_FUNC);
LCDML_DISP_add      (11 , _LCDML_G1  , LCDML_root_4_1    , 2  , "Settings"           , LCDML_FUNC);
LCDML_DISP_add      (12 , _LCDML_G1  , LCDML_root_4_1_2  , 1  , "Warm"               , LCDML_FUNC);
LCDML_DISP_add      (13 , _LCDML_G1  , LCDML_root_4_1_2  , 2  , "Long"               , LCDML_FUNC);
LCDML_DISP_add      (14 , _LCDML_G1  , LCDML_root_4      , 2  , "Set Time and Data"  , LCDML_FUNC_set_time);
LCDML_DISP_add      (15 , _LCDML_G1  , LCDML_root        , 6  , "Last Point"         , LCDML_FUNC);
LCDML_DISP_createMenu(_LCDML_DISP_cnt);

// ********************************************************************* 
// LCDML BACKEND (core of the menu, do not change here anything yet)
// ********************************************************************* 
// define backend function  
#define _LCDML_BACK_cnt    1  // last backend function id
  
LCDML_BACK_init(_LCDML_BACK_cnt);
LCDML_BACK_new_timebased_dynamic (0  , ( 20UL )         , _LCDML_start  , LCDML_BACKEND_control);
LCDML_BACK_new_timebased_dynamic (1  , ( 1000UL )       , _LCDML_stop   , LCDML_BACKEND_menu);
LCDML_BACK_create();

DHT dht(DHTPIN, DHT22);
LiquidCrystal_I2C lcd(0x3F,20,4);     // объявляем  переменную для работы с LCD дисплеем, указывая параметры дисплея (адрес I2C = 0x3F, количество столбцов = 20, количество строк = 4)
iarduino_RTC time(RTC_DS1302, RTC_DAT, RTC_CLK, RTC_RST);   // объявляем  переменную для работы с RTC модулем,  указывая название модуля (модель часов RTC_DS1302, номер вывода DAT, номер вывода CLK, номер вывода RST)

void setup()
{

  delay (300);                        // ждем 300мс
  dht.begin();
  lcd.begin();                        // инициируем LCD дисплей
  lcd.backlight();                    // включаем подсветку LCD дисплея
  time.begin();
  lcd.home ();
  // set special chars for scrollbar
  lcd.createChar(0, (uint8_t*)scroll_bar[0]);
  lcd.createChar(1, (uint8_t*)scroll_bar[1]);
  lcd.createChar(2, (uint8_t*)scroll_bar[2]);
  lcd.createChar(3, (uint8_t*)scroll_bar[3]);
  lcd.createChar(4, (uint8_t*)scroll_bar[4]);
  lcd.createChar(5, temp_cel);
  lcd.setCursor(0,0);
  lcd.print(F("booting"));
  // Enable all items with _LCDML_G1
  LCDML_DISP_groupEnable(_LCDML_G1); // enable group 1
  // LCDMenu Setup
  LCDML_setup(_LCDML_BACK_cnt);
}

void loop()
{
LCDML_run(_LCDML_priority); 
}


// ====================================================================================================================================================================
//
// Функции вывода на Экран
//
// ====================================================================================================================================================================

void LCDML_lcd_menu_display()
{
  if (LCDML_BUTTON_checkAny()) // check if any button is pressed (enter, up, down, left, right)
    {
      if (LCDML_BUTTON_checkEnter()) // check if button left is pressed
    {
      LCDML_BUTTON_resetLeft();
  lcd.home ();                                                                    // go home
  if (LCDML_DISP_update())                                                        // check if menu needs an update
    {                                                                             // init vars
      uint8_t n_max             = (LCDML.getChilds() >= _LCDML_DISP_rows) ? _LCDML_DISP_rows : (LCDML.getChilds());
      uint8_t scrollbar_min     = 0;
      uint8_t scrollbar_max     = LCDML.getChilds();
      uint8_t scrollbar_cur_pos = LCDML.getCursorPosAbs();
      uint8_t scroll_pos        = ((1.*n_max * _LCDML_DISP_rows) / (scrollbar_max - 1) * scrollbar_cur_pos);
      if (LCDML_DISP_update_content())                                            // update content
        {
          LCDML_lcd_menu_clear();                                                 // clear menu
          for (uint8_t n = 0; n < n_max; n++)                                     // display rows
            {
              lcd.setCursor(1, n);                                                // set cursor
              lcd.print(LCDML.content[n]);                                        // set content
            }
        }
      if (LCDML_DISP_update_cursor())                                             // update cursor and scrollbar
        {
          for (uint8_t n = 0; n < n_max; n++)                                     // display rows
            {
              lcd.setCursor(0, n);                                                //set cursor
              if (n == LCDML.getCursorPos())                                      //set cursor char
                {
                  lcd.write(_LCDML_DISP_cfg_cursor);
                } 
              else
                {
                  lcd.write(' ');
                }
              if (_LCDML_DISP_cfg_scrollbar == 1)                                 // delete or reset scrollbar
                {
                  if (scrollbar_max > n_max)
                    {
                      lcd.setCursor((_LCDML_DISP_cols - 1), n);
                      lcd.write((uint8_t)0);
                    }
                  else
                    {
                      lcd.setCursor((_LCDML_DISP_cols - 1), n);
                      lcd.print(' ');
                    }
                }
            }
          if (_LCDML_DISP_cfg_scrollbar == 1)                                     // display scrollbar
            {
              if (scrollbar_max > n_max)
                {
                  if (scrollbar_cur_pos == scrollbar_min)                         //set scroll position
                    {
                      lcd.setCursor((_LCDML_DISP_cols - 1), 0);                   // min pos
                      lcd.write((uint8_t)1);
                    }
                  else if (scrollbar_cur_pos == (scrollbar_max - 1)) 
                    {
                      lcd.setCursor((_LCDML_DISP_cols - 1), (n_max - 1));         // max pos
                      lcd.write((uint8_t)4);
                    }
                  else
                    {
                      lcd.setCursor((_LCDML_DISP_cols - 1), scroll_pos / n_max);  // between
                      lcd.write((uint8_t)(scroll_pos % n_max) + 1);
                    }
                }
            }
        }
    }
  LCDML_DISP_update_end();                                                        // reinit some vars
}
}
}

// lcd clear
void LCDML_lcd_menu_clear()
{
  lcd.clear();
  lcd.setCursor(0, 0);
}




/* ===================================================================== *
 *                                                                       *
 * DISPLAY SYSTEM                                                        *
 *                                                                       *
 * ===================================================================== *
 * every "disp menu function" needs three functions 
 * - void LCDML_DISP_setup(func_name)    
 * - void LCDML_DISP_loop(func_name)     
 * - void LCDML_DISP_loop_end(func_name)
 *
 * EXAMPLE CODE:
    void LCDML_DISP_setup(..menu_func_name..) 
    {
      // setup
      // is called only if it is started

      // starts a trigger event for the loop function every 100 millisecounds
      LCDML_DISP_triggerMenu(100);  
    }
    
    void LCDML_DISP_loop(..menu_func_name..)
    { 
      // loop
      // is called when it is triggert
      // - with LCDML_DISP_triggerMenu( millisecounds ) 
      // - with every button status change

      // check if any button is presed (enter, up, down, left, right)
      if(LCDML_BUTTON_checkAny()) {         
        LCDML_DISP_funcend();
      } 
    }
    
    void LCDML_DISP_loop_end(..menu_func_name..)
    {
      // loop end
      // this functions is ever called when a DISP function is quit
      // you can here reset some global vars or do nothing  
    } 
 * ===================================================================== *
 */


// *********************************************************************
/*void LCDML_DISP_setup(LCDML_FUNC_timer_info)
// *********************************************************************
{
  // setup function   
  lcd.print(F("x sec warten")); // print some content on first row  
  g_func_timer_info = 10;       // reset and set timer    
  LCDML_DISP_triggerMenu(100);  // starts a trigger event for the loop function every 100 millisecounds
}

void LCDML_DISP_loop(LCDML_FUNC_timer_info)
{ 
  // this timer checks every 1000 millisecounds if it is called
  if((millis() - g_timer_1) >= 1000) {
    g_timer_1 = millis();   
    g_func_timer_info--;                // increment the value every secound
    lcd.setCursor(0, 0);                // set cursor pos
    lcd.print(g_func_timer_info);       // print the time counter value
  }
  
  // reset the initscreen timer
  LCDML_DISP_resetIsTimer();
  
  // this function can only be ended when quit button is pressed or the time is over
  // check if the function ends normaly
  if (g_func_timer_info <= 0)
  {
    // end function for callback
    LCDML_DISP_funcend();  
  }   
}

void LCDML_DISP_loop_end(LCDML_FUNC_timer_info) 
{
  // this functions is ever called when a DISP function is quit
  // you can here reset some global vars or do nothing
}
*/
// *********************************************************************
uint8_t g_button_value = 0; // button value counter (global variable)
void LCDML_DISP_setup(LCDML_FUNC_p2)
// *********************************************************************
{ 
  // setup function
  // print lcd content
  lcd.setCursor(0, 0);
  lcd.print(F("press left or up"));  
  lcd.setCursor(0, 1);
  lcd.print(F("count: 0 of 3"));
  // Reset Button Value
  g_button_value = 0; 
}

void LCDML_DISP_loop(LCDML_FUNC_p2)
{
  // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
  // the quit button works in every DISP function without any checks; it starts the loop_end function 
  
  if (LCDML_BUTTON_checkAny()) // check if any button is pressed (enter, up, down, left, right)
  {
    if (LCDML_BUTTON_checkLeft() || LCDML_BUTTON_checkUp()) // check if button left is pressed
    {
      LCDML_BUTTON_resetLeft(); // reset the left button
      LCDML_BUTTON_resetUp(); // reset the left button
      g_button_value++;
      
      // update lcd content
      lcd.setCursor(7, 1); // set cursor   
      lcd.print(g_button_value); // print change content
    }    
  }
  
  // check if button count is three
  if (g_button_value >= 3) {
    // end function for callback
    LCDML_DISP_funcend();   
  } 
}

void LCDML_DISP_loop_end(LCDML_FUNC_p2) 
{
  // this functions is ever called when a DISP function is quit
  // you can here reset some global vars or do nothing
}

//===============================================================================================================================================================================
//                                                                     
// BACKEND SYSTEM                                                        
//                                                                       
// ===============================================================================================================================================================================
// every "backend function" needs three functions 
// - void LCDML_BACK_setup(..func_name..)    
// - void LCDML_BACK_loop(..func_name..)     
// - void LCDML_BACK_stable(..func_name..)
//
// - every BACKEND function can be stopped and started
// EXAMPLE CODE:
//    void LCDML_BACK_setup(LCDML_BACKEND_control)
//    {
      // setup
      // is called only if it is started or restartet (reset+start)
//    }
    
//    boolean LCDML_BACK_loop(LCDML_BACKEND_control)
//    {    
      // runs in loop
      
     
//      return false;  
//    }
    
//    void LCDML_BACK_stable(LCDML_BACKEND_control)
//    {
      // stable stop
      // is called when a backend function is stopped with stopStable  
//    }


// ===============================================================================================================================================================================
//
// Кнопки управления меню
//
// ===============================================================================================================================================================================
void LCDML_BACK_setup(LCDML_BACKEND_control)      
{
  // call setup   
  LCDML_CONTROL_setup();      
}
// backend loop
boolean LCDML_BACK_loop(LCDML_BACKEND_control)
{    
  // call loop
  LCDML_CONTROL_loop();

  // go to next backend function and do not block it
  return true;  
}
// backend stop stable
void LCDML_BACK_stable(LCDML_BACKEND_control)
{
}

void LCDML_CONTROL_setup()
{
  pinMode(_LCDML_CONTROL_digital_enter      , INPUT_PULLUP);      // устанавливаем режим работы вывода кнопки Вход, как "вход"
  pinMode(_LCDML_CONTROL_digital_up         , INPUT_PULLUP);      // устанавливаем режим работы вывода кнопки Вверх, как "вход"
  pinMode(_LCDML_CONTROL_digital_down       , INPUT_PULLUP);      // устанавливаем режим работы вывода кнопки Вниз, как "вход"
  # if(_LCDML_CONTROL_digital_enable_quit == 1)
    pinMode(_LCDML_CONTROL_digital_quit     , INPUT_PULLUP);      // устанавливаем режим работы вывода кнопки Выход, как "вход"
  # endif
  # if(_LCDML_CONTROL_digital_enable_lr == 1)
    pinMode(_LCDML_CONTROL_digital_left     , INPUT_PULLUP);      // устанавливаем режим работы вывода кнопки Лево, как "вход"
    pinMode(_LCDML_CONTROL_digital_right    , INPUT_PULLUP);      // устанавливаем режим работы вывода кнопки Право, как "вход"
  # endif
}

void LCDML_CONTROL_loop()
{
  #if(_LCDML_CONTROL_digital_low_active == 1)
  #  define _LCDML_CONTROL_digital_a !
  #else
  #  define _LCDML_CONTROL_digital_a
  #endif
  
  uint8_t but_stat = 0x00;
  
  bitWrite(but_stat, 0, _LCDML_CONTROL_digital_a(digitalRead(_LCDML_CONTROL_digital_enter)));
  bitWrite(but_stat, 1, _LCDML_CONTROL_digital_a(digitalRead(_LCDML_CONTROL_digital_up)));
  bitWrite(but_stat, 2, _LCDML_CONTROL_digital_a(digitalRead(_LCDML_CONTROL_digital_down)));
  #if(_LCDML_CONTROL_digital_enable_quit == 1)
  bitWrite(but_stat, 3, _LCDML_CONTROL_digital_a(digitalRead(_LCDML_CONTROL_digital_quit)));
  #endif
  #if(_LCDML_CONTROL_digital_enable_lr == 1)
  bitWrite(but_stat, 4, _LCDML_CONTROL_digital_a(digitalRead(_LCDML_CONTROL_digital_left)));
  bitWrite(but_stat, 5, _LCDML_CONTROL_digital_a(digitalRead(_LCDML_CONTROL_digital_right)));
  #endif  
  
  if (but_stat > 0) {
    if((millis() - g_LCDML_DISP_press_time) >= _LCDML_DISP_cfg_button_press_time) {
      g_LCDML_DISP_press_time = millis(); // reset press time
    
      if (bitRead(but_stat, 0)) { LCDML_BUTTON_enter(); }
      if (bitRead(but_stat, 1)) { LCDML_BUTTON_up();    }
      if (bitRead(but_stat, 2)) { LCDML_BUTTON_down();  }
      if (bitRead(but_stat, 3)) { LCDML_BUTTON_quit();  }
      if (bitRead(but_stat, 4)) { LCDML_BUTTON_left();  }
      if (bitRead(but_stat, 5)) { LCDML_BUTTON_right(); }        
    }    
  }
}

//======================================================== Установка времени ================================================================
uint8_t VAR_mode_SET  = 1;
void LCDML_DISP_setup(LCDML_FUNC_set_time)
{ 
  lcd.setCursor(0, 0);
  lcd.print(F("Change Time and Data"));
  LCDML_DISP_triggerMenu(100);
}

void LCDML_DISP_loop(LCDML_FUNC_set_time)
{
  lcd.setCursor (6,1);            // устанавливаем курсор в позицию: столбец 6, строка 0
  lcd.print(time.gettime("H:i:s"));    // выводим время
  lcd.setCursor (3,2);            // устанавливаем курсор в позицию: столбец 3, строка 1
  lcd.print (time.gettime("d.m.Y D")); // выводим дату и день недели
  time.blinktime(VAR_mode_SET);
  if (LCDML_BUTTON_checkAny()) 
    {
      if (LCDML_BUTTON_checkUp())
        {
          LCDML_BUTTON_resetUp();
          switch (VAR_mode_SET)                   // инкремент (увеличение) устанавливаемого значения
            {                                                                                               
              case 1: time.settime(0,                                   -1, -1, -1, -1, -1, -1); break;     // сек
              case 2: time.settime(-1, (time.minutes==59?0:time.minutes+1), -1, -1, -1, -1, -1); break;     // мин
              case 3: time.settime(-1, -1, (time.Hours==23?0:time.Hours+1),     -1, -1, -1, -1); break;     // час
              case 4: time.settime(-1, -1, -1, (time.day==31?1:time.day+1),         -1, -1, -1); break;     // дни
              case 5: time.settime(-1, -1, -1, -1, (time.month==12?1:time.month+1),     -1, -1); break;     // мес
              case 6: time.settime(-1, -1, -1, -1, -1, (time.year==99?0:time.year+1),       -1); break;     // год
              case 7: time.settime(-1, -1, -1, -1, -1, -1, (time.weekday==6?0:time.weekday+1) ); break;     // д.н.
            }
        }
      if (LCDML_BUTTON_checkDown())
        {
          LCDML_BUTTON_resetDown();
          switch (VAR_mode_SET)                   // декремент (уменьшение) устанавливаемого значения
            {                                     
              case 1: time.settime(0,                                   -1, -1, -1, -1, -1, -1); break;     // сек
              case 2: time.settime(-1, (time.minutes==0?59:time.minutes-1), -1, -1, -1, -1, -1); break;     // мин
              case 3: time.settime(-1, -1, (time.Hours==0?23:time.Hours-1),     -1, -1, -1, -1); break;     // час
              case 4: time.settime(-1, -1, -1, (time.day==1?31:time.day-1),         -1, -1, -1); break;     // дни
              case 5: time.settime(-1, -1, -1, -1, (time.month==1?12:time.month-1),     -1, -1); break;     // мес
              case 6: time.settime(-1, -1, -1, -1, -1, (time.year==0?99:time.year-1),       -1); break;     // год
              case 7: time.settime(-1, -1, -1, -1, -1, -1, (time.weekday==0?6:time.weekday-1) ); break;     // д.н.
            }
        }  
      if (LCDML_BUTTON_checkLeft())
          {
            LCDML_BUTTON_resetLeft();
            VAR_mode_SET--;                     // переходим к следующему устанавливаемому параметру
            if(VAR_mode_SET<1){VAR_mode_SET=7;}
          }
      if (LCDML_BUTTON_checkRight())
          {
            LCDML_BUTTON_resetRight();
            VAR_mode_SET++;                     // переходим к следующему устанавливаемому параметру
            if(VAR_mode_SET>7){VAR_mode_SET=1;} // возвращаемся к первому устанавливаемому параметру
          }
    }
}

void LCDML_DISP_loop_end(LCDML_FUNC_set_time) 
{
}
//===========================================================================================================================================

//======================================================= Выводит общую информацию ==========================================================
void LCDML_DISP_setup(LCDML_FUNC_information)
{
  LCDML_DISP_triggerMenu(500);
}

void LCDML_DISP_loop(LCDML_FUNC_information) 
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t))
    {
      lcd.clear();
      lcd.print("Failed DHT sensor!");
      return;
    }
  lcd.setCursor(0, 0);
  lcd.print(round (t));
  lcd.write(byte(5));
  lcd.print("C ");
  lcd.print(round (h));
  lcd.print("% ");
  lcd.setCursor(0, 3);
  lcd.print(time.gettime("H:i:s"));
  lcd.setCursor(10, 3);
  lcd.print(time.gettime("d.m.Y"));
}

void LCDML_DISP_loop_end(LCDML_FUNC_information)
{
}  
//===========================================================================================================================================

//=========================================================== Полив вручную ================================================================
int g_func_timer;  // time counter (global variable)
unsigned long g_timer_1;    // timer variable (globale variable)
int ClearFlag;
int hour_timer;
int min_timer;
int pressEnter;
void LCDML_DISP_setup(LCDML_FUNC_set_watering_time)
{
  pressEnter = 0;
  g_func_timer = 0;
  g_timer_1 = 0;
  ClearFlag = 0;
  hour_timer = 0;
  min_timer = 0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Watering Time ");
  lcd.setCursor(14, 0);
  lcd.print(g_func_timer);
  LCDML_DISP_triggerMenu(100);
}

void LCDML_DISP_loop(LCDML_FUNC_set_watering_time) 
{
  if (ClearFlag == 1)
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Watering Time ");
      lcd.setCursor(14, 0);
      if (g_func_timer < 3600)
        {
          lcd.print((g_func_timer/60));
        }
      else
        {
          hour_timer = g_func_timer/3600;
          lcd.print(hour_timer);
          lcd.print(":");
          lcd.setCursor(16,0);
          if (((g_func_timer-(hour_timer*3600))/60) < 10)
            {
              lcd.print ("0");
            }
          lcd.print((g_func_timer-(hour_timer*3600))/60);
        }
      ClearFlag = 0;
    }
  if (LCDML_BUTTON_checkAny()) 
    {
      ClearFlag = 1;
      if (LCDML_BUTTON_checkUp())
        {
          LCDML_BUTTON_resetUp();
          if (g_func_timer >= 10800)
            {
              g_func_timer = 0;
            }
          g_func_timer = g_func_timer + 300;
        }
      if (LCDML_BUTTON_checkDown())
        {
          LCDML_BUTTON_resetDown();
          g_func_timer = g_func_timer - 300;
          if (g_func_timer <= 0)
            {
              g_func_timer = 10800;
            }  
        }
      if (LCDML_BUTTON_checkEnter())
        {
          pressEnter = 1;
          LCDML_BUTTON_resetEnter();  
        }
    }
  if (pressEnter == 1)
    {
      if((millis()-g_timer_1) >= 1000)
        {
          g_timer_1 = millis();   
          g_func_timer--;                     // increment the value every second
          lcd.setCursor(0, 1);                // set cursor pos
          if (g_func_timer < 60)
            {
              lcd.print("0:00:");
              if (g_func_timer < 10)
                {
                  lcd.print ("0");
                }
              lcd.print(g_func_timer);
            }
          if (g_func_timer >=60 && g_func_timer <3600)
            {
              lcd.print ("0:");
              min_timer = g_func_timer/60;
              if (min_timer < 10)
                {
                  lcd.print ("0");
                }
              lcd.print (min_timer);
              lcd.print(":");
              if (g_func_timer-(min_timer*60) < 10)
                {
                  lcd.print ("0");
                }
              lcd.print(g_func_timer-(min_timer*60));
            }   
          if (g_func_timer >= 3600)
            {
              hour_timer = g_func_timer/3600;
              lcd.print(hour_timer);
              lcd.print(":");
              if ((g_func_timer-hour_timer*3600) >= 60)
                {
                  min_timer = ((g_func_timer-hour_timer*3600)/60);
                  if (min_timer < 10)
                    {
                      lcd.print ("0");
                    }
                  lcd.print(min_timer);
                  lcd.print(":");
                  if ((g_func_timer-(hour_timer*3600 + min_timer*60)) < 10)
                    {
                      lcd.print("0");
                    }
                  lcd.print (g_func_timer-(hour_timer*3600 + min_timer*60));  
                }
              else
                {
                  lcd.print("00:");
                  if (g_func_timer-hour_timer*3600 < 10)
                    {
                      lcd.print("0");
                    }
                  lcd.print (g_func_timer-hour_timer*3600);
                }    
            }
          if (g_func_timer <= 0)
            {
              pressEnter = 0;
              lcd.setCursor(0,3);
              lcd.print("Watering is finished");
            }
        }
    }
}         

void LCDML_DISP_loop_end(LCDML_FUNC_set_watering_time)
{
}  

//===========================================================================================================================================


//========================================================== Наполнение бака ================================================================
void LCDML_DISP_setup(LCDML_FUNC_set_filling_time)
{
  pressEnter = 0;
  g_func_timer = 0;
  g_timer_1 = 0;
  ClearFlag = 0;
  hour_timer = 0;
  min_timer = 0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Filling Time ");
  lcd.setCursor(14, 0);
  lcd.print(g_func_timer);
  LCDML_DISP_triggerMenu(100);
}

void LCDML_DISP_loop(LCDML_FUNC_set_filling_time) 
{
  if (ClearFlag == 1)
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Filling Time ");
      lcd.setCursor(14, 0);
      if (g_func_timer < 3600)
        {
          lcd.print((g_func_timer/60));
        }
      else
        {
          hour_timer = g_func_timer/3600;
          lcd.print(hour_timer);
          lcd.print(":");
          lcd.setCursor(16,0);
          if (((g_func_timer-(hour_timer*3600))/60) < 10)
            {
              lcd.print ("0");
            }
          lcd.print((g_func_timer-(hour_timer*3600))/60);
        }
      ClearFlag = 0;
    }
  if (LCDML_BUTTON_checkAny()) 
    {
      ClearFlag = 1;
      if (LCDML_BUTTON_checkUp())
        {
          LCDML_BUTTON_resetUp();
          if (g_func_timer >= 10800)
            {
              g_func_timer = 0;
            }
          g_func_timer = g_func_timer + 300;
        }
      if (LCDML_BUTTON_checkDown())
        {
          LCDML_BUTTON_resetDown();
          g_func_timer = g_func_timer - 300;
          if (g_func_timer <= 0)
            {
              g_func_timer = 10800;
            }  
        }
      if (LCDML_BUTTON_checkEnter())
        {
          pressEnter = 1;
          LCDML_BUTTON_resetEnter();  
        }
    }
  if (pressEnter == 1)
    {
      if((millis()-g_timer_1) >= 1000)
        {
          g_timer_1 = millis();   
          g_func_timer--;                     // increment the value every second
          lcd.setCursor(0, 1);                // set cursor pos
          if (g_func_timer < 60)
            {
              lcd.print("0:00:");
              if (g_func_timer < 10)
                {
                  lcd.print ("0");
                }
              lcd.print(g_func_timer);
            }
          if (g_func_timer >=60 && g_func_timer <3600)
            {
              lcd.print ("0:");
              min_timer = g_func_timer/60;
              if (min_timer < 10)
                {
                  lcd.print ("0");
                }
              lcd.print (min_timer);
              lcd.print(":");
              if (g_func_timer-(min_timer*60) < 10)
                {
                  lcd.print ("0");
                }
              lcd.print(g_func_timer-(min_timer*60));
            }   
          if (g_func_timer >= 3600)
            {
              hour_timer = g_func_timer/3600;
              lcd.print(hour_timer);
              lcd.print(":");
              if ((g_func_timer-hour_timer*3600) >= 60)
                {
                  min_timer = ((g_func_timer-hour_timer*3600)/60);
                  if (min_timer < 10)
                    {
                      lcd.print ("0");
                    }
                  lcd.print(min_timer);
                  lcd.print(":");
                  if ((g_func_timer-(hour_timer*3600 + min_timer*60)) < 10)
                    {
                      lcd.print("0");
                    }
                  lcd.print (g_func_timer-(hour_timer*3600 + min_timer*60));  
                }
              else
                {
                  lcd.print("00:");
                  if (g_func_timer-hour_timer*3600 < 10)
                    {
                      lcd.print("0");
                    }
                  lcd.print (g_func_timer-hour_timer*3600);
                }    
            }
          if (g_func_timer <= 0)
            {
              pressEnter = 0;
              lcd.setCursor(0,3);
              lcd.print("Filling is finished");
            }
        }
    }
}         

void LCDML_DISP_loop_end(LCDML_FUNC_set_filling_time)
{
}  

//===========================================================================================================================================

//========================================================== Проверяет ошибки ===============================================================
# if(_LCDML_DISP_rows > _LCDML_DISP_cfg_max_rows)
# error change value of _LCDML_DISP_cfg_max_rows in LCDMenuLib.h
# endif
# if(_LCDML_DISP_cols > _LCDML_DISP_cfg_max_string_length)
# error change value of _LCDML_DISP_cfg_max_string_length in LCDMenuLib.h
# endif