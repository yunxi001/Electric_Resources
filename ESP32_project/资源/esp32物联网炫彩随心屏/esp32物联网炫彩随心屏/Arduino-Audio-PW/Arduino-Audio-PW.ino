/*
  

*/

#include "config.h"

void showH_M(bool implement);
void draw_one(int16_t x, int16_t y, int16_t h);
uint8_t MouthDay_flag = 2;
uint8_t showTime_flag = 1;
uint16_t color_one = 0;
/*
#include <arduinoFFT.h>

#define SAMPLES1 64        // Must be a power of 
//#define MIC_IN A0         // Use A0 for mic input
//#define LED_PIN     4     // Data pin to LEDS  D2
//#define NUM_LEDS    64    // 灯珠数
//#define BRIGHTNESS  250    // LED 亮度 
//#define LED_TYPE    WS2812B
//#define COLOR_ORDER GRB 
//#define xres 8            // Total number of  columns in the display
//#define yres 8            // Total number of  rows in the display

double vReal1[SAMPLES1];
double vImag1[SAMPLES1];

//幅值算法*************************************************************
//从group取出特征角标取出初始幅值→加上shift偏移→乘gain倍率→除以gain2倍率
//暂时只用了freq_gain2
//int freq_group[8] = {2, 5, 12, 20, 30, 38, 50, 75};
//int freq_shift[8] = {-50, -10, -10, -10, -5, -5, -5, -5};
//int freq_gain[8] = {1, 1, 1, 1, 2, 2, 4, 4};
int freq_gain21[8] = {80, 70, 60, 55, 50, 40, 50, 60};
//算出来的长度值
unsigned char freq_block[8] = {0,0,0,0,0,0,0,0};

int Intensity1[xres] = { }; // initialize Frequency Intensity to zero
int Displacement1 = 1;

//CRGB leds[NUM_LEDS];            // Create LED Object
//arduinoFFT FFT = arduinoFFT();  // Create FFT object

//int color = 0; //颜色变化
//int color_rate = 1;  //单次变换间隔倍数
//int color_interval = 1;  //颜色变换间隔
*/

// handle WebSocket message
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        //Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        // send message to client
        webSocket.sendTXT(num, "Connected");
        configSync(num);  //
      }
      break;
    case WStype_TEXT:
      {
//        Serial.printf("[%u] get Text[%u].\n", num, length);
        strcpy(socketMessage, (char *)payload);
        if (socketMessage[0] == 'l')  //
        {
          modeChange();
        } else if (socketMessage[0] == 'b')  //
        {
          if (socketMessage[1] == 'n')  //
          {
            int bn = ((int)socketMessage[2] - 48) * 100 + ((int)socketMessage[3] - 48) * 10 + (int)socketMessage[4] - 48;
            if (bn >= 0 && bn <= 255) brightnessNow = bn;
            matrix->setBrightness(brightnessNow);
            matrix->show();
          } else {
            brightnessChange();
          }
        } else if (socketMessage[0] == 'c')  //
        {
          if (socketMessage[1] == 'c') {
            clockChange();
          } else {
            colorChange();
          }
        } else if (socketMessage[0] == 's')  //
        {
          statusSave();
        } else if (socketMessage[0] == 'p')  //
        {
          pointChange();
        } else if (socketMessage[0] == 'h') {  //
          int hh = ((int)socketMessage[2] - 48) * 100 + ((int)socketMessage[3] - 48) * 10 + (int)socketMessage[4] - 48;
//          Serial.print("传入色值：");
//          Serial.println(hh);
          if (hh >= 0 && hh <= 360) {
            switch (socketMessage[1]) {
              case 'h':
                hueh = hh;
                break;
              case 'm':
                huem = hh;
                break;
              case 's':
                hues = hh;
                break;
              default:
                break;
            }
          }
        } else if (socketMessage[0] == 'e') {  //emoji
          if (Mode == DRAW_MODE) {
            String txt = String();
 //           Serial.println(*payload);
 //           Serial.println(payload);
 //           uint8_t i = 0;
            for (uint8_t y = 0; y < yres; y++) {
              for (uint8_t x = 0; x < xres; x++) {
                uint8_t r = payload[y * 8 * 3 + x * 3 + 1] * 2;
                uint8_t g = payload[y * xres * 3 + x * 3 + 2] * 2;
                uint8_t b = payload[y * xres * 3 + x * 3 + 3] * 2;
                matrix->drawPixel(x, y, matrix->Color(r, g, b));
                //Serial.printf("(%u, %u) color: %u, %u, %u\n", x, y, r, g, b);
              }
            }   
            matrix->show();  // This sends the updated pixel color to the hardware.
          }
        }
      }
      break;
  }
}

