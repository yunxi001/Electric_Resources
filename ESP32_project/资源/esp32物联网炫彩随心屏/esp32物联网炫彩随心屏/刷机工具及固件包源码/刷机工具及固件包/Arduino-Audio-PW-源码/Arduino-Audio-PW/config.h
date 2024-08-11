#include <EEPROM.h>
#include "OneButton.h"
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <WiFiUdp.h>
#include <arduinoFFT.h>

#include "Pixfont.h"
#include "Cnfont.h"

#define LED_PIN     4       
#define buttonPin   2       
#define NUMPIXELS      64                           =
int   maxBrightness = 250;
#include <NTPClient.h>

#define SAMPLES 16        // Must be a power of 2
#define MIC_IN A0         // Use D0 for mic input
#define xres 8            // Total number of  columns in the display
#define yres 8            // Total number of  rows in the display
#define WIFI_NAME "matrix-draw"
#define MDNS_NAME "emoji"

#define DRAW_MODE 0
#define TIME_MODE 1
#define DATE_MODE 2
#define FFT_MODE 3
#define LOOP_MODE 4
#define MODE_MAX 4

Adafruit_NeoMatrix *matrix = new Adafruit_NeoMatrix(xres, yres, LED_PIN,
                            NEO_MATRIX_RIGHT   + NEO_MATRIX_BOTTOM  + 
                            NEO_MATRIX_COLUMNS   + NEO_MATRIX_ZIGZAG +
                            NEO_GRB + NEO_KHZ800);
//CRGB leds[NUMPIXELS];
/*
  const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255), matrix.Color(0, 100, 255), matrix.Color(100, 0, 255)
  };

*/
const double samplingFrequency = 3000; //Hz, 

unsigned int sampling_period_us;
unsigned long microseconds;

double vReal[SAMPLES];
double vImag[SAMPLES];
int freq_gain2[xres] = {40, 42, 45, 47, 51, 55, 57, 59};
int Intensity[xres] = { }; // initialize Frequency Intensity to zero
int Displacement = 2;          // Create LED Object
arduinoFFT FFT = arduinoFFT();  // Create FFT object

int x = matrix->width();
/*** 保存的配置 ***/
int Mode = 1;
uint8_t color_index = 0;
int brightnessNow;
int clockMode;
int point;
int LoopModeNow = TIME_MODE;

int hueh = 20;
int huem = 180;
int hues = 280;
uint8_t saturation = 100;//
uint8_t value = 100;//

String  H;
String  M;
String  S;
int D;//
int monthDay = 1;
int currentMonth = 1;

int colorLoop = 0;
struct ColorRGB{
  int r;
  int g;
  int b;
};

ColorRGB ColorVarInit(int r, int g, int b){
  ColorRGB ex;
  ex.r = r;
  ex.g = g;
  ex.b = b;
  return ex;
}

String binarydata[10]={
  "0000","0001",
  "0010","0011",
  "0100","0101",
  "0110","0111",
  "1000","1001"
};

String bitdata20[13]={
  "01110101010101010111","00100110001000100111",
  "01110001011101000111","01110001011100010111",
  "01010101011100010001","01110100011100010111",
  "01110100011101010111","01110001000100010001",
  "01110101011101010111","01110101011100010111",
  "01010101011101010101","01110010001000100010",
  "01110010011100010011"
};

char socketMessage[500] = { 'p', '1', '1' };

const uint8_t emoji[] PROGMEM = {0,0,0,0,0,0,205,255,39,205,255,39,205,255,39,205,255,39,0,0,0,0,0,0,0,0,0,205,255,39,205,255,39,205,255,39,205,255,39,205,255,39,205,255,39,0,0,0,205,255,39,0,0,0,205,255,39,205,255,39,205,255,39,205,255,39,0,0,0,205,255,39,205,255,39,0,0,0,205,255,39,205,255,39,205,255,39,205,255,39,0,0,0,205,255,39,205,119,168,205,119,168,205,255,39,205,255,39,205,255,39,205,255,39,205,119,168,205,119,168,205,119,168,205,119,168,205,255,39,205,255,39,205,255,39,205,255,39,205,119,168,205,119,168,0,0,0,205,255,39,205,255,39,0,0,0,0,0,0,205,255,39,205,255,39,0,0,0,0,0,0,0,0,0,205,255,39,205,255,39,205,255,39,205,255,39,0,0,0,0,0,0};

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
WiFiUDP udp;
NTPClient timeClient(udp, "ntp1.aliyun.com", 60 * 60 * 8, 30 * 60 * 1000);
OneButton button(buttonPin, true);

