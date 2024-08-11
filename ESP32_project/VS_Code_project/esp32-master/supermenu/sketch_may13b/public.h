#include <TFT_eSPI.h>
#include <WiFi.h>
#include "icon.h"
#include "font.h"
#include "SPI.h"
#include <time.h>
#include <Arduino.h>
#include <ESP32_Heweather.h>
#include "MyI2S.h"
#include "wave.h"
#include "SdFat.h"
#include "ESDFont.h"
#include <pgmspace.h>
#include "FileEnumerator.h"
#include <TJpg_Decoder.h>
#include<string.h>
#include <time.h>
#include <DigitalRainAnimation.hpp>
#define NTP1 
#define NTP2 
#define NTP3 

extern int overset;//定义超时时间
extern int timeoutchick;
extern DigitalRainAnimation<TFT_eSPI> matrix_effect;

extern WiFiUDP Udp;//声明UDP对象
extern TFT_eSPI tft; //TFT
extern WeatherNow weatherNow;         // 建立weatherNow对象
extern SdFs sd;//这个代表是SD卡文件系统
extern ESDFont SDFont;
extern FsFile file;
extern FileEnumerator fe;

extern String hour; //时间变量
extern String minute;
extern String minute2;
extern unsigned char funIndex;
extern bool buttonchick;
extern bool buttonchick2;

extern MyI2S mi;

extern char buf[128]; //文件名
extern const char *nowfilename;


extern int duoji;
extern int duojichange;


void kong();
void udpsend(IPAddress udpIP,uint16_t udpport,String S);
void changeduojiadd() ;
void changeduojisub();    //舵机



void duojisend();
void wifistmint();
void st123();
void wifiinit();
void microphone();
void clockint();
void setClock();
void showNum(String text, int16_t x, int16_t y, uint16_t color) ;
void showText(String text, int32_t x, int32_t y, uint32_t color, uint16_t bg);
int textWidth(String text);
void weather2();
void weather();
void showTitle();
void sdcardinit();
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap);
void showjpgpic(char picname[128]);