void setup() {
  Serial.begin(115200);
//  delay(3000);                  // power-up safety delay
 // FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip ); //Initialize LED strips
 // FastLED.setBrightness(BRIGHTNESS);

  delay(1000);

  EEPROM.begin(16);
  // Check if this is first time EEPROM is used

  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(50);
  matrix->show();  // This sends the updated pixel color to the hardware.
 // helloWorld();
  showWiFi();
  //WiFiManager
  WiFiManager wifiManager;
  wifiManager.autoConnect(WIFI_NAME);
  // start MDNS
  if (MDNS.begin(MDNS_NAME)) {
    Serial.println("MDNS responder started.");
  }
  button.attachClick(colorChange);                 //表示当按钮被单击时执行的操作
  button.attachDoubleClick(modeChange);            //表示当按钮被双击时执行的操作
  button.attachMultiClick(statusSave);             //表示当按钮被多次点击时执行的操作。
  button.attachDuringLongPress(brightnessChange);  //表示当按钮被长按时执行的操作。
  // start web server
  // handle index
  server.on("/", []() {
    // send index.html
    server.send(200, "text/html", "<!DOCTYPE html><html><head> <meta http-equiv='Content-Type' content='text/html; charset=utf-8' /> <meta name='viewport' content='user-scalable=no,initial-scale=2.0' /> <style> body { background: #000; color: #fff; text-align: center; font-size: small; } div { margin: 10px } </style></head><body> <div> <h3>Pick An Emoji</h3> <img id='emojis' onclick='clickEmoji(event);' src='data:image/png;base64,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'> </div> <div> <label id='out'></label> <canvas id='emoji' width='8' height='8'></canvas> </div> <div> <button onclick='sendCode(Point)'>屏幕方向</button> <button onclick='sendCode(Mode)'>模式切换</button> </div> <div> <span>亮度</span> <label><input id='bn' type='range' min='0' max='255' step='1' oninput='sendBrightness()'/></label>  </div> <div> <button onclick='sendCode(ClockMode)'>时钟样式</button> <button onclick='sendCode(Color)'>颜色变更</button> </div> <div> <span>小时</span> <label><input id='hueh' type='range' min='0' max='360' step='1' oninput='sendColor(hueh)'/></label>  </div> <div> <span>分钟</span> <label><input id='huem' type='range' min='0' max='360' step='1' oninput='sendColor(huem)'/></label>  </div> <div> <span>秒钟</span> <label><input id='hues' type='range' min='0' max='360' step='1' oninput='sendColor(hues)'/></label> </div> <div><button onclick='sendCode(Save)'>配置保存</button></div> <script> var Mode = 'l'; var Point = 'p'; var ClockMode = 'cc'; var Color = 'c'; var Save = 's'; var hueh = 'h'; var huem = 'm'; var hues = 's'; function nw() { return new WebSocket('ws://' + location.hostname + ':81/', ['arduino']); }  var ws = nw();  ws.onmessage = function (e) { console.log('Server: ', e.data);  if(e.data[0]=='{'){ console.log('json'); var jsonOb = eval('('+e.data+')'); var brightNess = typeof(jsonOb.brightNess)=='undefined'? 150:jsonOb.brightNess; var hueh = typeof(jsonOb.hueh)=='undefined'? 180:jsonOb.hueh; var huem = typeof(jsonOb.huem)=='undefined'? 180:jsonOb.huem; var hues = typeof(jsonOb.hues)=='undefined'? 180:jsonOb.hues; document.getElementById('bn').value=parseInt(brightNess); document.getElementById('hueh').value=parseInt(hueh); document.getElementById('huem').value=parseInt(huem); document.getElementById('hues').value=parseInt(hues);  } }; var sk = 0; function cc(c) { return String.fromCharCode(c / 2); } function clickEmoji(e) {  var xo = 3; var yo = 3; var xs = 13; var ys = 12; var x = e.offsetX; var y = e.offsetY;  var c = Math.round((x - xo - 4) / xs);  var r = Math.round((y - yo - 4) / ys);  document.getElementById('out').innerText = 'Selected [' + c + ', ' + r + ']';  var img = document.getElementById('emojis');  var ce = document.getElementById('emoji');  var ctx = ce.getContext('2d');  ctx.drawImage(img, -(xo + (xs * c)), -(yo + (ys * r)), 131, 122); var d = ctx.getImageData(0, 0, 8, 8).data;  var t = 'e';  for (var i = 0; i < 8; i++) { for(var j = 0; j < 8; j++){ var imageData = ctx.getImageData(j, i, 1, 1).data; t += cc(imageData[0]); t += cc(imageData[1]); t += cc(imageData[2]); } } ws.send(t, { binary: true }); } function sendCode(c){ console.log('发送命令：'+c); ws.send(c); } function sendBrightness() {  var Obrightness = document.getElementById('bn').value;  var oMessage = 'bn';  if (Obrightness.length == 1) {  oMessage = oMessage + '00';  } else if (Obrightness.length == 2) {  oMessage = oMessage + '0';  }  oMessage = oMessage + parseInt(Obrightness ? Obrightness : 255);  console.log('oMessage: ' + oMessage); ws.send(oMessage); } function sendColor(n){ var OHueValue = document.getElementById('hue'+n).value;  var oMessage = 'h'+n;  if (OHueValue.length == 1) {  oMessage = oMessage + '00';  } else if (OHueValue.length == 2) {  oMessage = oMessage + '0';  } oMessage = oMessage + parseInt(OHueValue ? OHueValue : 999);  console.log('oMessage: ' + oMessage); ws.send(oMessage);  } </script> </body> </html>");
    Serial.println("index.html sent.");
  });
  server.begin();

  // start webSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  // Add service to MDNS
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("ws", "tcp", 81);
  if (EEPROM.read(15) != 123) {  // not equal to 123
    Serial.println("未查到默认配置");
    Mode = 0;
    color_index = 0;
    brightnessNow = 50;
    clockMode = 1;
    point = 1;
  } else {  // if byte 256 is equal to 123
    Serial.println("存有默认配置");
    Mode = int(EEPROM.read(0));  // get previous lit leds before power failure
    color_index = int(EEPROM.read(1));
    brightnessNow = int(EEPROM.read(2));
    clockMode = int(EEPROM.read(3));
    point = int(EEPROM.read(4));
    hueh = int(EEPROM.read(5)) * 2;
    huem = int(EEPROM.read(6)) * 2;
    hues = int(EEPROM.read(7)) * 2;
  }
  matrix->setBrightness(brightnessNow);
  matrix->show();
  setPoint();
  ShowIP();
 // sampling_period_us = round(1000000000 * (1.0 / samplingFrequency));  //
 sampling_period_us = round(1000000000 * (1.0 / 1000));
  switchMode(true);
}