//
enum charfont
{
    Apple4x6,   //
    Apple5x7,   //
    Apple6x10,  //
    Pix5x7,     //
    Atari8x8,   //
    Undertale8x8, //
    Blodnum8x8, //
    Cnfont8x8,  //
    Clock3x5,   // :
    Clock3x4,   //
};

struct FontAttribute  //
{
    const unsigned char *font_bitmap;  //
    const unsigned char *font_index;  //
    int font_num;   //
    int font_width; //
    int font_height; //
    int font_test_x; //
    int font_test_y; //
    int font_test_space;  //
};

struct FontAttribute fontattribute[]=    //
{
    {
        apple4x6_bitmap,
        apple_index,
        sizeof(apple_index) / sizeof(apple_index[0]),
        4,
        6,
        3,
        1,
        4,
    },
    {
        apple5x7_bitmap,
        apple_index,
        sizeof(apple_index) / sizeof(apple_index[0]),
        5,
        7,
        2,
        1,
        5,
    },
    {
        apple6x10_bitmap,
        apple6x10_index,
        sizeof(apple6x10_index) / sizeof(apple6x10_index[0]),
        6,
        10,
        2,
        -1,
        6,
    },
    {
        Pix5x7_bitmap,
        Pix5x7_index,
        sizeof(Pix5x7_index) / sizeof(Pix5x7_index[0]),
        5,
        7,
        2,
        1,
        6,
    },
    {
        Atari8x8_bitmap,
        Atari8x8_index,
        sizeof(Atari8x8_index) / sizeof(Atari8x8_index[0]),
        8,
        8,
        1,
        1,
        8,
    },
    {
        Undertale8x8_bitmap,
        Undertale8x8_index,
        sizeof(Undertale8x8_index) / sizeof(Undertale8x8_index[0]),
        8,
        8,
        1,
        1,
        8,
    },
    {
        Blodnum8x8_bitmap,
        Blodnum8x8_index,
        sizeof(Blodnum8x8_index) / sizeof(Blodnum8x8_index[0]),
        8,
        8,
        0,
        0,
        8,
    },
    {
        Cnfont_bitmap,
        (const unsigned char *)Cnfont_index,
        CNFONTLEN,
        8,
        8,
        0,
        0,
        8,
    },
    {
        Clock3x5_bitmap,
        Clock3x5_index,
        sizeof(Clock3x5_index) / sizeof(Clock3x5_index[0]),
        3,
        5,
        2,
        1,
        4,
    },
    {
        Clock3x4_bitmap,
        Clock3x4_index,
        sizeof(Clock3x4_index) / sizeof(Clock3x4_index[0]),
        3,
        4,
        2,
        2,
        4,
    },
};

//**********************************************
void ShowIP();
uint16_t hsv2rgb(uint16_t hue, uint8_t saturation, uint8_t value);
uint16_t cn_utf8_to_unicode(const uint32_t cInput);
void showCharacter(int char_x, int char_y, int c, int charfont, int showHue);
void drawFastXLineGXY(int16_t x, int16_t y, int16_t h, bool s);
void drawFastXLine(int16_t x, int16_t y, int16_t h, int16_t c);
void drawFastYLine(int16_t x, int16_t y, int16_t h, int16_t c);
uint16_t Gradient(int x, int y);
void haloworld();
void showWiFi();
void configSync(uint8_t num);
void showbitnumber(int number, int xlength, int ylength, int x, int y);
