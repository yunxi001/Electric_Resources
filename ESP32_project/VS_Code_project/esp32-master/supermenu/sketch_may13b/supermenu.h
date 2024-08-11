#ifndef SUPERMENU_H
#define SUPERMENU_H

//切换函数定义
#include <Arduino.h>
#include "OneButton.h"




extern OneButton button;
extern OneButton button1;
extern OneButton button2;
void buttoninit1();
void buttonSET(void);





//定义按键结构体
typedef struct
{
  unsigned char index;//索引号
  unsigned char up;//
  unsigned char down;
  unsigned char left;
  unsigned char right;
  void (*operation)(void);//操作
} KEY_TABLE;





extern KEY_TABLE table[22];
extern void (*current)(void);
extern void   (*loopCall)();
void buttonright();
void buttonleft();
void buttondown();
void buttonup();
void buttonchange();

//菜单函数定义
void button11(void);
void button12(void);
void button21(void);
void button22(void);
void button23(void);




void filechoose(char indexname[128],char dirname[128],char filekind[128]);//文件选择，主要针对的是文件夹与文件类型
void fileup();//选择上一个文件（读取index函数中的文件名称）
void filedown();//选择下一个文件（读取index函数中的文件名称）
void functionout();//功能退出 回到上一界面
void buttonSET(void);//按键初始化


//拓展进入功能
void sdjpgshow();//现实sd卡中的图片
void watchsleep();//手表睡眠息屏模式


//菜单函数
void menu11(void);
void menu12(void);
void menu21(void);
void menu22(void);
void menu23(void);
void menu24(void);
void menu25(void);
void menu26(void);
void menu27(void);
void menu28(void);
void menu29(void);
void menu210(void);
void menu31(void);
void menu32(void);
void menu33(void);
void menu34(void);
void menu35(void);
void menu36(void);
void menu37(void);
void menu38(void);
void menu39(void);
void menu310(void);

#endif