unsigned long lastTime = 0;
unsigned long lastColorTime = 0;
unsigned long lastLoopTime = 0;
unsigned int counter = 0;

//设置屏幕朝向
void setPoint() {
  matrix->clear();
  switch (point) {
    case 1:
      matrix = new Adafruit_NeoMatrix(xres, yres, LED_PIN,
                                      NEO_MATRIX_RIGHT + NEO_MATRIX_BOTTOM + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG + NEO_GRB + NEO_KHZ800);
      break;
    case 2:
      matrix = new Adafruit_NeoMatrix(xres, yres, LED_PIN,
                                      NEO_MATRIX_TOP + NEO_MATRIX_RIGHT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG + NEO_GRB + NEO_KHZ800);
      break;
    case 3:
      matrix = new Adafruit_NeoMatrix(xres, yres, LED_PIN,
                                      NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE + NEO_GRB + NEO_KHZ800);
      break;
    case 4:
      matrix = new Adafruit_NeoMatrix(xres, yres, LED_PIN,
                                      NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE + NEO_GRB + NEO_KHZ800);
      break;
  }
  matrix->setBrightness(brightnessNow);
  matrix->show();
}

void pointChange() {
  if (++point > 4) point = 1;  //
  Serial.println("变换朝向");
  Serial.println(point);
  setPoint();
}

