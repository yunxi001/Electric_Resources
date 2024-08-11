#include "public.h"
#include "supermenu.h"
//

#define PIN_INPUT 35
#define PIN_INPUT1 12
#define PIN_INPUT2 27 



extern bool programchick=1; //检查和用于程序的退出
int duoji = 0 ;




 OneButton button(PIN_INPUT, true);
 OneButton button1(PIN_INPUT1, true);
 OneButton button2(PIN_INPUT2, true); 

//void watchsleep()
//{
//  
//  
//
//}







void menuright()
{
     timeoutchick=overset;
      funIndex = table[funIndex].right; 
      current = table[funIndex].operation;//根据需要获取对应需要执行的函数
    (*current)();//执行获取到的函数
     buttonchick = 0;
     programchick=1;
}
void menuleft()
{ 
    timeoutchick=overset;
    funIndex = table[funIndex].left;
    current = table[funIndex].operation;//根据需要获取对应需要执行的函数
    (*current)();//执行获取到的函数
    buttonchick = 0;
     programchick=1;
}
void menudown()
{
    timeoutchick=overset;
    funIndex = table[funIndex].down; 
    current = table[funIndex].operation;//根据需要获取对应需要执行的函数
    (*current)();//执行获取到的函数
    buttonchick = 0;
    programchick=1;
}
void menuup()
{ 
  timeoutchick=overset;
  funIndex = table[funIndex].up; 
  current = table[funIndex].operation;//根据需要获取对应需要执行的函数
  (*current)();//执行获取到的函数
  buttonchick = 0;
  programchick=1;
}





void buttonSET(void)//按键初始化
{ timeoutchick=overset;
  button.reset();//清除一下按钮状态机的状态
  button1.reset();//清除一下按钮状态机的状态
  button2.reset();//清除一下按钮状态机的状态
  button.attachClick(menudown);
  button1.attachDoubleClick(menuleft);
  button1.attachClick(menuright);
  button2.attachClick(menuup);
}


 
char buf[128]; //文件名
const char *nowfilename;
void filechoose(char indexname[128],char dirname[128],char filekind[128])
{
  timeoutchick=overset;
  FsFile dir;
  FsFile idx;

  file.remove(indexname);//SD目录存储文件
  if(!idx.open(indexname, O_RDWR|O_CREAT))
  {
    Serial.println("crate file error");
    return;
  }

  // Open root directory
  if (!dir.open( dirname)){
    Serial.println("dir.open failed");
    return;
  }//SD目录存储文件存储目录
  
  while (file.openNext(&dir, O_RDONLY)) {
    memset(buf, 0, 128);
    file.getName(buf, 128);
    fe.Add2(buf,filekind);
    file.close();
  }
  dir.close();

  fe.Flush();
  
       tft.fillRoundRect(0, 28, 240, 280,8,TFT_BLACK);        
       auto filename = fe.Next(); 
       SDFont.DrawText(25, 180, filename);    //输出文字 
       nowfilename = filename;
   
}


void fileup()
{
  
//上翻功能
timeoutchick=overset;
      delay(100);
      tft.fillRoundRect(0, 28, 240, 280,8,TFT_BLACK);    
      auto filename = fe.Prev();
      SDFont.DrawText(25, 180, filename);    //输出文字  
      nowfilename = filename;     
        
}

void sdjpgshow()

{
  timeoutchick=overset;
    String a="/jpg/";
    const char *b=nowfilename;
    a =a+String(b);
    char c[128];  
    strcpy(c,a.c_str());  
    showjpgpic(c);  
}


void filedown()
{
      timeoutchick=overset;
      tft.fillRoundRect(0, 28, 240, 280,8,TFT_BLACK);
      auto filename = fe.Next();
      SDFont.DrawText(25, 180, filename);    //输出文字   
      nowfilename = filename;  
  
}



void functionout()
{
    timeoutchick=overset;
    buttonSET();//按键初始化
   funIndex = table[funIndex].left;
   current = table[funIndex].operation;//根据需要获取对应需要执行的函数
   (*current)();//执行获取到的函数
   buttonchick = 1; 
   programchick =0;
}
void progmloop()
{

        button.tick();
        delay(10);
        button1.tick();
        delay(10);
        button2.tick();
        delay(10);
  
}




