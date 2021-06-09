/*
   DHTdisplay2進階範例程式
*/

//引入函式庫
#include <DHT.h>;
#include <U8g2lib.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

//定義常數 SSD1306
#define DHTPIN 32             //感測器接GPIO 32
#define DHTTYPE DHT22         //感測器類型 DHT 22  (AM2120)
#define I2C_OLED_SCL    15    //螢幕SCL GPIO 15        
#define I2C_OLED_SDA    4     //螢幕SDA GPIO 4
#define VERSION "3.4_DHTdisplay2: "   //程式名稱及版本

//定義全域變數
DHT dht(DHTPIN, DHTTYPE); //定義感測器全域變數
float hum;                //定義濕度變數
float temp;               //定義溫度變數
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ I2C_OLED_SCL, /* data=*/ I2C_OLED_SDA, /* reset=*/ U8X8_PIN_NONE);   //螢幕物件
char* message = "DHT Display v1.0";   //系統訊息
bool marqueeFlag = true;              //跑馬燈狀態
bool wifiFlag = false;                //Wifi狀態
bool cloudUploadFlag = false;         //雲端上傳狀態
//Wifi熱點連線資料
char* ssid = "Sam phone";               //Wifi熱點ssid
char* password = "00000000";          //Wifi熱點密碼

WiFiUDP ntpUDP;
//**語法 NTPClient(UDP& udp, const char* poolServerName, long timeOffset);
//string[] TWNTPServers = { "clock.stdtime.gov.tw", "tock.stdtime.gov.tw",
//"watch.stdtime.gov.tw", "time.stdtime.gov.tw", "tick.stdtime.gov.tw" };
//使用台灣的NTP Server, timeOffset 28800 = +8hours*60*60*1000 ticks
NTPClient timeClient(ntpUDP, "clock.stdtime.gov.tw", 28800);
char* currentTime = "現在時間";        //時間訊息


//***************************************************************
//******啟始程序
//***************************************************************
void setup(void) {
  Serial.begin(9600);         //啟始監控視窗
  dht.begin();                //啟始感測器
  u8g2.begin();               //啟始螢幕
  u8g2.enableUTF8Print();     //允許顯示UTF8字串
  wifiFlag = WifiConnect();   //檢查Wifi連線
  timeClient.begin();         //啟始NTPCient
}



//***************************************************************
//******工作迴圈程序
//***************************************************************
void loop(void) {
  wifiFlag = WifiConnect();   //檢查Wifi連線
  ReadDHT();                  //讀取感測器數值
  DHTdisplay();               //OLED顯示內容
 }







//***************************************************************
//******Wifi連線程序
//******回傳值bool（true:連線成功, false:連線失敗）
//***************************************************************
bool WifiConnect() {
  if (IsWifiConnected()) return true;             //傳回wifi連線狀態為真

  //***嘗試連線到Wifi
  Serial.println("Connecting to " + String(ssid));
  WiFi.begin(ssid, password);                     //啟始Wifi連線
  int tryTimes = 10;                              //嘗試連線次數
  while (tryTimes-- > 0) {
    if (IsWifiConnected()) return true;           //傳回wifi連線狀態為真
    Serial.print(".");
    delay(500);
  }
  Serial.println("WiFi disconnected!");
  message = "WiFi disconnected!";
  return false;                                   //傳回wifi連線狀態為假
}



//***************************************************************
//******檢查Wifi是否連線
//******回傳值bool（true:連線成功, false:連線失敗）
//***************************************************************
bool IsWifiConnected() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected.");
    //將ip顯示在監控視窗
    Serial.println(WiFi.localIP());
    //將ip顯示在系統訊息
    IPAddress ip = WiFi.localIP();
    message = new char[40]();
    sprintf(message, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    timeClient.update();          //更新目前時間
    currentTime = new char[40]();
    sprintf(currentTime, "Current Time %d:%d", timeClient.getHours(), timeClient.getMinutes());
    return true;
  }
  else {
    return false;
  }
}



//***************************************************************
//******讀取感測器數值程序
//***************************************************************
void ReadDHT() {
  //***讀取感測器數值
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  //顯示感測器資料到監控視窗
  Serial.print(VERSION);  //顯示程式版本
  Serial.println("Temperature=" + String(temp, 1) + "°C, " + "Humidity=" + String(hum, 1) + "%RH");
}



