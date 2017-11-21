/**************************************************************
calculator.ino
BlackBug Engineering
21.11.2017
https://github.com/dbprof/calculator
***************************************************************/

//Библиотека ЖК-дисплея
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//Библиотека клавиатуры
#include <Keypad.h>

//Объявляем ЖК-дисплей
LiquidCrystal_I2C lcd(0x27,16,2);

//Переменная строк клавиатуры
const byte ROWS = 4;

//Переменная столбцов клавиатуры
const byte COLS = 4;

//Объявляем двумерный массив символов клавиатуры
//не хватает символа "очистки"
//процедура выполняется при нажатии двух точек подряд - ".."
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','+'},
  {'4','5','6','-'},
  {'7','8','9','*'},
  {'.','0','=','/'}
};

//Пины подключения строк клавиатуры
//на 1 пин подключит Arduino Uno не удалось, пришлось использовать 8
byte rowPins[ROWS] = {0, 8, 2, 3};

//Пины подключения столбцов клавиатуры 
byte colPins[COLS] = {4, 5, 6, 7};

//Инициализация клавиатуры
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

//Переменные для вычисления и знак вычисления
String sVal1;   //1 переменная
bool isVal1;    //признак окончания ввода 1 переменной
String sVal2;   //2 переменная
bool isVal2;    //признак окончания ввода 2 переменной
char cSign;     //знак действия
char prevKey;   //предидущий символ (используется для поиска двойного нажатия - "..")
bool isSign;    //признак наличия знака действия
float fResult;  //результат
bool isResult;  //признак наличия результата

//Функция очистки ЖК-дисплея и обнуления переменных
void clearAll(){
  lcd.clear();  
  fResult = 0;  
  cSign = ' ';  
  prevKey = ' ';
  sVal1 = "";   
  isVal1 = 1;   
  sVal2 = "";   
  isVal2 = 0;   
  isSign = 0;   
  isResult = 0; 
}

//Функция вывода на ЖК-дисплей
void printAll(){
  lcd.clear();
  //В верхней строке ЖК-дисплея пишем числа
  lcd.setCursor(0, 0);
  //Если есть результат, то пишем его
  //если результата нет, но есть знак действия, то пишем 2 переменную
  //если нет ни результата, ни знака действия, то пишем 1 переменную
  if (isResult){
    lcd.print(String(fResult));
  }
  else{
    if (isSign){
      if(sVal2!=0){lcd.print(String(sVal2));}
    }
    else {
      if(sVal1!=0){lcd.print(String(sVal1));}
    }
  }
  //В нижней строке ЖК-дисплея выводим знак действия
  lcd.setCursor(0, 1);
  lcd.print(cSign);
}

void setup(){
  //Порт вывода для тестирования
  Serial.begin(9600);

  //Инициализация ЖК-дисплея
  lcd.init();
  //Включаем подсветку                     
  lcd.backlight();
  //Обнуляем переменные
  clearAll();
}
  
void loop(){
  //Читаем введенный с клавиатуры символ
  char customKey = customKeypad.getKey();

  //Если символ - знак действия, то запоминаем его в переменную 
  //и проставляем признаки наличия знака действия и 1 переменной
  //если ошибочно ввод начат со знака, то 1 переменная останется = 0
  if (customKey){
    if (customKey == '+' || customKey == '-' || customKey == '*' || customKey == '/'){
      cSign = customKey;
      isSign = 1;
      isVal2 = 1;
    }
    //Очищаем ЖК-дисплей и обнуляем переменные, если введены две точки - ".."
    else if (customKey == '.' && prevKey == '.'){
      clearAll();
    }
    //Производим вычисления, если нажат символ "=" и уже есть признаки
    //наличия 1 и 2 переменных и знака действия
    else if (customKey == '='){
      if (isVal1 && isVal2 && isSign){
        if (cSign == '+'){fResult = sVal1.toFloat() + sVal2.toFloat();}
        else if (cSign == '-'){fResult = sVal1.toFloat() - sVal2.toFloat();}
        else if (cSign == '*'){fResult = sVal1.toFloat() * sVal2.toFloat();}
        else if (cSign == '/'){fResult = sVal1.toFloat() / sVal2.toFloat();}
        else {fResult = 0;}
        cSign = '=';
        isResult = 1;
      }
    }
    //Продолжаем читать в 1 или 2 переменную в зависимости от признаков наличия
    else {
      if (!isResult){
        if (isVal2){
          sVal2 = sVal2 + String(customKey);
        }
        else {
          if (isVal1){
            sVal1 = sVal1 + String(customKey);
          }
        }
      }
    }
    //Выводим всю накопленную информацию на ЖК-дисплей
    printAll();
    //Запоминаем в переменную предидущую нажатую клавишу,
    //чтобы отловить момент ввода двух точек и очистить экран
    prevKey = customKey;
    //Пишем в порт вывода для тестирования
    Serial.println("(Val1=" + String(sVal1) + ")" + cSign + "(Val2=" + String(sVal2) + ")" + "=" + String(fResult));
  }
}