void menu11()
{
 
  if(buttonchick==0)//清除残留图像
  {
   delay(100);
  showjpgpic("/beijin.jpg");
  showTitle();
//  weather2();
  setClock();
  showNum(hour.c_str(), 85, 80, TFT_BLACK);
  showNum(minute.c_str(), 110, 130, TFT_BLACK);
  buttonchick = 1;
  }
  minute2 = minute;
  setClock(); 
     if(minute2!=minute)//清除残留图像
  {
   
    delay(100);
  showjpgpic("/beijin.jpg");
  setClock();
  showNum(hour.c_str(), 85, 80,TFT_BLACK);
  showNum(minute.c_str(), 110, 130, TFT_BLACK);
  showTitle();
//  weather2();
  minute2=minute;
    }
    
  }
 
void menu12(void)
{
    if(buttonchick==0)//清除残留图像
  {
   tft.fillRoundRect(0, 0, 240, 280,8,TFT_BLACK); 
   showTitle();
   tft.pushImage(90, 90, 60, 60, sbpic5); 
   SDFont.DrawText(90, 180, "各种应用");    //输出文字
   buttonchick2=buttonchick;
   buttonchick = 1;
  }

  }
void menu21(void)
{
    if(buttonchick==0)//清除残留图像
  {
   tft.fillRoundRect(0, 0, 240, 280,8,TFT_BLACK); 
   showTitle();
   tft.pushImage(90, 90, 60, 60, sbpic4); 
   SDFont.DrawText(90, 180, "相册");    //输出文字
   buttonchick2=buttonchick;
   buttonchick = 1;
  }
 
}
//二级菜单2
void menu22(void)
{
    if(buttonchick==0)//清除残留图像
  {
   tft.fillRoundRect(0, 28, 240, 280,8,TFT_BLACK);
   tft.pushImage(90, 90, 60, 60, sbpic8); 
   SDFont.DrawText(90, 180, "拓展");    //输出文字
   buttonchick2=buttonchick;
   buttonchick = 1;

  }

}
 