//***************************************************************
//******OLED顯示內容程序
//***************************************************************
void DHTdisplay() {
  //***OLED顯示內容
  u8g2.clearBuffer();                             //清除緩衝區記憶體

  //顯示溫度內容
  u8g2.setFont(u8g2_font_open_iconic_all_2x_t);
  u8g2.drawGlyph(0, 22, 259);                     //溫度圖示
  u8g2.setFont(u8g2_font_logisoso16_tf);
  u8g2.setCursor(26 , 22);
  u8g2.print(String(temp, 1));                    //溫度數值（小數一位）
  u8g2.setCursor(65 , 22);
  u8g2.print("°C");                               //溫度單位（°C） requires enableUTF8Print()
  //顯示濕度內容
  u8g2.setFont(u8g2_font_open_iconic_all_2x_t);
  u8g2.drawGlyph(0, 44, 152);                     //濕度圖示
  u8g2.setFont(u8g2_font_logisoso16_tf);
  u8g2.setCursor(26 , 44);
  u8g2.print(String(hum, 1));                     //濕度數值（小數一位）
  u8g2.setCursor(70 , 44);
  u8g2.print("%");                                //濕度單位（%）

  //顯示系統狀態
  if (wifiFlag) {
    u8g2.setFont(u8g2_font_open_iconic_all_2x_t);
    u8g2.drawGlyph(110, 22, 247);                   //Wifi圖示
    if (cloudUploadFlag) {
      u8g2.setFont(u8g2_font_open_iconic_all_2x_t);
      u8g2.drawGlyph(90, 22, 126);                    //雲端上傳圖示
    }
    cloudUploadFlag = !cloudUploadFlag;               //***測式顯示效果，實際運用時要移除
  }

  //***顯示系統訊息
  marqueeFlag = !marqueeFlag;                         //訊息顯示效果切換，際運用時可不移除
  if (marqueeFlag) {
    //動態訊息文字跑馬燈
    int16_t offset = -(int16_t)u8g2.getDisplayWidth();
    int16_t len = strlen(currentTime);
    for (;;)                        // then do the scrolling
    {
      drawScrollString(offset, currentTime);  // no clearBuffer required, screen will be partially cleared here
      u8g2.sendBuffer();            // transfer internal memory to the display
      offset += 10;                 //調整跑馬燈速度
      if ( offset > len * 8 + 1 )
        break;
    }
  }
  else {
    //靜態訊息顯示
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.setCursor(0 , 62);
    u8g2.print(message);                            //訊息文字
    u8g2.sendBuffer();                              //送到緩衝區記憶體以顯示字串
    delay(5000);
  }

}



//***************************************************************
//******繪製文字跑馬燈程序
//***************************************************************
void drawScrollString(int16_t offset, const char *s)
{
  static char buf[36];  // should for screen with up to 256 pixel width
  size_t len;
  size_t char_offset = 0;
  u8g2_uint_t dx = 0;
  size_t visible = 0;

  u8g2.setDrawColor(0);   // clear the scrolling area
  u8g2.drawBox(0, 49, u8g2.getDisplayWidth() - 1, u8g2.getDisplayHeight() - 1);
  u8g2.setDrawColor(1);   // set the color for the text

  len = strlen(s);
  if ( offset < 0 )
  {
    char_offset = (-offset) / 8;
    dx = offset + char_offset * 8;
    if ( char_offset >= u8g2.getDisplayWidth() / 8 )
      return;
    visible = u8g2.getDisplayWidth() / 8 - char_offset + 1;
    strncpy(buf, s, visible);
    buf[visible] = '\0';
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.drawStr(char_offset * 8 - dx, 62, buf);
  }
  else
  {
    char_offset = offset / 8;
    if ( char_offset >= len )
      return; // nothing visible
    dx = offset - char_offset * 8;
    visible = len - char_offset;
    if ( visible > u8g2.getDisplayWidth() / 8 + 1 )
      visible = u8g2.getDisplayWidth() / 8 + 1;
    strncpy(buf, s + char_offset, visible);
    buf[visible] = '\0';
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.drawStr(-dx, 62, buf);
  }
}