//
void modeChange() {
  matrix->clear();
  if (++Mode > MODE_MAX) Mode = 0;  //
  Serial.println("按键切换模式");
  Serial.println(Mode);
  switchMode(true);
}
//
void clockChange() {
  matrix->clear();
  if (++clockMode > 1) clockMode = 0;  //
  Serial.println("按键切换时钟");
  Serial.println(clockMode);
  switchMode(true);
}
//
void colorChange() {
  if (++color_index > 3) color_index = 0;  //
  Serial.println("按键切换颜色");
  Serial.println(color_index);
  switchMode(true);
}
//
void brightnessChange() {
  unsigned long t = millis();
  if ((t - lastTime) > 500) {
    brightnessNow += 10;
    if (brightnessNow > 100) {
      brightnessNow = 10;
    }
    matrix->setBrightness(brightnessNow);
    matrix->show();
    lastTime = millis();
  }
}


//RGB颜色数组https://www.sojson.com/rgb.html
const ColorRGB myRGBColorPalette[4][2] = {
  { ColorVarInit(255, 102, 0), ColorVarInit(30, 144, 255) },
  { ColorVarInit(255, 255, 0), ColorVarInit(255, 0, 77) },
  { ColorVarInit(0, 255, 0), ColorVarInit(148, 0, 211) },
  { ColorVarInit(0, 255, 255), ColorVarInit(255, 0, 255) }
};

//
void statusSave() {
  Serial.println("按键保存配置");
  if (EEPROM.read(15) != 123) {  // not equal to 123
    EEPROM.write(15, 123);       // write value 123 to byte 256
  }
  EEPROM.write(0, Mode);
  EEPROM.write(1, color_index);
  EEPROM.write(2, brightnessNow);
  EEPROM.write(3, clockMode);
  EEPROM.write(4, point);
  EEPROM.write(5, hueh / 2);
  EEPROM.write(6, huem / 2);
  EEPROM.write(7, hues / 2);
  EEPROM.commit();
  int x = xres;
  for (int i = 0; i < 36; i++) {
    matrix->clear();
    matrix->fillScreen(0);
    matrix->setCursor(x, 0);
    matrix->print(F("Saved"));
    matrix->setTextColor(matrix->Color(148, 0, 211));
    if (--x < -36) {
      x = matrix->width();
    }
    matrix->show();
    system_soft_wdt_feed();
    delay(70);
  }
  matrix->clear();
  switchMode(true);
}

//如果implement为true，则清空LED矩阵，并显示绘图板内容，然后将内容刷新到LED矩阵上。
//无论implement的值如何，都会检查是否到达了10秒的时间间隔，
//如果是，则执行一次计数器逻辑，并输出WebSocket连接的状态信息。
void showDrawingBoard(bool implement) {

  unsigned long t = millis();
  if (implement) {
    matrix->clear();
    showEmoji();
    matrix->show();
  }
  if ((t - lastTime) > 10 * 1000 | implement) {
    counter++;
    bool ping = (counter % 2);
    int i = webSocket.connectedClients(ping);
//    Serial.printf("%d Connected websocket clients ping: %d\n", i, ping);
    lastTime = millis();
  }
}

void showEmoji() {
//  for (uint8_t y = 0; y < yres; y++) {
//    for (uint8_t x = 0; x < xres; x++) {
 //     uint8_t r = pgm_read_dword(&(emoji[y * xres * 3 + x * 3]));
 //     uint8_t g = pgm_read_dword(&(emoji[y * xres * 3 + x * 3 + 1]));
 //     uint8_t b = pgm_read_dword(&(emoji[y * xres * 3 + x * 3 + 2]));
 //    matrix->drawPixel(x, y, matrix->Color(r, g, b));

 //   }
 // }
                for (uint8_t y = 0; y < yres; y++) {
              for (uint8_t x = 0; x < xres; x++) {
                uint8_t r = emoji1[y * 8 * 3 + x * 3 + 1] * 2;
                uint8_t g = emoji1[y * xres * 3 + x * 3 + 2] * 2;
                uint8_t b = emoji1[y * xres * 3 + x * 3 + 3] * 2;
                matrix->drawPixel(x, y, matrix->Color(r, g, b));
                //Serial.printf("(%u, %u) color: %u, %u, %u\n", x, y, r, g, b);
              }
            }
}

/**
 * @brief 
 * 
 * @param implement 
 */
