#include "public.h"
#define NTP1 "ntp1.aliyun.com"
#define NTP2 "ntp2.aliyun.com"
#define NTP3 "ntp3.aliyun.com"

String  humidity;  
String temp;  
String weather1;
const char *ssid = "123";                                                                                // WIFI账户
const char *password = "123456789";                                                                                 // WIFI密码
const String WDAY_NAMES[] = {"Sum", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};  //星期

DigitalRainAnimation<TFT_eSPI> matrix_effect = DigitalRainAnimation<TFT_eSPI>();

MyI2S mi;


const int record_time = 10;  // second
const char filename[] = "/我的录音.wav";
const int waveDataSize = record_time * 88200;
int32_t communicationData[1024];     //接收缓冲区
char partWavData[1024];

uint8_t img_buff[50000] PROGMEM= {0};//用于存储传过来的图片


//bool buttonchick = false;
//bool buttonchick2 = false;
//bool pgmbuttonchick = true;//检查和用于程序的退出
int sWidth  = TFT_WIDTH;   //屏幕宽度
int sHeight = TFT_HEIGHT;  //屏幕高度
int fgColor =TFT_BLACK;  //背景颜色
int bgColor = TFT_ORANGE;  //标题颜色
double voltage =0;

//UDP TCP相关

extern  IPAddress tcpserverIP(192,168,4,1); //目标地址
extern uint16_t tcpserverPort =6068; 

int duojichange=0;



void kong()
{
  timeoutchick=overset;
}



void udpsend(IPAddress udpIP,uint16_t udpport,String S)
{   
  timeoutchick=overset;
      /*将接受到的数据发送回去*/
    Udp.flush();  
    Udp.beginPacket(udpIP,udpport);  //准备发送数据到目标IP和目标端口
    Udp.print(S);  //将接收到的数据放入发送的缓冲区    
    Udp.endPacket();  //向目标IP目标端口发送数据 
    delay(500); 
    Udp.flush();
}

void changeduojiadd()    //舵机
{
  timeoutchick=overset;
  
  duoji =duoji+10; 
  
    if(duoji>180)
  {
    duoji=180;
  } 
  delay(100);
    if(  duojichange!=duoji)
  { 
  udpsend(tcpserverIP,6068,String(duoji)); 
  delay(100);

  }
}
void changeduojisub()    //舵机
{

  timeoutchick=overset;
  
  duoji =duoji-10; 
if(duoji<0)
{
  duoji=0;
  }
   delay(100);
     if(  duojichange!=duoji)
  {
 udpsend(tcpserverIP,6068,String(duoji));
  delay(100);
 
  }
}

void duojisend()
{

}

void st123()
{  
  
  file = sd.open("/读写测试", O_WRITE|O_CREAT);

  file.write("东北大学秦皇岛分校",128);
  file.close();
  }

void microphone()
{
  //删除并创建文件
  sd.remove(filename);
  file = sd.open(filename, O_WRITE|O_CREAT);
  if(!file)
  {
    Serial.println("crate file error");
    return;
  }

  auto header = CreateWaveHeader(1, 44100, 16);
  header.riffSize = waveDataSize + 44 - 8;
  header.dataSize = waveDataSize;
  file.write(&header, 44);

  if(!mi.InitInput(I2S_BITS_PER_SAMPLE_32BIT, 17, 16, 12))
  {
    Serial.println("init i2s error");
    return;
  }

  Serial.println("start");

  for (int j = 0; j < waveDataSize/1024; ++j) {
    auto sz = mi.Read((char*)communicationData, 4096);
    char*p =(char*)(communicationData);
    for(int i=0;i<sz/4;i++)
    {
      communicationData[i] *= 20;  //提高声音
      if(i%2 == 0)   //这里获取到的数据第一个Int32是右声道
      {
          partWavData[i] = p[4*i + 2];
          partWavData[i + 1] = p[4*i + 3];
      }
    }
    file.write((const byte*)partWavData, 1024);
  }
  file.close();

  Serial.println("finish");
}