//二级菜单3
void menu23(void)
{
    if(buttonchick==0)//清除残留图像
  {
   tft.fillRoundRect(0, 28, 240, 280,8,TFT_BLACK); 
   tft.pushImage(90, 90, 60, 60, sbpic6); 
   SDFont.DrawText(90, 180, "电子书");    //输出文字
   buttonchick2=buttonchick;
   buttonchick = 1;
  }
}
void menu24(void)
{
    if(buttonchick==0)//清除残留图像
  {
   tft.fillRoundRect(0, 28, 240, 280,8,TFT_BLACK);
   tft.pushImage(90, 90, 60, 60, sbpic9); 
   SDFont.DrawText(90, 180, "音乐");    //输出文字
   buttonchick2=buttonchick;
   buttonchick = 1;

  }
}
void menu25(void)
{
    if(buttonchick==0)//清除残留图像
  {
   tft.fillRoundRect(0, 28, 240, 280,8,TFT_BLACK);
   tft.pushImage(90, 90, 60, 60, sbpic2); 
   SDFont.DrawText(90, 180, "小电视");    //输出文字
   buttonchick2=buttonchick;
   buttonchick = 1;
  }
}
void menu26(void)
{
    if(buttonchick==0)//清除残留图像
  {
   tft.fillRoundRect(0, 28, 240, 280,8,TFT_BLACK);
   tft.pushImage(90, 90, 60, 60, sbpic3); 
   SDFont.DrawText(90, 180, "运动健康");    //输出文字
   buttonchick2=buttonchick;
   buttonchick = 1;
  }
}
void menu27(void)
{
    if(buttonchick==0)//清除残留图像
  {
   tft.fillRoundRect(0, 28, 240, 280,8,TFT_BLACK);
   tft.pushImage(90, 90, 60, 60, sbpic10); 
   SDFont.DrawText(90, 180, "闹钟");    //输出文字
   buttonchick2=buttonchick;
   buttonchick = 1;
  }
}
void menu28(void)
{
    if(buttonchick==0)//清除残留图像
  {
   tft.fillRoundRect(0, 28, 240, 280,8,TFT_BLACK);
   tft.pushImage(90, 90, 60, 60, sbpic7); 
   SDFont.DrawText(90, 180, "录音");    //输出文字
   buttonchick2=buttonchick;
   buttonchick = 1;
  }
}
void menu29(void)
{
    if(buttonchick==0)//清除残留图像
  {
   tft.fillRoundRect(0, 28, 240, 280,8,TFT_BLACK);
   tft.pushImage(90, 90, 60, 60, sbpic5); 
   SDFont.DrawText(90, 180, "设置");    //输出文字
   buttonchick2=buttonchick;
   buttonchick = 1;
  }
}
void menu210(void)
{
    if(buttonchick==0)//清除残留图像
  {
   tft.fillRoundRect(0, 28, 240, 280,8,TFT_BLACK);
   tft.pushImage(90, 90, 60, 60, sbpic1); 
   SDFont.DrawText(90, 180, "蓝牙助手");    //输出文字
   buttonchick2=buttonchick;
   buttonchick = 1;
  }
}
void menu31(void){
  
      if(buttonchick==0)//清除残留图像
  {
  filechoose("/index.idx" ,"/jpg",".jpg");//文件选择，主要针对的是文件夹与文件类型
  button.reset();//清除一下按钮状态机的状态
  button1.reset();//清除一下按钮状态机的状态
  button2.reset();//清除一下按钮状态机的状态
  button.attachClick(filedown);
  button1.attachDoubleClick(functionout);
  button1.attachClick(sdjpgshow);
  button2.attachClick(fileup);
  buttonchick2=buttonchick;
  buttonchick = 1;
  }
  
 while(programchick==1)
 {
  progmloop();
    
 }
                
}
void menu32(void){

  tft.fillRoundRect(0, 28, 240, 280,8,TFT_BLACK); 
      if(buttonchick==0)//清除残留图像
  {

  WiFi.disconnect();
  delay(2000);
  WiFi.begin("esp32", "123456789");
  button.reset();//清除一下按钮状态机的状态
  button1.reset();//清除一下按钮状态机的状态
  button2.reset();//清除一下按钮状态机的状态
  button.attachClick(changeduojiadd);
  button1.attachDoubleClick(functionout);
  button1.attachClick(kong);
  button2.attachClick(changeduojisub);
  buttonchick2=buttonchick;
  buttonchick = 1;
    
  }
  
 while(programchick==1)
 {
  if(duoji!=duojichange)
  {
  tft.fillRoundRect(0, 28, 240, 280,8,TFT_BLACK); 
  tft.drawWideLine(30,95,(30+duoji),95,12,TFT_GREEN,TFT_GREEN);
  showNum(String(duoji), 85, 150, TFT_ORANGE);
  duojichange=duoji;
  }
  progmloop();  
 }
 
}




  
void menu33(void){
        tft.fillRoundRect(0, 28, 240, 280,8,TFT_BLACK);  
      showNum("33", 5, 100,TFT_ORANGE);   
  }
void menu34(void){
        tft.fillRoundRect(0, 28, 240, 280,8,TFT_BLACK);  
      showNum("341", 5, 100,TFT_ORANGE);   
  }
void menu35(void){
        tft.fillRoundRect(0, 28, 240, 280,8,TFT_BLACK);  
      showNum("351", 5, 100,TFT_ORANGE);   
  }
void menu36(void){
        tft.fillRoundRect(0, 28, 240, 280,8,TFT_BLACK);  
      showNum("361", 5, 100,TFT_ORANGE);   
  }
void menu37(void){
        tft.fillRoundRect(0, 28, 240, 280,8,TFT_BLACK);  
      showNum("371", 5, 100,TFT_ORANGE);   
  }
void menu38(void){
        if(buttonchick==0)//清除残留图像
  {
        tft.fillRoundRect(0, 28, 240, 280,8,TFT_BLACK);  
        microphone();
        buttonchick = 1;
  }
}
void menu39(void){
 wifistmint(); 
  }
void menu310(void){
        tft.fillRoundRect(0, 28, 240, 280,8,TFT_BLACK);  
      showNum("40", 5, 100,TFT_ORANGE);   
  }
