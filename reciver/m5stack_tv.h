namespace m5stack_tv
{
  #include "CompositeGraphics.h"
  #include "Image.h"
  #include "CompositeOutput.h"
  #include <soc/rtc.h>
  #include "font6x8.h"
  
  const int XRES = 320;
  const int YRES = 200;

  CompositeGraphics graphics(XRES, YRES);
  CompositeOutput composite(CompositeOutput::NTSC, XRES * 2, YRES * 2);
  Font<CompositeGraphics> font(6, 8, font6x8::pixels);

  char* strToChar(String str) {
    int len = str.length() + 1;
    char* buf = new char[len];
    strcpy(buf, str.c_str());
    return buf;
  }

  void compositeCore(void *data) {  
    while (true)
    {
      composite.sendFrameHalfResolution(&graphics.frame);
    }
  }
  
  void begin() {
    rtc_clk_cpu_freq_set(RTC_CPU_FREQ_240M);
    composite.init();
    graphics.init();
    graphics.setFont(font);
    xTaskCreatePinnedToCore(compositeCore, "c", 2048, NULL, 1, NULL, 0);
  }

  void setTextColor(int c) {
    graphics.setTextColor(c);
  }
  
  void setCursor(int x, int y) {
    graphics.setCursor(x, y);
  }

  void print(String str) {
    graphics.print(strToChar(str));
  }

  void drawBitmap(int x, int y, int w, int h, const unsigned char* img) {   
    Image<CompositeGraphics> img_(w, h, img);
    img_.draw(graphics, x, y);
  }

  void fillRect(int x, int y, int w, int h, int color = 0) {
    graphics.fillRect(x, y, w, h, color);
  }

  void drawDot(int x, int y, int color) {
    graphics.dotFast(x, y, color);
  }

  void clearScreen(int color = 0) {
    fillRect(0, 0, XRES, YRES, color);
  }
}