void showTime(bool implement) {
   if (millis() - lastTime > 5000 | implement) {
  timeClient.update();
  H = timeClient.getFormattedTime().substring(0, 2);
  M = timeClient.getFormattedTime().substring(3, 5);
  if (millis() - lastColorTime > 300 | implement) {
    if (!implement) {
      lastColorTime = millis();
    }
  }
  /*
    switch (clockMode) {
      case 0:
        colorLoop = colorLoop + 1;
        if (colorLoop > 54) {
          colorLoop = 1;
        }
//        showHours();
//        showbittime(M, 3);
          showH_M(implement);
        break;
      case 1:
        S = timeClient.getFormattedTime().substring(6, 8);
        binaryClock();
        break;
    }
*/
    showH_M(implement);
 //   matrix->show();
  }
}
unsigned long lastH_MTime = 0;
void showH_M(bool implement)
{
  if (millis() - lastH_MTime > 3000) {
    switch (showTime_flag) {
      case 1:
        showTime_flag = 2;
        break;
      case 2:
        showTime_flag = 1;
        break;
    }
    implement = true;
    lastH_MTime = millis();
    matrix->clear();
  
  switch (showTime_flag) {
    case 2:
//      showTime(implement);
      showbittime(H, 1);
      draw_one(0, 0, 2);
      break;
    case 1:
//      showMouthDay(implement);
      showbittime(M, 1);
      draw_one(0, 0, 4);
      break;
  }
    drawFastXLine(1, 7, 7, hsv2rgb(hueh, saturation - 50, value - 50));
  switch (D) {
    case 0:
      drawFastXLine(7, 7, 1, hsv2rgb(hueh, saturation, value));
      break;
    default:
      drawFastXLine(D, 7, 1, hsv2rgb(hueh, saturation, value));
      break;
  }
    matrix->show();
  }
}
/**
 * @brief 
 * 
 * @param implement 
 */

unsigned long lastMouthDayTime = 0;
void showMouthDay(bool implement) {
  if (millis() - lastTime > 5000 | implement) {
    timeClient.update();
    D = timeClient.getDay();
    //
    unsigned long epochTime = timeClient.getEpochTime();
    struct tm *ptm = gmtime((time_t *)&epochTime);
    monthDay = ptm->tm_mday;
    currentMonth = ptm->tm_mon + 1;
//    Serial.println(monthDay);
 //   Serial.println(currentMonth);
    lastTime = millis();
    matrix->clear();
  }
  
  if (millis() - lastMouthDayTime > 3000) {
    switch (MouthDay_flag) {
      case 1:
        MouthDay_flag = 2;
        break;
      case 2:
        MouthDay_flag = 1;
        break;
    }
    implement = true;
    lastMouthDayTime = millis();
    matrix->clear();
  
  switch (MouthDay_flag) {
    case 1:
//      showTime(implement);
draw_one(0, 0, 6);
      showbitnumber(currentMonth, 4, 5, 0, 1);
      
      break;
    case 2:
//      showMouthDay(implement);
draw_one(0, 0, 8);
      showbitnumber(monthDay, 4, 5, 0, 1);
      
      break;
  }
//  showbitnumber(monthDay, 4, 5, 0, 1);
  drawFastXLine(1, 7, 7, hsv2rgb(hueh, saturation - 50, value - 50));
  switch (D) {
    case 0:
      drawFastXLine(7, 7, 1, hsv2rgb(hueh, saturation, value));
      break;
    default:
      drawFastXLine(D, 7, 1, hsv2rgb(hueh, saturation, value));
      break;
  }
  matrix->show();
  }
}

void draw_one(int16_t x, int16_t y, int16_t h){
  for(int i=x;i<x+h;i++){
    if(color_one >= 360)
    color_one = 5;
    matrix->drawPixel(i,y,hsv2rgb(color_one, saturation, value));
    color_one += 12;
  }
}

void showLoopMode(bool implement) {
  if (millis() - lastLoopTime > 6000) {
    switch (LoopModeNow) {
      case DATE_MODE:
        LoopModeNow = TIME_MODE;
        break;
      case TIME_MODE:
        LoopModeNow = DATE_MODE;
        break;
    }
    implement = true;
    lastLoopTime = millis();
    matrix->clear();    
  }
  switch (LoopModeNow) {
    case TIME_MODE:
      showTime(implement);
      break;
    case DATE_MODE:
      showMouthDay(implement);
      break;
  }
}


