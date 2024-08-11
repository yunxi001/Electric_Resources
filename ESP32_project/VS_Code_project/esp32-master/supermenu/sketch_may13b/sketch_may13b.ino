#include "supermenu.h"
#include "public.h"
#include <SPI.h>







//FREERTOS系统相关
#include <FreeRTOSConfig.h>
xSemaphoreHandle xMutex; //互斥量
extern bool isSleep = false;  //是否睡眠
int lightPin     = TFT_BL; //屏幕背光控制
extern uint8_t back_light = 150;
extern uint8_t back_light1 =back_light;
int overset = 10;//定义超时时间
int timeoutchick = overset;
int sleeptime=0;

void sleep();





WiFiUDP Udp;

TFT_eSPI tft = TFT_eSPI();
 WeatherNow weatherNow; 
String hour; //时间变量
String minute;
String minute2;
String UserKey = "c3f210e24dd546feb682c9a80e60d6fe";   // 私钥 https://dev.heweather.com/docs/start/get-api-key
String Location = "101150101"; // 城市代码 https://github.com/heweather/LocationList,表中的 Location_ID
String Unit = "m";             // 公制-m/英制-i
String Lang = "zh";            // 语言 英文-en/中文-zh
float ROUND = 0.5;             // 更新间隔<小时>空气质量API 1小时更新一次


int battPin      = 34;     //电量检测
//******STAR*********按键定义******************************
//

#define NTP1 "ntp1.aliyun.com"//时间地址
#define NTP2 "ntp2.aliyun.com"//时间地址
#define NTP3 "ntp3.aliyun.com"//时间地址

SdFs sd;//这个代表是SD卡文件系统
ESDFont SDFont;//字库对象
FsFile file;  //文件对象
FileEnumerator fe; //文件目录对象


void (*current)(void);//菜单回调函数
unsigned char funIndex = 0;//菜单索引


//******end***********************************************


bool buttonchick = false;
bool buttonchick2 = false;
bool pgmbuttonchick = true;//检查和用于程序的退出

//定义按键操作数据
KEY_TABLE table[22] =
{
  //*********一级菜单*********
  {0, 0, 1, 0, 0, (*menu11)},
  {1, 0, 1, 1, 2, (*menu12)},
  //*********二级菜单*********
  {2, 2, 3, 1, 12, (*menu21)},
  {3, 2, 4, 1, 13, (*menu22)},
  {4, 3, 5, 1, 14, (*menu23)},
  {5, 4, 6, 1, 15, (*menu24)},
  {6, 5, 7, 1, 16, (*menu25)},
  {7, 6, 8, 1, 17, (*menu26)},
  {8, 7, 9, 1, 18, (*menu27)},
  {9, 8, 10, 1, 19, (*menu28)},
  {10, 9, 11, 1, 20, (*menu29)},
  {11, 10, 11, 1, 21, (*menu210)},
  //**********三级菜单*********
  {12, 12, 12, 2, 12, (*menu31)},
  {13, 13, 13, 3, 13, (*menu32)},
  {14, 14, 14, 4, 14, (*menu33)},
  {15, 15, 15, 5, 15, (*menu34)},
  {16, 16, 16, 6, 16, (*menu35)},
  {17, 17, 17, 7, 17, (*menu36)},
  {18, 18, 18, 8, 18, (*menu37)},
  {19, 19, 19, 9, 19, (*menu38)},
  {20, 20, 20, 10, 20, (*menu39)},
  {21, 21, 21, 11, 21, (*menu310)},
};

//************************************************************************************************

void task1(void* param)
{

  while(1)
  {
    int core = xPortGetCoreID();  //获取当前核

    if(xSemaphoreTake(xMutex, portMAX_DELAY))
    {       
     sleep();
     xSemaphoreGive(xMutex);
    }
   
  }
  vTaskDelete(NULL);  //结束任务
  
}




//睡眠
void sleep() 
{
timeoutchick--;
delay(1000);

            while((isSleep == false)&&(timeoutchick<=0))
            {
              while(back_light1>0&&(isSleep == false))
              {
              back_light1 =back_light1-5 ;
              analogWrite(TFT_BL, back_light1);
              delay(40);
              }
              analogWrite(TFT_BL, 0);              
              isSleep=true;                                         
            }

            while((isSleep == true)&&(timeoutchick>0))
            {         
             back_light1 =back_light ;
             analogWrite(TFT_BL, back_light1);
             isSleep = false;
             current = table[funIndex].operation;//根据需要获取对应需要执行的函数
             sleeptime=0;
            }
            if((isSleep == true)&&(timeoutchick<=0))
            {
              sleeptime++;
              delay(1000);
              if(sleeptime>=60)
              {
                 esp_sleep_enable_ext0_wakeup(GPIO_NUM_0,0);
                 esp_light_sleep_start();
                 }             
             }

                         
   
}
void buttonscan()
{

  button.tick();
  delay(10);
  button1.tick();
  delay(10);
  button2.tick();
  delay(10);
  
}
//***************************************************************************************************



  





void setup()
{
    tft.init();//tft初始化
    tft.setSwapBytes(true);//tft显示防反转
    wifiinit();
    clockint();
    sdcardinit();
    fe.Init();
    setClock();
    showTitle();
    buttonchick=0;
    buttonSET();
    matrix_effect.init(&tft);

  TaskHandle_t handle1;
  TaskHandle_t handle2;
  xMutex = xSemaphoreCreateMutex();//创建互斥锁
  xTaskCreatePinnedToCore(task1, "task1", 2000, NULL, 15, &handle1, 0);

  current = table[funIndex].operation;//根据需要获取对应需要执行的函数

  
}
 
void loop()
{  
buttonscan();
}
