/*******************************************************
 * 函数名：ShowIP
 * 描述  ：滑动显示当前设备IP
 * 参数  ：无
 * 返回值：无
 **********************************************************/
void ShowIP()
{
    int sa[4], r=0, t=0;
    String oneLine = WiFi.localIP().toString()+".";
    int s1,s2,s3;
    int ipx,x = 8;
    uint16_t ipcolor=hsv2rgb(60, 100, 100);
    for (int i=0; i < oneLine.length(); i++)
    { 
      if(oneLine.charAt(i) == '.') 
      { 
        sa[t] = oneLine.substring(r, i).toInt();
        r=(i+1); 
        t++; 
      }
    }
    for(int j=0;j<85;j++){
      ipx = x;
      for(int i=0; i < 4; i++){
        s1 = sa[i]/100;
        s2 = (sa[i] - s1*100)/10;
        s3 = (sa[i] - s1*100)%10;
        if(sa[i]>99){
          showCharacter(ipx, 2, s1 + 48, Clock3x5, ipcolor);
          ipx += 4;
          showCharacter(ipx, 2, s2 + 48, Clock3x5, ipcolor);
          ipx += 4;
          showCharacter(ipx, 2, s3 + 48, Clock3x5, ipcolor);
        }else if(sa[i]>9){
          showCharacter(ipx, 2, s2 + 48, Clock3x5, ipcolor);
          ipx += 4;
          showCharacter(ipx, 2, s3 + 48, Clock3x5, ipcolor);
        }else{
          showCharacter(ipx, 2, s3 + 48, Clock3x5, ipcolor);
        }
        if(i<3){
          ipx += 4;
          matrix->drawPixel(ipx,6,ipcolor);
          ipx += 2;
        }
      }
      if(--x < -90) {
        x = matrix->width();
      }
      matrix->show();
      system_soft_wdt_feed();
      delay(80);
      matrix->clear();
    }
    matrix->clear();
}

/*******************************************************
 * 函数名：hsv2rgb
 * 描述  ：颜色转换方法
 * 参数  ：h s v
 * 返回值：rgb
 **********************************************************/
uint16_t hsv2rgb(uint16_t hue, uint8_t saturation, uint8_t value)
{
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    uint16_t hi = (hue / 60) % 6;
    uint16_t F = 100 * hue / 60 - 100 * hi;
    uint16_t P = value * (100 - saturation) / 100;
    uint16_t Q = value * (10000 - F * saturation) / 10000;
    uint16_t T = value * (10000 - saturation * (100 - F)) / 10000;
    if(hue == 0){
      return matrix->Color(255*value/100, 255*value/100, 255*value/100);
    }
    switch (hi)
    {
    case 0:
        red = value;
        green = T;
        blue = P;
        break;
    case 1:
        red = Q;
        green = value;
        blue = P;
        break;
    case 2:
        red = P;
        green = value;
        blue = T;
        break;
    case 3:
        red = P;
        green = Q;
        blue = value;
        break;
    case 4:
        red = T;
        green = P;
        blue = value;
        break;
    case 5:
        red = value;
        green = P;
        blue = Q;
        break;
    default:
        return matrix->Color(255, 0, 0);
    }
    red = red * 255 / 100;
    green = green * 255 / 100;
    blue = blue * 255 / 100;
    return matrix->Color(red, green, blue);
}

/*******************************************************
 * 函数名：cn_utf8_to_unicode
 * 描述  ：中文字符utf-8编码转unicode编码函数
 * 参数  ：
 *        @cInput     中文的utf-8编码值
 * 返回值：对于中文的unicode编码值
 **********************************************************/
uint16_t cn_utf8_to_unicode(const uint32_t cInput)
{
    // b1 表示UTF-8编码的pInput中的高字节, b2 表示次高字节, ...
    uint8_t b1, b2, b3;
    uint16_t cOutput = 0;
    b1 = (cInput & 0x000F0000) >> 16;
    b2 = (cInput & 0x00003F00) >> 8;
    b3 = (cInput & 0x0000003F);
    cOutput = (b1 << 12) + (b2 << 6) + b3;
    return cOutput;
}

/*******************************************************
 * 函数名：showCharacter
 * 描述  ：显示单个字符
 * 参数  ：
 *        @char_x     字符左上角x坐标
 *        @char_y     字符左上角y坐标
 *        @c          字符编码
 *        @charfont   字体
 *        @showHue    显示的颜色
 * 返回值：无
 **********************************************************/