void binaryClock() {
  int h1, h2;
  int s1, s2;
  int m1, m2;
  if (H.toInt() < 10) {
    h1 = 0;
    h2 = (String(H.charAt(1)).toInt() + 1) - 1;
  } else {
    h1 = (String(H.charAt(0)).toInt() + 1) - 1;
    h2 = (String(H.charAt(1)).toInt() + 1) - 1;
  }
  showBinary(h1, 0, hueh);
  showBinary(h2, 1, hueh);

  if (M.toInt() < 10) {
    m1 = 0;
    m2 = (String(M.charAt(1)).toInt() + 1) - 1;
  } else {
    m1 = (String(M.charAt(0)).toInt() + 1) - 1;
    m2 = (String(M.charAt(1)).toInt() + 1) - 1;
  }
  showBinary(m1, 3, huem);
  showBinary(m2, 4, huem);

  if (S.toInt() < 10) {
    s1 = 0;
    s2 = (String(S.charAt(1)).toInt() + 1) - 1;
  } else {
    s1 = (String(S.charAt(0)).toInt() + 1) - 1;
    s2 = (String(S.charAt(1)).toInt() + 1) - 1;
  }
//  Serial.println(s1);
//  Serial.println(s2);
  showBinary(s1, 6, hues);
  showBinary(s2, 7, hues);
}

void showBinary(int num, int x, int hue) {
  for (int i = 0; i < 4; i++) {
    if (String(binarydata[num].charAt(i)).toInt() != 0) {
      matrix->drawPixel(x, 2 + i, hsv2rgb(hue, 100, 100));
    } else {
      matrix->drawPixel(x, 2 + i, hsv2rgb(hue, 60, 30));
    }
  }
}

void showHours() {
  int hours = timeClient.getHours();
  uint16_t colorxy = hsv2rgb(hueh, 100, 100);
  if (hours > 12) {
    hours = hours - 12;
  }
  drawFastXLineGXY(0, 0, 8, false);
  drawFastXLineGXY(0, 1, 8, false);
  int n = hours / 3;
  int surplus = hours % 3;
  switch (n) {
    case 0:
      drawFastXLineGXY(1, 0, surplus, true);
      break;
    case 1:
      drawFastXLineGXY(1, 0, 3, true);
      drawFastXLineGXY(5, 0, surplus, true);
      break;
    case 2:
      drawFastXLineGXY(1, 0, 3, true);
      drawFastXLineGXY(5, 0, 3, true);
      drawFastXLineGXY(1, 1, surplus, true);
      break;
    case 3:
      drawFastXLineGXY(1, 0, 3, true);
      drawFastXLineGXY(5, 0, 3, true);
      drawFastXLineGXY(1, 1, 3, true);
      drawFastXLineGXY(5, 1, surplus, true);
      break;
    case 4:
      drawFastXLineGXY(1, 0, 3, true);
      drawFastXLineGXY(5, 0, 3, true);
      drawFastXLineGXY(1, 1, 3, true);
      drawFastXLineGXY(5, 1, 3, true);
      break;
  }
}

void showbittime(String nowtime, int y) {
  if (nowtime.toInt() < 10) {
    showbitmap(bitdata20[(int)(0)], 4, 5, 0, y);
    showbitmap(bitdata20[(int)((String(nowtime.charAt(1)).toInt() + 1) - 1)], 4, 5, 4, y);
  } else {
    showbitmap(bitdata20[(int)((String(nowtime.charAt(0)).toInt() + 1) - 1)], 4, 5, 0, y);
    showbitmap(bitdata20[(int)((String(nowtime.charAt(1)).toInt() + 1) - 1)], 4, 5, 4, y);
  }
}


void showbitmap(String bitrgbstr, int xlength, int ylength, int x, int y) {
  //Serial.println("bitrgbstr = " + bitrgbstr);
  for (int i = x; i < x + (xlength); i = i + (1)) {
    for (int j = y; j < y + (ylength); j = j + (1)) {
      if (String(bitrgbstr.charAt(((j - y) * xlength + i - x))).toInt() != 0) {
        matrix->drawPixel(i, j, Gradient(i, j));
      } else {
        matrix->drawPixel(i, j, matrix->Color(0, 0, 0));
      }
    }
  }
}