void wifiinit()
{
  Serial.begin(115200);
  Serial.println();
  //设置ESP32工作模式为无线终端模式
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  delay(500);
  int a =8 ;
  while (WiFi.status() != WL_CONNECTED)
  { 
    matrix_effect.loop();
  }
  Serial.println("WiFi connected!");
  
}

void clockint()
{

  configTime(8 * 3600, 0, NTP1, NTP2, NTP3);

  }




//time_t now; //实例化时间
void setClock()
{
  struct tm timeInfo; //声明一个结构体
  if (!getLocalTime(&timeInfo))
  { //一定要加这个条件判断，否则内存溢出
    Serial.println("Failed to obtain time");
    return;
  }
  //Serial.print(asctime(&timeInfo)); //默认打印格式：Mon Oct 25 11:13:29 2021
  String date = WDAY_NAMES[timeInfo.tm_wday];
  String shuju = String(timeInfo.tm_hour); //年//时
  shuju += ":";
  shuju += timeInfo.tm_min;
  shuju += ":";
  shuju += timeInfo.tm_sec;
  shuju += " ";
  //showText(shuju.c_str(), 20, 6, fgColor);

  if(timeInfo.tm_hour<10){
  hour =("0"+String(timeInfo.tm_hour));
  hour +=":";
  }
  else{
     hour = String(timeInfo.tm_hour)+=":";
  }
  
  if(timeInfo.tm_min<10){
  minute =("0"+String(timeInfo.tm_min));
  
  }
  else{
     minute = String(timeInfo.tm_min);  
  }
}








//显示数字（大）
void showNum(String text, int16_t x, int16_t y, uint16_t color) {
  tft.setTextSize(1);
  tft.setTextColor(color);
  tft.drawString(text, x, y, 6);
  //tft.setFreeFont(&Orbitron_Light_32);
  //tft.drawString(text, x, y);
}



//显示中英文
void showText(String text, int32_t x, int32_t y, uint32_t color) {
  int i = 0;
  int x1 = x;
  while (i < text.length()) {
    char t1 = text.charAt(i);
    if (t1 >= 0 && t1 <= 127) {
      //英文字符
      tft.setTextSize(1);
      tft.setTextColor(color);
      tft.drawString((String)t1, x1, y+2, 2);
      i ++;
      x1 += 10;
    } else {
      //全角字符
      char t2 = text.charAt(i+1);
      char t3 = text.charAt(i+2);
      //全角占3个字符
      for (int k = 0; k < sizeof(FONT_CN) / sizeof(FONT_CN[0]); k++){
        if (FONT_CN[k].cnIndex[0] == t1 && FONT_CN[k].cnIndex[1] == t2 && FONT_CN[k].cnIndex[2] == t3)
          tft.drawBitmap(x1, y, FONT_CN[k].cnName, FONT_CN[k].cnPixel, FONT_CN[k].cnPixel, color);
      }
      i += 3;
      x1 += 20;
    }
  }
}
//获取字符宽度
int textWidth(String text) {
  int i = 0;
  int x1 = 0;
  while (i < text.length()) {
    char t1 = text.charAt(i);
    if (t1 >= 0 && t1 <= 127) {
      i ++;
      x1 += 10;
    } else {
      i += 3;
      x1 += 20;
    }
  }
  return x1;
}


void weather2()
{ 
  
  
    showText(humidity, 55, 170, fgColor);
    showText(temp, 105, 170, fgColor);
    showText(weather1, 175, 170, fgColor);
    if(weather1=="晴")
    tft.pushImage(20, 160, 32, 32, weather_1); 
    if(weather1=="多云")
    tft.pushImage(20, 160, 32, 32, weather_2); 
    if(weather1=="阴")
    tft.pushImage(20, 160, 32, 32, weather_3); 
    if(weather1=="雨")
    tft.pushImage(20, 160, 32, 32, weather_4);
    if(weather1=="雪")
    tft.pushImage(20, 160, 32, 32, weather_5); 
  }

