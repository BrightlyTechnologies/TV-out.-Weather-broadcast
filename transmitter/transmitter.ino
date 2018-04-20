#include <M5Stack.h>
#include <iarduino_RF433_Transmitter.h>                  
#include <HTTPClient.h>

WiFiMulti wifiMulti;
iarduino_RF433_Transmitter radioTX(5); 
extern unsigned char timer_logo[];
extern unsigned char insertsd_logo[];
extern unsigned char error_logo[];
extern unsigned char radio_logo[];
String url = "";
String weather = "";
String temp = "";
String pres = "";
String hum = "";
String desc = "";
String weatherIcon = "";
String total = "";
String updtime = "";

void sendString(String str) {
  int len = str.length() + 1;
  char bfr[len];  
  str.toCharArray(bfr, len);
  int n = 1;
  if (len <= 8) n = 4; 
  for (int i = 0; i < n; i++)
  {
    radioTX.write(&bfr, sizeof(bfr)); 
  }
}

String GET(String url) {
  while (true)
  {
    if ((wifiMulti.run() == WL_CONNECTED))
    {
      HTTPClient http;
      http.begin(url);
      int httpCode = http.GET();
      if (httpCode > 0)
      {
          if (httpCode == HTTP_CODE_OK)
          {
            return http.getString();
          }
      }
      else
      {
        return (httpCode + "");
      }
      http.end();
    }
  }  
  return ""; 
}

char* strToChar(String str) {
  int len = str.length() + 1;
  char* buf = new char[len];
  strcpy(buf, str.c_str());
  return buf;
}

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

int cntChrs(String str, char chr) {
  int cnt = 0;
  for (int i = 0; i < str.length(); i++)
  {
    if (str[i] == chr) cnt++;  
  }
  return cnt;
}

String TFReadFile(String path) {
  File file = SD.open(strToChar(path));
  String buf = "";
  if (file)
  {
    while(file.available())
    {
      buf += (char)file.read();
    }
    file.close();
  }
  return buf;
}

bool configWifiMulti() {
  /* Get WiFi SSID & password from wifi.ini from TF-card */
  String file = TFReadFile("/system/wifi.ini");
  if (file != "")
  {
    for (int i = 0; i < cntChrs(file, '\n'); i++)
    {
      String wifi = parseString(i, '\n', file);
      wifi = wifi.substring(0, (wifi.length()));
      if (wifi[wifi.length() - 1] == '\r') wifi = wifi.substring(0, (wifi.length() - 1));
      String ssid = parseString(0, ' ', wifi);
      String pswd = parseString(1, ' ', wifi);
      char* ssid_ = strToChar(ssid);
      char* pswd_ = strToChar(pswd);
      if (wifiMulti.addAP(ssid_, pswd_))
      {
        return true;
      }
    }
  }
  return false;
}


