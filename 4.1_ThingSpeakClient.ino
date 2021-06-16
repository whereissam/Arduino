/*
   ThingSpeakClient範例程式
*/

//引入函式庫
#include <WiFi.h>
#include <ThingSpeak.h>

//Wifi熱點連線資料
char* ssid = "Sam phone";               //Wifi熱點ssid
char* password = "00000000";          //Wifi熱點密碼
bool wifiFlag = false;                //Wifi狀態

//ThingSpeak變數
WiFiClient  client;
unsigned long myChannelNumber = 1417393;      //ChannelID(數字)
const char * myWriteAPIKey = "ONV5CXY1I2KVEEU0";    //WriteAPI Key(字串)



//***************************************************************
//******啟始程序
//***************************************************************
void setup(void) {
  Serial.begin(9600);           //啟始監控視窗
  ConnectWifi();                //Wifi連線程序
  ThingSpeak.begin(client);     //啟始ThingSpeak
}


//***************************************************************
//******工作迴圈程序
//***************************************************************
void loop(void) {
  ConnectWifi();    //Wifi連線程序

  //***ThingSpeak上傳資料程序
  if (wifiFlag) {
    ThingSpeak.setField(1, random(20, 40));     //設定field1為溫度（測試用亂數）
    ThingSpeak.setField(2, random(50, 70));     //設定field2為濕度（測試用亂數）
    //寫入 ThingSpeak channel
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (x == 200) {
      Serial.println("Channel update successful.");   //成功
    }
    else {
      Serial.println("Problem updating channel. HTTP error code " + String(x)); //失敗
    }
    delay(15000); // Wait 15 seconds to update the channel again
  }
}


//***************************************************************
//******Wifi連線程序
//***************************************************************
void ConnectWifi() {
  wifiFlag = false;
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected.");
    //將ip顯示在監控視窗
    Serial.println(WiFi.localIP());
    wifiFlag = true;
  }
  else {
    Serial.println("WiFi disconnected!");
    WiFi.begin(ssid, password);                  //啟始Wifi連線
    int tryTimes = 10;                           //嘗試連線次數
    while (tryTimes-- > 0) {                     //(要執行其他程式，不能一直在試連線)
      if (WiFi.status() == WL_CONNECTED) {
        wifiFlag = true;
        break;  //跳出迴圈
      } Serial.print(".");
      delay(500);
    }
  }
}
