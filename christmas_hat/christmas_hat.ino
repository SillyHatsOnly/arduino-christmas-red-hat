#include <DFRobotDFPlayerMini.h>                                                              //  подключаем библиотеку для работы с MP3-плеером
#include <iarduino_NeoPixel.h>                                                                //  подключаем библиотеку для работы со светодиодами NEoPixel
#include <iarduino_HC_SR04_int.h>                                                             //  подключаем библиотеку для работы с датчиком расстояния HC-SR04
#include <Servo.h>                                                                            //  подключаем библиотеку для работы с сервоприводом

DFRobotDFPlayerMini myDFPlayer;                                                               //  объявляем объект myDFPlayer для работы с плеером

uint8_t       neo_pin            = 4;                                                         //  указываем вывод, к которому подключены светодиоды NeoPixel
uint16_t      modul_number       = 2;                                                         //  указываем количество модулей в цепи
iarduino_NeoPixel led(neo_pin, modul_number * 4 );                                            //  объявляем объект led для работы со светодиодами NeoPixel

iarduino_HC_SR04_int sensor(3, 2);                                                            //  объявляем объект sensor для работы с датчиком расстояния HC-SR04

Servo Servo_1;                                                                                //  создаём объект Servo_1 для работы с сервоприводом

bool          flg_first          = true;                                                      //  флаг первого запуска
bool          flg_moroz;                                                                      //  флаг блока "мороз"
bool          flg_terminator;                                                                 //  флаг блока "терминатор"
uint8_t       i                  = 0;                                                         //  переменная номера трека
uint8_t       r, g, b;                                                                        //  переменная для хранения цветов RGB для каждого светодиода
uint16_t      angle;                                                                          //  переменная угла поворота сервопривода
uint16_t      eye                = 255;                                                       //  переменная яркости глаза "терминатора"
uint32_t      timer1, timer2;                                                                 //  счётчики времени при срабатывании анимации
uint32_t      hcsr_waiting_time  = 30000;                                                     //  время ожидания повтора анимации
uint32_t      action_time;                                                                    //  время работы анимации

void setup() {
  Serial.begin(9600);                                                                         //  инициируем работу Монитора последовательного порта на скорости 9600бод
  myDFPlayer.begin(Serial);                                                                   //  указываем последовательный порт для общения с MP3-плеером
  Servo_1.attach(A0);                                                                         //  указываем вывод, к которому подключен сервопривод
  Servo_1.write(0);                                                                           //  указываем угол, на который должен повернуться сервопривод (возвращаем в начальное положение)
  led.begin();                                                                                //  инициализируем адресные светодиоды
  led.setColor(NeoPixelAll, 0, 0, 0);                                                         //  указываем всем светодиодам погаснуть
  led.write();                                                                                //  выключаем светодиоды
  sensor.distance();                                                                          //  запрашиваем значение расстояния
  delay(500);                                                                                 //  задержка 0.5сек
}

