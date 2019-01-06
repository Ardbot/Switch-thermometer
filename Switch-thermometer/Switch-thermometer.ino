#include <OneWire.h>

OneWire ds(2);
int celsius;
int brightness = 0; // Яркость
int fres = A4; // Фоторезистор

void setup() {
  Serial.begin(9600);
  pinMode(3, OUTPUT); //  Стрелка
  pinMode(5, OUTPUT); //  Синий
  pinMode(6, OUTPUT); //  Зеленый
  pinMode(9, OUTPUT); //  Красный
}

void loop() {
  Temp(); //  Узнаем температуру
  fotores();  //  Узнаем уровень освещения
  Home(); //  Режим разбиения на температурные диапазоны
 // full(); //  От 0 до 50 градусов на лдной шкале

}
void Temp()
{
  byte i;
  byte data[12];
  byte addr[8];


  // поиск адреса датчика
  if ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1); // команда на измерение температуры

  delay(1000);

  ds.reset();
  ds.select(addr);
  ds.write(0xBE); // команда на начало чтения измеренной температуры

  // считываем показания температуры из внутренней памяти датчика
  for ( i = 0; i < 9; i++) {
    data[i] = ds.read();
  }

  int16_t raw = (data[1] << 8) | data[0];
  // датчик может быть настроен на разную точность, выясняем её
  byte cfg = (data[4] & 0x60);
  if (cfg == 0x00) raw = raw & ~7; // точность 9-разрядов, 93,75 мс
  else if (cfg == 0x20) raw = raw & ~3; // точность 10-разрядов, 187,5 мс
  else if (cfg == 0x40) raw = raw & ~1; // точность 11-разрядов, 375 мс

  // преобразование показаний датчика в градусы Цельсия
  celsius = (float)raw / 16.0;
}
void Home()
{
  if (celsius > 0 && celsius < 18) //  Холодно
  {
    analogWrite(3, celsius * 14); // 14 получаем путем деления шкалы (255) на интервал температуры
    analogWrite(5, 256 - (celsius * 14));
  }
  else
    analogWrite(5, 0);

  if (celsius >= 18 && celsius <= 25) //  Норма
  {
    analogWrite(3, (celsius - 18) * 36);
    analogWrite(6, brightness); // Зеленый //(celsius - 17) * 30
  }
  else
    analogWrite(6, 0);

  if (celsius > 25) //  Жара
  {
    analogWrite(3, (celsius - 25) * 10);
    analogWrite(9, (celsius - 25) * 10); //  Красный
  }
  else
    analogWrite(9, 0);
}
void full() // от 0 до 50 градусов Цельсия
{
  analogWrite(3, celsius * 5);
  //analogWrite(5, brightness); //  синий
  analogWrite(6, brightness); //  зеленный
  // analogWrite(9, brightness); //  Красный
  Serial.print("t=");
  Serial.println(celsius);
  Serial.println(brightness);

}
void fotores()
{
  brightness =  analogRead(4);
  if (brightness < 10)
  {
    brightness = 10;
  }
  else
    brightness = brightness / 4;
}