/*
void getSamples_read()
{
  Serial.println("getSamples_read");
  if(getSamples_flag == 1 & adc_number < 64)
  {
    vReal[adc_number] = analogRead(MIC_IN);
//    Serial.println(vReal[adc_number]);
    vImag[adc_number] = 0;
    Serial.println("adc_number");
    adc_number ++;
    getSamples_flag = 0;
  }
}
*/

void showFFT() {
  //Collect Samples
 // Serial.println("FTT模式");
//   if(adc_number < 64 & getSamples_flag == 0)
//  {
//    Serial.println("getSamples_flag");
//    getSamples_flag = 1;
 // }
 // if(adc_number >= 64)
  

    getSamples();
    //Update Display
    displayUpdate();
    matrix->show();
    delay(1);
}

void getSamples() {
  microseconds = micros();

  for (int i = 0; i < SAMPLES; i++) {    

    vReal[i] = analogRead(MIC_IN);
    
    vImag[i] = 0;
   while (micros() - microseconds < 333) {
      //empty loop
    }
    microseconds += sampling_period_us;
  }
  bool reduce = false;
  if ((millis() - lastTime) > 25) {
    lastTime = millis();
    reduce = true;
  }
  //FFT
  FFT.Windowing(vReal, 1, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
  //Update Intensity Array
//    for(int i = 2; i < (xres*Displacement)+2; i+=Displacement){
//    vReal[i] = constrain(vReal[i],freq_gain2[i-2] ,1800);            // set max value for input data
//    vReal[i] = map(vReal[i], freq_gain2[i-2], 1800, 0, yres);        // map data to fit our display

 //   Intensity[(i/Displacement)-2] --;                      // Decrease displayed value
 //   if (vReal[i] > Intensity[(i/Displacement)-2])          // Match displayed value to measured value
 //     Intensity[(i/Displacement)-2] = vReal[i];
 // }
  for (int i = 0; i < xres; i++) {
    vReal[i] = (vReal[i * Displacement + 1] + vReal[i * Displacement + 2]) / 2;
 //   Serial.println(vReal[i]);
    vReal[i] = constrain(vReal[i],0, 1800);                      // set max value for input data
    vReal[i] = map(vReal[i], freq_gain2[i] + 50, 1800, 0, yres);  // map data to fit our display
    if (reduce) { Intensity[i]--; }                               // Decrease displayed value
    if (vReal[i] > Intensity[i])                                  // Match displayed value to measured value
      Intensity[i] = vReal[i];
  
  }
  
//    for (int i = 0; i < xres; i++) {
//    vReal[i] = (vReal[i * Displacement + 1] + vReal[i * Displacement + 2]) / 2;
//    Serial.println(vReal[i]);
//    vReal[i] = constrain(vReal[i], 0, 1200);                      // set max value for input data
 //   vReal[i] = map(vReal[i], freq_gain2[i] + 50, 1200, 0, yres);  // map data to fit our display
 //   if (reduce) { Intensity[i]--; }                               // Decrease displayed value
 //   if (vReal[i] > Intensity[i])                                  // Match displayed value to measured value
 //     Intensity[i] = vReal[i];
 // }
}
int color = 0;
void displayUpdate() {
  color = color + 9;
  
microseconds = micros();
  for (int i = 0; i < xres; i++) {
    
    drawFastYLine(i, yres - Intensity[i], Intensity[i], hsv2rgb(color + 6, 100, 100));
    drawFastYLine(i, 0, yres - 1 - Intensity[i], matrix->Color(0, 0, 0));

    while (micros() - microseconds < 333) {     
    microseconds += sampling_period_us;
  }
 //   color += 350/xres;  // Increment the Hue to get the Rainbow
    if (color > 350) {
      color = 0;
    }      
 }
}
/*
void showFFT() {
  for(int i = 0; i < 4; i++){
    Visualizer(); 
    delay(1);
    flow(i);
  }
}

void flow(int rate){
  if(rate == 3){
    for(int i = 0; i < 8; i++){
      if(freq_block[i]>0){
        freq_block[i] = freq_block[i]-1;
       }
    }
  }
}

void Visualizer(){
  //Collect Samples
  getSamples();
  
  //Update Display
  displayUpdate();
  
  matrix->show();
}

void getSamples(){
  for(int i = 0; i < SAMPLES1; i++){
    vReal1[i] = analogRead(MIC_IN);
    delay(1);
 //  Serial.println(vReal1[i]);
    vImag1[i] = 0;
  }
  bool reduce = false;
  if ((millis() - lastTime) > 25) {
    lastTime = millis();
    reduce = true;
  }
  //FFT
  FFT.Windowing(vReal1, 1, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal1, vImag1, SAMPLES1, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal1, vImag1, SAMPLES1);

  //Update Intensity Array
    for (int i = 0; i < xres; i++) {
    vReal1[i] = (vReal1[i * Displacement + 1] + vReal1[i * Displacement + 2]) / 2;
    Serial.println(vReal1[i]);
    vReal1[i] = constrain(vReal1[i], 0, 1200);                      // set max value for input data
    vReal1[i] = map(vReal1[i], freq_gain2[i] + 50, 1200, 0, yres);  // map data to fit our display
    if (reduce) { Intensity1[i]--; }                               // Decrease displayed value
    if (vReal1[i] > Intensity1[i])                                  // Match displayed value to measured value
      Intensity1[i] = vReal1[i];
  }
}

void displayUpdate(){
  int color = 0;
/*
  drawFastYLine(i, yres - Intensity[i], Intensity[i], hsv2rgb(color + 5, 100, 100));
  drawFastYLine(i, 0, yres - 1 - Intensity[i], matrix->Color(0, 0, 0));
  void drawFastYLine(int16_t x, int16_t y, int16_t h, int16_t c){
  for(int i=y;i<y+h;i++){
    matrix->drawPixel(x,i,c);
  }
}

for(int i = 0; i < xres; i++){
    for(int j = void switchMode(bool change); j < yres; j++){
      if(j <= Intensity1[i]){                                // Light everything within the intensity range
//        if(j%2 == 0){
//          leds[(xres*(j+1))-i-1] = CHSV(color, 255, BRIGHTNESS);
//        }
//        else{
//          leds[(xres*j)+i] = CHSV(color, 255, BRIGHTNESS);
//        }
        if(j>freq_block[i]){
          freq_block[i] = min(j+1,8); 
        }
//leds[(xres*j)+i] = CHSV(color, 255, BRIGHTNESS);
matrix->drawPixel(i,j,hsv2rgb(color + 5 , 100, 100));
      }
      else{                                                  // Everything outside the range goes dark
//        if(j%2 == 0){
//          leds[(xres*(j+1))-i-1] = CHSV(color, 255, 0);
//        }
//        else{
//          leds[(xres*j)+i] = CHSV(color, 255, 0);
//        }
        if(j == freq_block[i]){
        //  leds[(xres*j)+i] = CHSV(color, 0, BRIGHTNESS);//白色坠落点
          matrix->drawPixel(i,j,hsv2rgb(color + 5, 100, 200));
        }else{
        //  leds[(xres*j)+i] = CHSV(color, 255, 0);
          matrix->drawPixel(i,j,hsv2rgb(color + 2, 255, 0));
        }   
      }
    }
    color += 255/xres;             // Increment the Hue to get the Rainbow
    if(color >= 255)
    color = 0;
  }

for(int i = 0; i < xres; i++){
    for(int j = yres - Intensity1[i]; j < yres; j++){
      matrix->drawPixel(i,j,hsv2rgb(color + 5, 100, 100));
    }
}
for(int i = 0; i < xres; i++){
    for(int j = 0; j < yres - 1; j++){
      matrix->drawPixel(i,j,matrix->Color(0, 0, 0));
    }
}
    color += 255/xres;             // Increment the Hue to get the Rainbow
    if(color >= 255)
    color = 0;
}
*/
  
void switchMode(bool change) {
  switch (Mode) {
    case DRAW_MODE:
      showDrawingBoard(change);
      break;
    case TIME_MODE:
      showTime(change);
      break;
    case DATE_MODE:
      
      showMouthDay(change);
      break;
    case FFT_MODE:
      
      showLoopMode(change);
      break;
    case LOOP_MODE:
    showFFT();
      
      break;
  }
}
/*
void switchMode(bool change) {
  switch (Mode) {
    case DRAW_MODE:
      showDrawingBoard(change);
      break;
    case TIME_MODE:
      showLoopMode(change);
      break;
    case DATE_MODE:
      showFFT();
      break;
  }
}
*/
void loop() {
  button.tick();
  MDNS.update();
  webSocket.loop();
  server.handleClient();
  switchMode(false);
}