void loop() {
  //==========================================================================================//  БЛОК НАСТРОЙКИ РАБОТЫ ДАТЧИКА РАССТОЯНИЯ
  if (sensor.distance() >= 15 && sensor.distance() <= 40) {                                   //  Проверяем, если расстояние до препятствия от 15 до 40см, то
    if (millis() > timer1 + hcsr_waiting_time || flg_first) {                                 //  проверяем, прошло ли заданное время после последней сработки, или сброшен ли флаг при первом запуске, и если да, то
      i++;                                                                                    //  Увеличиваем номер трека на 1
      if(i>5) i = 1;                                                                          //  Если значение номера трека больше 5, то сбрасываем значение стрека в 0
      flg_first   = false;                                                                    //  Снимаем флаг первого запуска
      timer1      = millis();                                                                 //  обновляем значение таймера,
      myDFPlayer.volume(20);                                                                  //  выставляем уровень громкости плеера
      myDFPlayer.play(i);                                                                     //  указываем плееру включить трек
      if (i == 1) {action_time = 8400;  flg_moroz = true;  flg_terminator = false; }          //  Если номер трека 1, то устанавливаем время работы анимации, ставим флаг МОРОЗ, сбрасываем флаг ТЕРМИНАТОР
      if (i == 2) {action_time = 9700;  flg_moroz = true;  flg_terminator = false; }          //  Если номер трека 2, то устанавливаем время работы анимации, ставим флаг МОРОЗ, сбрасываем флаг ТЕРМИНАТОР
      if (i == 3) {action_time = 11000; flg_moroz = true;  flg_terminator = false; }          //  Если номер трека 3, то устанавливаем время работы анимации, ставим флаг МОРОЗ, сбрасываем флаг ТЕРМИНАТОР
      if (i == 4) {action_time = 10600; flg_moroz = true;  flg_terminator = false; }          //  Если номер трека 4, то устанавливаем время работы анимации, ставим флаг МОРОЗ, сбрасываем флаг ТЕРМИНАТОР
      if (i == 5) {action_time = 13600; flg_moroz = false; flg_terminator = true;  }          //  Если номер трека 5, то устанавливаем время работы анимации, ставим флаг ТЕРМИНАТОР, сбрасываем флаг МОРОЗ
    }
  }
  //==========================================================================================//
  if (flg_moroz && !flg_terminator) {                                                         //  Если был установлен флаг МОРОЗ, а флаг ТЕРМИНАТОР был сброшен, то
    if (millis() < timer1 + action_time) {                                                    //  проверяем, прошло ли заданное время на выполнение анимации, и если нет, то
      //======================================================================================//  БЛОК НАСТРОЙКИ ПОДСВЕТКИ
      if (millis() > timer2 + 500) {                                                          //  Проверяем, прошло ли время с момента последнего выполнения блока, и если нет, то
        timer2 = millis();                                                                    //  обновляем значение таймера,
        uint16_t f;                                                                           //  определяем переменную как коэффициент яркости
        if (f == 0  ) {f = 42; } else                                                         //  f=60°  (0 ... 42  ... 255   =   0° ... 60°  ... 360°)
        if (f == 42 ) {f = 85; } else                                                         //  f=120° (0 ... 85  ... 255   =   0° ... 120° ... 360°)
        if (f == 85 ) {f = 127;} else                                                         //  f=180° (0 ... 127 ... 255   =   0° ... 180° ... 360°)
        if (f == 127) {f = 170;} else                                                         //  f=240° (0 ... 170 ... 255   =   0° ... 240° ... 360°)
        if (f == 170) {f = 212;} else                                                         //  f=300° (0 ... 212 ... 255   =   0° ... 300° ... 360°)
        if (f == 212) {f = 0;  }                                                              //  f=360° (0 ... 255 ... 255   =   0° ... 360° ... 360°)
        if (f < 85)   {          b = 0; r = f * 3; g = 255 - r;} else                         //  перелив от зелёного к красному, через жёлтый
        if (f < 170)  {f -= 85;  g = 0; b = f * 3; r = 255 - b;} else                         //  перелив от красного к синему  , через фиолетовый
                      {f -= 170; r = 0; g = f * 3; b = 255 - g;}                              //  перелив от синего   к зелёному, через голубой
        for (uint8_t t = 0; t < 8; t++) {led.setColor(t, r, g, b);}                           //  устанавливаем выбранный цвет для очередного светодиода
      }
      //======================================================================================//  БЛОК НАСТРОЙКИ СЕРВОПРИВОДА
      if (i == 1) {                                                                           //  ТРЕК 1
        if (millis() > timer1 + 350  && millis() < timer1 + 450  ) {angle = 45;} else         //  здрА
        if (millis() > timer1 + 750  && millis() < timer1 + 850  ) {angle = 20;} else         //  вствУ
        if (millis() > timer1 + 950  && millis() < timer1 + 1050 ) {angle = 20;} else         //  йтЕ
        if (millis() > timer1 + 1150 && millis() < timer1 + 1250 ) {angle = 20;} else         //  рЕ
        if (millis() > timer1 + 1350 && millis() < timer1 + 1450 ) {angle = 20;} else         //  бЯ
        if (millis() > timer1 + 1550 && millis() < timer1 + 1650 ) {angle = 45;} else         //  тА
        if (millis() > timer1 + 2350 && millis() < timer1 + 2450 ) {angle = 45;} else         //  кАк
        if (millis() > timer1 + 2750 && millis() < timer1 + 2850 ) {angle = 20;} else         //  жЕ
        if (millis() > timer1 + 2950 && millis() < timer1 + 3050 ) {angle = 20;} else         //  вЫ
        if (millis() > timer1 + 3150 && millis() < timer1 + 3250 ) {angle = 20;} else         //  вЫ
        if (millis() > timer1 + 3350 && millis() < timer1 + 3450 ) {angle = 20;} else         //  рОс
        if (millis() > timer1 + 3750 && millis() < timer1 + 3850 ) {angle = 20;} else         //  лИ
        if (millis() > timer1 + 4250 && millis() < timer1 + 4350 ) {angle = 20;} else         //  бОль
        if (millis() > timer1 + 4550 && millis() < timer1 + 4650 ) {angle = 20;} else         //  шИ
        if (millis() > timer1 + 4750 && millis() < timer1 + 4850 ) {angle = 20;} else         //  мИ
        if (millis() > timer1 + 5150 && millis() < timer1 + 5250 ) {angle = 45;} else         //  стА
        if (millis() > timer1 + 5350 && millis() < timer1 + 5450 ) {angle = 20;} else         //  лИ
        if (millis() > timer1 + 5850 && millis() < timer1 + 5950 ) {angle = 45;} else         //  А
        if (millis() > timer1 + 6150 && millis() < timer1 + 6250 ) {angle = 20;} else         //  мЕ
        if (millis() > timer1 + 6350 && millis() < timer1 + 6450 ) {angle = 20;} else         //  нЯ
        if (millis() > timer1 + 6550 && millis() < timer1 + 6650 ) {angle = 20;} else         //  тО
        if (millis() > timer1 + 7050 && millis() < timer1 + 7150 ) {angle = 20;} else         //  всЕ
        if (millis() > timer1 + 7150 && millis() < timer1 + 7250 ) {angle = 20;} else         //  Уз
        if (millis() > timer1 + 7450 && millis() < timer1 + 7550 ) {angle = 45;} else         //  нА
        if (millis() > timer1 + 7750 && millis() < timer1 + 7850 ) {angle = 20;} else         //  лИ?
                                                                   {angle =  0;}              //  согласные
      }
      //======================================================================================//
      if (i == 2) {                                                                           //  ТРЕК 2
        if (millis() > timer1 + 150  && millis() < timer1 + 250  ) {angle = 20;}else          //  Я
        if (millis() > timer1 + 650  && millis() < timer1 + 750  ) {angle = 20;} else         //  рЕ
        if (millis() > timer1 + 750  && millis() < timer1 + 850  ) {angle = 20;} else         //  бЯ
        if (millis() > timer1 + 950  && millis() < timer1 + 1050 ) {angle = 45;} else         //  тА
        if (millis() > timer1 + 1450 && millis() < timer1 + 1600 ) {angle = 45;} else         //  стА
        if (millis() > timer1 + 1650 && millis() < timer1 + 1750 ) {angle = 20;} else         //  рЫй
        if (millis() > timer1 + 1950 && millis() < timer1 + 2100 ) {angle = 20;} else         //  дЕд
        if (millis() > timer1 + 2650 && millis() < timer1 + 2750 ) {angle = 20;} else         //  мнЕ
        if (millis() > timer1 + 2850 && millis() < timer1 + 2950 ) {angle = 20;} else         //  У
        if (millis() > timer1 + 3050 && millis() < timer1 + 3150 ) {angle = 20;} else         //  же
        if (millis() > timer1 + 3500 && millis() < timer1 + 3600 ) {angle = 20;} else         //  пЯть
        if (millis() > timer1 + 4000 && millis() < timer1 + 4100 ) {angle = 20;} else         //  тЫ
        if (millis() > timer1 + 4150 && millis() < timer1 + 4250 ) {angle = 20;} else         //  сЯч
        if (millis() > timer1 + 4550 && millis() < timer1 + 4650 ) {angle = 20;} else         //  лЕт
        if (millis() > timer1 + 5350 && millis() < timer1 + 5450 ) {angle = 20;} else         //  в Ян
        if (millis() > timer1 + 5550 && millis() < timer1 + 5650 ) {angle = 45;} else         //  вА
        if (millis() > timer1 + 5750 && millis() < timer1 + 5850 ) {angle = 20;} else         //  рЕ
        if (millis() > timer1 + 6050 && millis() < timer1 + 6150 ) {angle = 20;} else         //  И
        if (millis() > timer1 + 6150 && millis() < timer1 + 6250 ) {angle = 20;} else         //  фЕв
        if (millis() > timer1 + 6350 && millis() < timer1 + 6450 ) {angle = 45;} else         //  рА
        if (millis() > timer1 + 6650 && millis() < timer1 + 6750 ) {angle = 20;} else         //  лЕ
        if (millis() > timer1 + 7150 && millis() < timer1 + 7250 ) {angle = 20;} else         //  Я
        if (millis() > timer1 + 7550 && millis() < timer1 + 7650 ) {angle = 20;} else         //  гУ
        if (millis() > timer1 + 7900 && millis() < timer1 + 8000 ) {angle = 20;} else         //  лЯ
        if (millis() > timer1 + 8000 && millis() < timer1 + 8100 ) {angle = 20;} else         //  Ю
        if (millis() > timer1 + 8500 && millis() < timer1 + 8600 ) {angle = 20;} else         //  пО
        if (millis() > timer1 + 8750 && millis() < timer1 + 8850 ) {angle = 20;} else         //  зЕм
        if (millis() > timer1 + 9000 && millis() < timer1 + 9100 ) {angle = 20;} else         //  лЕ
                                                                   {angle =  0;}              //  согласные
      }
      //======================================================================================//
      if (i == 3) {                                                                           //  ТРЕК 3
        if (millis() > timer1 + 250   && millis() < timer1 + 350   ) {angle = 20;} else       //  тОль
        if (millis() > timer1 + 550   && millis() < timer1 + 650   ) {angle = 20;} else       //  кО
        if (millis() > timer1 + 1000  && millis() < timer1 + 1100  ) {angle = 45;} else       //  встА
        if (millis() > timer1 + 1300  && millis() < timer1 + 1400  ) {angle = 20;} else       //  нУ
        if (millis() > timer1 + 1550  && millis() < timer1 + 1650  ) {angle = 20;} else       //  Я
        if (millis() > timer1 + 1900  && millis() < timer1 + 2000  ) {angle = 20;} else       //  с пО
        if (millis() > timer1 + 2150  && millis() < timer1 + 2250  ) {angle = 20;} else       //  стЕ
        if (millis() > timer1 + 2450  && millis() < timer1 + 2550  ) {angle = 20;} else       //  лИ
        if (millis() > timer1 + 3100  && millis() < timer1 + 3200  ) {angle = 45;} else       //  нА
        if (millis() > timer1 + 3500  && millis() < timer1 + 3600  ) {angle = 20;} else       //  чИ
        if (millis() > timer1 + 3700  && millis() < timer1 + 3800  ) {angle = 45;} else       //  нА
        if (millis() > timer1 + 3900  && millis() < timer1 + 4000  ) {angle = 20;} else       //  Ют
        if (millis() > timer1 + 4150  && millis() < timer1 + 4250  ) {angle = 20;} else       //  сЯ
        if (millis() > timer1 + 4400  && millis() < timer1 + 4500  ) {angle = 20;} else       //  мЕ
        if (millis() > timer1 + 4600  && millis() < timer1 + 4700  ) {angle = 20;} else       //  тЕ
        if (millis() > timer1 + 5000  && millis() < timer1 + 5100  ) {angle = 20;} else       //  лИ
        if (millis() > timer1 + 5600  && millis() < timer1 + 5700  ) {angle = 45;} else       //  кАк
        if (millis() > timer1 + 6050  && millis() < timer1 + 6150  ) {angle = 20;} else       //  встрЯ
        if (millis() > timer1 + 6450  && millis() < timer1 + 6550  ) {angle = 20;} else       //  хнУ
        if (millis() > timer1 + 6700  && millis() < timer1 + 6800  ) {angle = 20;} else       //  Я
        if (millis() > timer1 + 6950  && millis() < timer1 + 7050  ) {angle = 20;} else       //  рУ
        if (millis() > timer1 + 7100  && millis() < timer1 + 7200  ) {angle = 45;} else       //  кА
        if (millis() > timer1 + 7300  && millis() < timer1 + 7400  ) {angle = 20;} else       //  вОм
        if (millis() > timer1 + 8200  && millis() < timer1 + 8300  ) {angle = 20;} else       //  всЁ
        if (millis() > timer1 + 8600  && millis() < timer1 + 8700  ) {angle = 20;} else       //  пО
        if (millis() > timer1 + 9000  && millis() < timer1 + 9100  ) {angle = 20;} else       //  крО
        if (millis() > timer1 + 9250  && millis() < timer1 + 9350  ) {angle = 20;} else       //  Ет
        if (millis() > timer1 + 9500  && millis() < timer1 + 9600  ) {angle = 20;} else       //  сЯ
        if (millis() > timer1 + 9850  && millis() < timer1 + 9950  ) {angle = 20;} else       //  снЕж
        if (millis() > timer1 + 10150 && millis() < timer1 + 10250 ) {angle = 20;} else       //  кОм
                                                                     {angle =  0;}            //  согласные
      }
      //======================================================================================//
      if (i == 4) {                                                                           //  ТРЕК 4
        if (millis() > timer1 + 200   && millis() < timer1 + 300   ) {angle = 20;} else       //  нО
        if (millis() > timer1 + 550   && millis() < timer1 + 600   ) {angle = 20;} else       //  сЕй
        if (millis() > timer1 + 900   && millis() < timer1 + 1000  ) {angle = 45;} else       //  чАс
        if (millis() > timer1 + 1350  && millis() < timer1 + 1450  ) {angle = 20;} else       //  Я
        if (millis() > timer1 + 1750  && millis() < timer1 + 1850  ) {angle = 20;} else       //  О
        if (millis() > timer1 + 2050  && millis() < timer1 + 2150  ) {angle = 20;} else       //  чЕнь
        if (millis() > timer1 + 2450  && millis() < timer1 + 2550  ) {angle = 20;} else       //  дОб
        if (millis() > timer1 + 2900  && millis() < timer1 + 3000  ) {angle = 20;} else       //  рЫй
        if (millis() > timer1 + 3650  && millis() < timer1 + 3750  ) {angle = 20;} else       //  И
        if (millis() > timer1 + 3950  && millis() < timer1 + 4150  ) {angle = 20;} else       //  с рЕ
        if (millis() > timer1 + 4150  && millis() < timer1 + 4250  ) {angle = 20;} else       //  бЯ
        if (millis() > timer1 + 4300  && millis() < timer1 + 4400  ) {angle = 45;} else       //  тА
        if (millis() > timer1 + 4450  && millis() < timer1 + 4550  ) {angle = 20;} else       //  мИ
        if (millis() > timer1 + 4650  && millis() < timer1 + 4750  ) {angle = 20;} else       //  дрУ
        if (millis() > timer1 + 4900  && millis() < timer1 + 5000  ) {angle = 20;} else       //  жУ
        if (millis() > timer1 + 5850  && millis() < timer1 + 5950  ) {angle = 20;} else       //  нИ
        if (millis() > timer1 + 6250  && millis() < timer1 + 6350  ) {angle = 20;} else       //  кО
        if (millis() > timer1 + 6650  && millis() < timer1 + 6750  ) {angle = 20;} else       //  гО
        if (millis() > timer1 + 6900  && millis() < timer1 + 7000  ) {angle = 20;} else       //  нЕ
        if (millis() > timer1 + 7050  && millis() < timer1 + 7150  ) {angle = 45;} else       //  зА
        if (millis() > timer1 + 7200  && millis() < timer1 + 7300  ) {angle = 20;} else       //  мО
        if (millis() > timer1 + 7350  && millis() < timer1 + 7450  ) {angle = 45;} else       //  рО
        if (millis() > timer1 + 7650  && millis() < timer1 + 7750  ) {angle = 20;} else       //  жУ
        if (millis() > timer1 + 8200  && millis() < timer1 + 8300  ) {angle = 20;} else       //  нИ
        if (millis() > timer1 + 8550  && millis() < timer1 + 8650  ) {angle = 20;} else       //  кО
        if (millis() > timer1 + 8850  && millis() < timer1 + 8950  ) {angle = 20;} else       //  гО
        if (millis() > timer1 + 9300  && millis() < timer1 + 9400  ) {angle = 20;} else       //  не
        if (millis() > timer1 + 9600  && millis() < timer1 + 9700  ) {angle = 45;} else       //  зА
        if (millis() > timer1 + 9850  && millis() < timer1 + 9950  ) {angle = 20;} else       //  стУ
        if (millis() > timer1 + 10150 && millis() < timer1 + 10250 ) {angle = 20;} else       //  жУ
                                                                     {angle =  0;}            //  согласные
      }
      //======================================================================================//
      Servo_1.write(angle);                                                                   //  указываем сервоприводу отклониться на угол angle
      led.write();                                                                            //  записываем указанные цвета в модули
      delay(10);                                                                              //  задержка 0.1сек
    }
    else{                                                                                     //  Если время выполнения анимации закончилось, то
      flg_moroz = false;                                                                      //  сбрасываем флаг МОРОЗ
    }
  } else
  //==========================================================================================//  ТРЕК 5
  if (!flg_moroz && flg_terminator) {                                                         //  Если флаг МОРОЗ сброшен, а флаг ТЕРМИНАТОР установлен, то
    if (millis() < timer1 + action_time) {                                                    //  проверяем, прошло ли заданное время на выполнение анимации, и если нет, то
      //======================================================================================//  БЛОК НАСТРОЙКИ ПОДСВЕТКИ
      if (millis() > timer1 + 2950 && millis() < timer1 + 8000 ) {                            //  проверяем, прошло ли заданное время на выполнение анимации, и если нет, то
        for (uint8_t t = 0; t < 4; t++) {led.setColor(t, eye, 0, 0);}                         //  включаем 4 светодиода одного модуля красным цветом
      }
      if (millis() >= timer1 + 8000 && millis() < timer1 + 13500 ) {                          //  проверяем, прошло ли заданное время на выполнение анимации, и если нет, то
        if (millis() > timer2 + 15) {                                                         //  каждые 15мс
          timer2 = millis();                                                                  //  обновляем счётчик и
          if (eye > 0) eye--;                                                                 //  уменьшаем значение яркости "глаза" на 1
          else         eye = 0;                                                               //  Если же значение долшло до 0, то так и оставляем это значение
        }
        for (uint8_t t = 0; t < 4; t++) {led.setColor(t, eye, 0, 0);}                         //  Устанавливаем яркость
      }
      //======================================================================================//  БЛОК НАСТРОЙКИ СЕРВОПРИВОДА
      if (millis() > timer1 + 4750 && millis() < timer1 + 4850 ) {angle = 45;} else           //  hA
      if (millis() > timer1 + 5050 && millis() < timer1 + 5150 ) {angle = 45;} else           //  stA
      if (millis() > timer1 + 5150 && millis() < timer1 + 5250 ) {angle = 45;} else           //  lA
      if (millis() > timer1 + 5300 && millis() < timer1 + 5400 ) {angle = 20;} else           //  vI
      if (millis() > timer1 + 5650 && millis() < timer1 + 5750 ) {angle = 45;} else           //  stA
      if (millis() > timer1 + 6150 && millis() < timer1 + 6250 ) {angle = 45;} else           //  bA
      if (millis() > timer1 + 6400 && millis() < timer1 + 6500 ) {angle = 20;} else           //  bY
                                                                 {angle =  0;}                //  согласные
      //======================================================================================//
      Servo_1.write(angle);                                                                   //  указываем сервоприводу отклониться на угол angle
      led.write();                                                                            //  записываем указанные ранее цвета в модули
      delay(10);                                                                              //  задержка 0.1сек
    } else {                                                                                  //  Если время выполнения анимации закончилось, то
      flg_terminator = false;                                                                 //  сбрасываем флаг ТЕРМИНАТОР
      }
  }
  else {                                                                                      //  Если время выполнения анимации закончилось, то
  //==========================================================================================//  БЛОК РАБОТЫ УСТРОЙСТВА БЕЗ НАЛИЧИЯ ПРЕПЯТСТВИЯ В ЗОНЕ ДАТЧИКА
    eye = 255;                                                                                //  указываем значение яркости глаза максимальным
    myDFPlayer.pause();                                                                       //  останавливаем плеер
    led.setColor(NeoPixelAll, 0, 0, 0);                                                       //  устанавливаем цвет всех светодиодов в чёрный(гасим)
    led.write();                                                                              //  записываем указанный цвет в модули
  }
}