String configOpenWeather() {
  String file = TFReadFile("/system/openweather.ini");
  if (file != "")
  {  
    String city = "&q=" + parseString(1, '\"', parseString(1, ':', parseString(5, ',', GET("http://nl.sxgeo.city/?")))); 
    String api_key = "&APPID=" + parseString(0, ' ', file);
    String app_id = "&id=" + parseString(1, ' ', file);
    String lang = "&lang=" + parseString(2, ' ', file);
    String units = "&units=" + parseString(3, ' ', file);
    String host = "http://api.openweathermap.org/data/2.5/weather?";
    String url_ = host + city + api_key + app_id + lang + units;
    return url_;
  }
  return "";
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
  M5.Lcd.drawBitmap(30, 75, 59, 59, (uint16_t *)timer_logo);
  M5.Lcd.setCursor(110, 90);
  M5.Lcd.print("STARTING...");
  M5.Lcd.setCursor(110, 110);
  M5.Lcd.print("WAIT A MOMENT");
  if (!SD.begin())
  {
    M5.Lcd.fillRoundRect(0, 0, 320, 240, 7, 0xffff);
    M5.Lcd.drawBitmap(50, 70, 62, 115, (uint16_t *)insertsd_logo);
    M5.Lcd.setCursor(130, 70);
    M5.Lcd.print("INSERT");
    M5.Lcd.setCursor(130, 90);
    M5.Lcd.print("THE TF-CARD");
    M5.Lcd.setCursor(130, 110);
    M5.Lcd.print("AND TAP");
    M5.Lcd.setCursor(130, 130);
    M5.Lcd.setTextColor(0xe8e4);
    M5.Lcd.print("POWER");
    M5.Lcd.setTextColor(0x7bef);
    M5.Lcd.print(" BUTTON"); 
    while(true);
  }
  else
  {
    if (!configWifiMulti())
    {
      M5.Lcd.fillRoundRect(0, 0, 320, 240, 7, 0xffff);
      M5.Lcd.drawBitmap(30, 75, 59, 59, (uint16_t *)error_logo);
      M5.Lcd.setCursor(110, 70);
      M5.Lcd.print("CHECK YOUR");
      M5.Lcd.setCursor(110, 90);
      M5.Lcd.print("WI-FI SETTINGS");
      M5.Lcd.setCursor(110, 110);
      M5.Lcd.print("AND TAP");
      M5.Lcd.setCursor(110, 130);
      M5.Lcd.setTextColor(0xe8e4);
      M5.Lcd.print("POWER");
      M5.Lcd.setTextColor(0x7bef);
      M5.Lcd.print(" BUTTON");
      while(true);
    }
    else
    {
      M5.Lcd.fillRoundRect(0, 0, 320, 240, 7, 0xffff);
      message("wifi is starting");
      M5.Lcd.drawBitmap(40, 80, 59, 59, (uint16_t *)radio_logo);  
      M5.Lcd.setCursor(130, 70);
      M5.Lcd.print("WEATHER RADIO");
      M5.Lcd.setCursor(130, 90);
      M5.Lcd.print("TRANSMITTER");
      M5.Lcd.setCursor(130, 110);
      M5.Lcd.print("433 MHz");
      M5.Lcd.setCursor(130, 130);
      M5.Lcd.print("ON M5STACK");
    }
  } 
  url = configOpenWeather();
  if (url == "")
    {
      M5.Lcd.fillRoundRect(0, 0, 320, 240, 7, 0xffff);
      M5.Lcd.drawBitmap(30, 75, 59, 59, (uint16_t *)error_logo);
      M5.Lcd.setCursor(110, 70);
      M5.Lcd.print("CHECK YOUR");
      M5.Lcd.setCursor(110, 90);
      M5.Lcd.print("OPENWEATHER");
      M5.Lcd.setCursor(110, 110);
      M5.Lcd.print("SETTINGS AND TAP");
      M5.Lcd.setCursor(110, 130);
      M5.Lcd.setTextColor(0xe8e4);
      M5.Lcd.print("POWER");
      M5.Lcd.setTextColor(0x7bef);
      M5.Lcd.print(" BUTTON");
      while(true);
    }
    message("radio is starting");
    radioTX.begin(1000);        
    radioTX.openWritingPipe(0); 
}

unsigned long crntTime;
unsigned long prTransmit = 0;
unsigned long prUpdate = 0;

void loop() {
  crntTime = millis();
  if ((crntTime - prUpdate) >= 20000)
  {
    prUpdate = crntTime;
    message("connecting to openweather");
    weather = GET(url);
    if (weather != "")
    {
      message("GET - ok");
      temp = parseString(2, ':', parseString(7, ',', weather));
      pres = parseString(1, ':', parseString(8, ',', weather));
      hum = parseString(1, ':', parseString(9, ',', weather));
      desc = parseString(1, '"', parseString(1, ':', parseString(4, ',', weather))); 
      weatherIcon = parseString(1, '"', parseString(1, ':', parseString(5, ',', weather))); 
    }
    else
    {
      message("GET - bad");
    }
  }
  crntTime = millis();
  if ((crntTime - prTransmit) >= 10000)
  {
    prTransmit = crntTime;
    if (weatherIcon != "")
    {
      message("sending of radio data");
      sendString("1" + String((char)0x1d) + temp);
      delay(1);
      sendString("2" + String((char)0x1d) + String(round(pres.toInt() * 0.75)));
      delay(1);
      sendString("3" + String((char)0x1d) + hum);
      delay(1);
      sendString("4" + String((char)0x1d) + desc);
      delay(1);
      sendString("5" + String((char)0x1d) + weatherIcon);
      delay(1);
      message("radio data has sent");
    }
    else
    {
      message("waiting for radio data");
    }
  }
}
