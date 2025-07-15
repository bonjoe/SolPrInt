#include "screen.h"

// Joy‑IT wiring on ESP32 VSPI
#define TFT_CS   -1   // no CS pin, tied high
#define TFT_DC    5   // DC → GPIO5
#define TFT_RST   4   // RST → GPIO4
static Adafruit_ST7789 lcd(TFT_CS, TFT_DC, TFT_RST);

void screen_init() {
    SPI.begin(18, 19, 23);         // SCLK=18, MISO=19 (unused), MOSI=23
    lcd.init(240, 240, SPI_MODE2);
    lcd.setRotation(1);
    lcd.fillScreen(ST77XX_BLACK);
}

void screen_displayDashboard(double hourOfYear, double power, double pvPower, double  soc, double irrWeek, double prevAction,double currSpeed) {
     int year = 2025;
     bool leap = ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
     int daysPerMonth[] = {31, 28 + leap, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
     
     int totalDays = int(hourOfYear / 24.0);
     double hf = fmod(hourOfYear, 24.0);
     int hh = int(hf);int mm = int((hf - hh) * 60);int doy = totalDays + 1;int mo = 1;int day = doy;
     for (int i = 0; i < 12; ++i) {
         if (day > daysPerMonth[i]) {day -= daysPerMonth[i];mo++;} 
         else {break;}
         }

     char tbuf[6];char dbuf[11];
     snprintf(tbuf, sizeof(tbuf), "%02d:%02d", hh, mm);snprintf(dbuf, sizeof(dbuf), "%04d-%02d-%02d", year, mo, day);   
     lcd.fillScreen(ST77XX_BLACK);
   
     //Time & Date on top
     lcd.setTextSize(3); lcd.setTextColor(ST77XX_WHITE);
     lcd.setCursor(10, 5);lcd.print(tbuf);
     
     // Position date to the right of time
     int16_t tx, ty; uint16_t tw, th;
     lcd.getTextBounds(tbuf, 0, 0, &tx, &ty, &tw, &th);
     lcd.setTextSize(2);
     lcd.setCursor(10 + tw + 8, 5 + (th - 16));lcd.print(dbuf);

     uint16_t y = 45;
     const int leftX      = 10;
     const int barX       = 144;
     const int rightColX  = barX + 2;
     const int rightWidth = 240 - rightColX - 10;
     lcd.setTextSize(2);lcd.setTextColor(ST77XX_WHITE);
     // Line 1: Pwr on left, “SOC” header on right
     lcd.setCursor(leftX, y); lcd.print("PP: "); lcd.print(power, 1); lcd.print("W");
     lcd.drawFastVLine(barX, y, 16 * 2, ST77XX_WHITE);
     {
       const char* hdr = "SOC";
       int16_t tx, ty; uint16_t tw, th;
       lcd.getTextBounds(hdr, 0, 0, &tx, &ty, &tw, &th);
       lcd.setCursor(rightColX + (rightWidth - tw) / 2, y); lcd.print(hdr);
     }   
     // Line 2: PV on left, SOC value on right
     lcd.setCursor(leftX, y + 20); lcd.print("PV: "); lcd.print(pvPower, 1); lcd.print("W");
     lcd.drawFastVLine(barX, y + 20, 16 * 2, ST77XX_WHITE);
     {
       char buf[8]; snprintf(buf, sizeof(buf), "%d%%", int(soc));
       int16_t tx, ty; uint16_t tw, th;
       lcd.getTextBounds(buf, 0, 0, &tx, &ty, &tw, &th);
       lcd.setCursor(rightColX + (rightWidth - tw) / 2, y + 20); lcd.print(buf);
     } 
  
     // Line 3: IrrW on left, (right side empty)
     lcd.setTextSize(1);
     lcd.setCursor(leftX, y + 40);lcd.print("IrrWeek:"); lcd.print(irrWeek, 1); lcd.print("W");
     lcd.drawFastVLine(barX, y + 40, 10, ST77XX_WHITE);   
     //Previous action + current action on one line
     lcd.setTextSize(2);lcd.setTextColor(ST77XX_WHITE);
     lcd.setCursor(10, 125); lcd.print(prevAction, 0);lcd.print(" ");
     int16_t ax = lcd.getCursorX();int16_t ay = lcd.getCursorY() + 4;
     drawRightArrow(ax, ay, 16, ST77XX_WHITE);lcd.setCursor(ax + 20 + 8, lcd.getCursorY());
           
     int intPart = int(currSpeed); int decPart = int((currSpeed - intPart) * 10);
      
      // 2) Prepare strings
      char bufInt[6];  sprintf(bufInt, "%d",    intPart);
      char bufDec[4];  sprintf(bufDec, ".%d",   decPart);
      const char* unit = "rpm";
      
      // 3) Measure each segment’s pixel width/height
      int16_t x1, y1;uint16_t wInt, hInt, wDec, hDec, wUnit, hUnit;
      lcd.setTextSize(6);lcd.getTextBounds(bufInt, 0, 0, &x1, &y1, &wInt, &hInt);
      lcd.setTextSize(3); lcd.getTextBounds(bufDec, 0, 0, &x1, &y1, &wDec, &hDec);
      lcd.setTextSize(2); lcd.getTextBounds(unit,   0, 0, &x1, &y1, &wUnit, &hUnit);
      
      // 4) Compute centered start X
      uint16_t totalW = wInt + wDec + wUnit;
      int16_t startX = (240 - totalW) / 2 + 8;
      int16_t yBase  = 125;  // your baseline Y
      
      lcd.setTextSize(6);lcd.setCursor(startX, yBase);  lcd.print(bufInt);     
      lcd.setTextSize(3); lcd.setCursor(startX + wInt, yBase + (hInt - hDec)); lcd.print(bufDec); 
      lcd.setTextSize(2);lcd.setCursor(startX + wInt + wDec, yBase + (hInt - hUnit));  lcd.print(unit);

     const char* mode;
     if (currSpeed == 0)       mode = "Off";
     else if (currSpeed == 20) mode = "Eco";
     else if (currSpeed == 70) mode = "Power";
     else                      mode = "Medium";
     char mBuf[32]; snprintf(mBuf, sizeof(mBuf), "Mode: %s", mode);
     uint16_t textW, textH;
     lcd.setTextSize(3);lcd.getTextBounds(mBuf, 0, 0, &x1, &y1, &textW, &textH);
     int16_t cx = (240 - textW) / 2;
     lcd.setCursor(cx, 200);lcd.print(mBuf);

}
      
void drawRightArrow(int x, int y, int length, uint16_t color) {
     lcd.drawLine(x, y, x + length, y, color);
     const int head = 8;const int halfH = 4; 
     lcd.fillTriangle(x + length + head, y,x + length, y - halfH, x + length, y + halfH, color);
}
