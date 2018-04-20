#include <M5Stack.h>
#include <iarduino_RF433_Receiver.h>
#include "m5stack_tv.h"
#include "weatherIcons/main.h"

iarduino_RF433_Receiver radioRX(5);

extern unsigned char radio_logo[];
uint8_t k;
char j[24];
String temp, pres, hum, desc, icon;

String parseString(int idSeparator, char separator, String str) { // like a split JS
  String output = "";
  int separatorCout = 0;
  for (int i = 0; i < str.length(); i++)
  {
    if ((char)str[i] == separator)
    {
      separatorCout++;
    }
    else
    {
      if (separatorCout == idSeparator)
      {
        output += (char)str[i];
      }
      else if (separatorCout > idSeparator)
      {
        break;
      }
    }
  }
  return output;
}

void drawWeatherIcon(int x, int y, String str) {
  if (str == "01d") m5stack_tv::drawBitmap(x, y, _01d::xres, _01d::yres, _01d::pixels);
  else if (str == "01n") m5stack_tv::drawBitmap(x, y, _01n::xres, _01n::yres, _01n::pixels);
  else if (str == "02d") m5stack_tv::drawBitmap(x, y, _02d::xres, _02d::yres, _02d::pixels);
  else if (str == "02n") m5stack_tv::drawBitmap(x, y, _02n::xres, _02n::yres, _02n::pixels);
  else if (str == "03d") m5stack_tv::drawBitmap(x, y, _03d::xres, _03d::yres, _03d::pixels);
  else if (str == "03n") m5stack_tv::drawBitmap(x, y, _03n::xres, _03n::yres, _03n::pixels);
  else if (str == "04d") m5stack_tv::drawBitmap(x, y, _04d::xres, _04d::yres, _04d::pixels);
  else if (str == "04n") m5stack_tv::drawBitmap(x, y, _04n::xres, _04n::yres, _04n::pixels);
  else if (str == "09d") m5stack_tv::drawBitmap(x, y, _09d::xres, _09d::yres, _09d::pixels);
  else if (str == "09n") m5stack_tv::drawBitmap(x, y, _09n::xres, _09n::yres, _09n::pixels);
  else if (str == "10d") m5stack_tv::drawBitmap(x, y, _10d::xres, _10d::yres, _10d::pixels);
  else if (str == "10n") m5stack_tv::drawBitmap(x, y, _10n::xres, _10n::yres, _10n::pixels);
  else if (str == "11d") m5stack_tv::drawBitmap(x, y, _11d::xres, _11d::yres, _11d::pixels);
  else if (str == "11n") m5stack_tv::drawBitmap(x, y, _11n::xres, _11n::yres, _11n::pixels);
  else if (str == "13d") m5stack_tv::drawBitmap(x, y, _13d::xres, _13d::yres, _13d::pixels);
  else if (str == "13n") m5stack_tv::drawBitmap(x, y, _13n::xres, _13n::yres, _13n::pixels);
  else if (str == "50d") m5stack_tv::drawBitmap(x, y, _50d::xres, _50d::yres, _50d::pixels);
  else if (str == "50n") m5stack_tv::drawBitmap(x, y, _50n::xres, _50n::yres, _50n::pixels);
}

void message(String text) {
  M5.Lcd.fillRect(0, 180, 320, 30, 0x7bef);
  M5.Lcd.setCursor(10, 187);
  M5.Lcd.setTextColor(0xffff);
  M5.Lcd.print(text);
  M5.Lcd.setTextColor(0x7bef);
}

void setup() {
  m5.begin();
  M5.Lcd.fillRoundRect(0, 0, 320, 240, 7, 0xffff);
  M5.Lcd.setTextColor(0x7bef);
  M5.Lcd.setTextSize(2);
  M5.Lcd.drawBitmap(35, 80, 59, 59, (uint16_t *)radio_logo);  
  M5.Lcd.setCursor(120, 70);
  M5.Lcd.print("WEATHER RADIO");
  M5.Lcd.setCursor(120, 90);
  M5.Lcd.print("RECIVER");
  M5.Lcd.setCursor(120, 110);
  M5.Lcd.print("433 MHz");
  M5.Lcd.setCursor(120, 130);
  M5.Lcd.print("ON M5STACK");
  message("radio is starting");
  radioRX.begin(1000);              
  radioRX.openReadingPipe(0);
  radioRX.startListening();
  message("TV is starting");
  m5stack_tv::begin();
  message("waiting for radio data");
}

void loop() { 
  if (radioRX.available(&k))
  {
    message("data accepted");
    radioRX.read(&j, sizeof(j));
    delay(1);
    message(j);
    int type = (parseString(0, (char)0x1d, j)).toInt();
    String data = parseString(1, (char)0x1d, j);
    if (type == 1) temp = data;
    else if (type == 2) pres = data;
    else if (type == 3) hum = data;
    else if (type == 4) desc = data; 
    else if (type == 5) icon = data;
    
    if (type > 0)
    {
      m5stack_tv::setTextColor(0);
      m5stack_tv::clearScreen(54);
      message("drawing on TV");  
      m5stack_tv::setCursor(140, 60);
      m5stack_tv::print("Temperature, C: " + temp);
      m5stack_tv::setCursor(140, 80);
      m5stack_tv::print("Humidity, %: " + hum);
      m5stack_tv::setCursor(140, 100);
      m5stack_tv::print("Pressure, mm Hg: " + pres);
      m5stack_tv::setCursor(140, 120);
      m5stack_tv::print(desc);
      drawWeatherIcon(30, 45, icon);
    }
  }
}
