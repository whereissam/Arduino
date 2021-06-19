/*
   LineNotifyClient範例程式
   https://github.com/TridentTD/TridentTD_LineNotify
*/

//引入函式庫
#include <WiFi.h>
#include <TridentTD_LineNotify.h> //LineNotify library

//LineNotify參數
#define LINE_TOKEN  "vtsLHVK64EWoOb3JNl9SS1vbqO6aJGKa3sYyif5c1Bu"      //LineNotify API Token

//Wifi熱點連線資料
char* ssid = "Sam phone";               //Wifi熱點ssid
char* password = "00000000";          //Wifi熱點密碼



//***************************************************************
//******啟始程序
//***************************************************************
void setup(void) {
  Serial.begin(9600);           //啟始監控視窗
  WiFi.begin(ssid, password);   //啟始Wifi連線
}


//***************************************************************
//******工作迴圈程序
//***************************************************************
void loop(void) {
  delay(10000);      //延遲10secs

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected.");
    //將ip顯示在監控視窗
    Serial.println(WiFi.localIP());
    //呼叫LineNotify程序
    LineNotify();
  }
  else {
    Serial.println("WiFi disconnected!");
    WiFi.begin(ssid, password);                  //啟始Wifi連線
    int tryTimes = 10;                           //嘗試連線次數
    while (tryTimes-- > 0) {                     //(要執行其他程式，不能一直在試連線)
      if (WiFi.status() == WL_CONNECTED) break;  //跳出迴圈
      Serial.print(".");
      delay(500);
    }
  }
}


//***************************************************************
//******Line Notify程序
//***************************************************************
void LineNotify() {

  // 設定Line Token
  LINE.setToken(LINE_TOKEN);

  // 一般訊息
  LINE.notify("這是IOT通知");

  // 數字
  LINE.notify(2342);          //整數
  LINE.notify(212.43434, 5);  //浮點數

  // 貼圖Line Sticker https://devdocs.line.me/files/sticker_list.pdf
  LINE.notifySticker(3, 240);       // Line Sticker PackageID 3 , StickerID 240
  LINE.notifySticker("Hello", 1, 2); // Line Sticker PackageID 1 , StickerID 2 

  //圖片 url
  LINE.notifyPicture("https://preview.ibb.co/j6G51n/capture25610417181915334.png");
  LINE.notifyPicture("圖片url", "https://www.fotoaparat.cz/storage/pm/09/10/23/670915_a5351.jpg");

}