void showCharacter(int char_x, int char_y, int c, int charfont, int showHue)
{
    int i, x, y, characterSpace;
    unsigned char line;
    unsigned short ct;

    const unsigned char *cb = fontattribute[charfont].font_bitmap;
    const unsigned char *ci = fontattribute[charfont].font_index;
    int len = fontattribute[charfont].font_num;
    int lx = fontattribute[charfont].font_width;
    int ly = fontattribute[charfont].font_height;

    if (charfont == Cnfont8x8 && c > 0xFF) //中文字体中的非英文字体
    {
        ct = cn_utf8_to_unicode(c);
    } 
    else{
        ct = (unsigned short)c;
    }
    for (i = 0; i < len; i++)
    {
        if (charfont == Cnfont8x8) //中文编码为8字节，不方便用放flash，内存充足，因此放内存中。
        {
            if (Cnfont_index[i] == ct)
            {
                characterSpace = i;
                break;
            }
        }
        else
        {
            if (pgm_read_byte(ci + i) == ct) // ci[i] == ct
            {
                characterSpace = i;
                break;
            }
        }
    }
    if (i == len){
        characterSpace = 1; //空白符
    }
    for (y = 0; y < ly; y++)
    {
        line = pgm_read_byte(cb + (ly * characterSpace + y));
        for (x = 0; x < lx; x++)
        {
            if(((line >> (7 - x)) & 0x01) != 0){
                matrix->drawPixel(char_x + x, char_y + y,hsv2rgb(showHue, 100, 100));
            }else{
                matrix->drawPixel(char_x + x, char_y + y,matrix->Color( 0, 0, 0));
            }
        }
    }
}

void drawFastXLineGXY(int16_t x, int16_t y, int16_t h, bool s){
  for(int i=x;i<x+h;i++){
    if(s){
      matrix->drawPixel(i,y,Gradient(i,y));
    }else{
      matrix->drawPixel(i,y,matrix->Color(0,0,0));
    }  
  }
}

void drawFastXLine(int16_t x, int16_t y, int16_t h, int16_t c){
  for(int i=x;i<x+h;i++){
      matrix->drawPixel(i,y,c);
  }
}

void drawFastYLine(int16_t x, int16_t y, int16_t h, int16_t c){
  for(int i=y;i<y+h;i++){
    matrix->drawPixel(x,i,c);
  }
}

//渐变色颜色计算方法
uint16_t Gradient(int x, int y){
  ColorRGB A = myRGBColorPalette[color_index][0];
  ColorRGB B = myRGBColorPalette[color_index][1];
  if(color_index == 0){
    if(Mode == TIME_MODE || (Mode == LOOP_MODE && LoopModeNow == TIME_MODE)){
      if(y<2){
        return hsv2rgb(hueh,100,100);
      }else{
        return hsv2rgb(huem,100,100);
      }
    }else{
      return hsv2rgb(huem,100,100);
    }
  }
  int n = 0;
  if(colorLoop<29){
    if((x+y)*2>colorLoop){
      n = (x+y)*2-colorLoop;
    }else{
      n = colorLoop-(x+y)*2;
    }
  }else{
    if((x+y)*2>(colorLoop-28)){
      n = 28-((x+y)*2-colorLoop+28);
    }else{
      n = 28-(colorLoop-28-(x+y)*2);
    }
  }
  int r = (B.r-A.r)/28*n+A.r;
  int g = (B.g-A.g)/28*n+A.g;
  int b = (B.b-A.b)/28*n+A.b;
  return matrix->Color(r,g,b);
}

void helloWorld(){
    int x = xres;
    for(int i=0;i<71;i++){
        matrix->clear();
        matrix->fillScreen(0);
        matrix->setCursor(x, 0);
        matrix->print(F("Hello World"));
        matrix->setTextColor(matrix->Color(148,0,211));
        if(--x < -71) {
        x = matrix->width();
        }
        matrix->show();
        system_soft_wdt_feed();
        delay(70);
    }
    matrix->clear();
}

void showWiFi(){
    uint16_t cl = matrix->Color( 0, 102, 255);
    matrix->drawPixel(0, 7, cl);
    drawFastXLine(1,1,6,cl);
    drawFastXLine(2,3,3,cl);
    drawFastXLine(1,5,2,cl);
    drawFastYLine(6,2,5,cl);
    drawFastYLine(4,4,2,cl);
    matrix->drawPixel(2, 6, cl);
    matrix->show();
}

//配置同步方法
void configSync(uint8_t num){
  String jsonStr = String("{'mode':")+Mode+String(",'brightNess':")+brightnessNow+
  String(",'hueh':")+hueh+String(",'huem':")+huem+String(",'hues':")+hues+
  String(",'clockMode':")+clockMode+
  String(",'point':")+point+String("}");
  webSocket.sendTXT(num, jsonStr);
}
/**
 * @brief 显示数字
 * 
 * @param number 
 * @param xlength 
 * @param ylength 
 * @param x 
 * @param y 
 */
void showbitnumber(int number, int xlength, int ylength, int x, int y ){
  String numStr = String(number);
  if(number<10){
    showbitmap(bitdata20[(int)(0)],xlength,ylength, x, y);
    showbitmap(bitdata20[(int)((String(numStr.charAt(0)).toInt() + 1) - 1)],xlength,ylength, x+4, y);
  }else if(number<100){
    showbitmap(bitdata20[(int)((String(numStr.charAt(0)).toInt() + 1) - 1)],xlength,ylength, x, y);
    showbitmap(bitdata20[(int)((String(numStr.charAt(1)).toInt() + 1) - 1)],xlength,ylength, x+4, y);
  }
}