void weather()
{
  if(weatherNow.get()){ 
    showText(String(weatherNow.getHumidity()), 55, 170, fgColor);
    humidity=String(weatherNow.getHumidity());  //获取湿度
    showText(String(weatherNow.getTemp()), 105, 170, fgColor);
    temp=String(weatherNow.getTemp());
    showText(String(weatherNow.getWeatherText()), 175, 170, fgColor);
    weather1=String(weatherNow.getWeatherText());
    if(String(weatherNow.getWeatherText())=="晴")
    tft.pushImage(20, 160, 32, 32, weather_1); 
    if(String(weatherNow.getWeatherText())=="多云")
    tft.pushImage(20, 160, 32, 32, weather_2); 
    if(String(weatherNow.getWeatherText())=="阴")
    tft.pushImage(20, 160, 32, 32, weather_3); 
    if(String(weatherNow.getWeatherText())=="雨")
    tft.pushImage(20, 160, 32, 32, weather_4);
    if(String(weatherNow.getWeatherText())=="雪")
    tft.pushImage(20, 160, 32, 32, weather_5);    
  } else {  // 更新失败
  }
}

void showTitle() {
 
  //显示标题
  tft.drawFastHLine(5, 29, sWidth - 10, TFT_DARKGREY);
  int iconW = 40;
  //蓝牙图标
  tft.pushImage(sWidth - iconW, 6, 16, 16, blueth_on);
  iconW += 20;
  //Wifi图标
  if (WiFi.status() == WL_CONNECTED) {
    tft.pushImage(sWidth - iconW, 6, 16, 16, wifi_on);
  } else {
    tft.pushImage(sWidth - iconW, 6, 16, 16, wifi_off);
  }
  iconW += 20;



  //电池图标


  if (voltage > 4.2) voltage = 4.2;
  if (voltage >= 4) {
    tft.pushImage(sWidth - iconW, 6, 16, 16, battery_1);
  } else if (voltage >= 3.9) {
    tft.pushImage(sWidth - iconW, 6, 16, 16, battery_2);
  } else if (voltage >= 3.7) {
    tft.pushImage(sWidth - iconW, 6, 16, 16, battery_3);
  } else if (voltage >= 3.5) {
    tft.pushImage(sWidth - iconW, 6, 16, 16, battery_4);
  } else if (voltage < 3.5) {
    tft.pushImage(sWidth - iconW, 6, 16, 16, battery_5);
  }
  showText(String(voltage) + "V", sWidth - iconW - 50, 5, fgColor);
}



bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
   // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);

  // This might work instead if you adapt the sketch to use the Adafruit_GFX library
  // tft.drawRGBBitmap(x, y, bitmap, w, h);

  // Return 1 to decode next block
  return 1;
}
void sdcardinit()//sdjpg图片显示初始化**sd卡目录初始化
{
    if(!sd.begin(5,18000000))
  {
    Serial.println("init sd error!");
    return;
  }
  SDFont.Init(&sd, &tft);  //初始化ESDFont
  if(SDFont.SetFont("/123.bin"))  //选择字库
  {
    Serial.println("3");
    SDFont.setColor(TFT_WHITE, TFT_BLACK); //设置文字颜色
  }
   TJpgDec.setJpgScale(1);
  //TJpgDec.setSwapBytes(true);
  //  // The decoder must be given the exact name of the rendering function above
  TJpgDec.setCallback(tft_output);

  fe.Init();
  
}
void showjpgpic(char picname[128])
{
    // Draw the image, top left at 0,0
  memset(img_buff,0,50000);
  file.open(picname,O_RDONLY);
  file.read(img_buff,50000);
  file.close();
  TJpgDec.drawJpg(0,0,img_buff, sizeof(img_buff));
  memset(img_buff,0,50000);
  // How much time did rendering take (ESP8266 80MHz 271ms, 160MHz 157ms, ESP32 SPI 120ms, 8bit parallel 105ms
  // Wait before drawing again
}


void wifistmint()
{ 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
     while (WiFi.status() == WL_CONNECTED)
  { 
    weather();
    clockint();
    WiFi.disconnect();
    Serial.println("WiFi stop"); 
  }
}  
