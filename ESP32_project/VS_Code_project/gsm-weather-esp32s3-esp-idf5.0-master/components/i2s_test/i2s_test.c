#include "i2s_test.h"
#include <math.h>
#include "main.h"
#include "wav.h"


//char *wav_file_name="/sdcard/test.wav";
char *wav_file_name="/spiffs/test.wav";

wavctrl WaveCtrlData;
FILE* Wave_File;

void wav_play_task(void *pvParameter)
{
	uint8_t ret=0;
	uint32_t time=0;
	uint8_t read_buf[1024];
	uint32_t read_len=0;
	size_t i2s_bytes_write = 0;

	Wave_File = fopen(wav_file_name,"r");
	if(Wave_File==NULL)
	{
		i2s_test_DEBUG("打开文件失败\r\n");

		goto exit;
		return 2;
	}else
	{
		i2s_test_DEBUG("打开文件成功\r\n");
	}

	ret = fseek(Wave_File,WaveCtrlData.datastart,SEEK_SET);		//跳过文件头
	if(ret)
	{
		i2s_test_DEBUG("偏移WAV头失败%d\r\n",ret);

		goto exit;
		return 3;
	}else
	{
		i2s_test_DEBUG("偏移WAV头成功%d\r\n",WaveCtrlData.datastart);
	}

	WM8978_HPvol_Set(0,0);	//耳机音量设置
	WM8978_SPKvol_Set(40);	//喇叭音量设置

	WM8978_ADDA_Cfg(1,0);	//开启DAC
	WM8978_Input_Cfg(0,0,0);//关闭输入通道
	WM8978_Output_Cfg(1,0);	//开启DAC输出

	if(WaveCtrlData.bps==16)
		WM8978_I2S_Cfg(2,0);	//飞利浦标准,16位数据长度
	else if(WaveCtrlData.bps==24)
		WM8978_I2S_Cfg(2,2);	//飞利浦标准,24位数据长度
	else if(WaveCtrlData.bps==32)
		WM8978_I2S_Cfg(2,3);	//飞利浦标准,32位数据长度

	i2s_set_clk(I2S_NUM, WaveCtrlData.samplerate, WaveCtrlData.bps, WaveCtrlData.nchannels);
	i2s_test_DEBUG("开始播放\r\n");
	while(1)
	{
		read_len = FillWaveFileBuf(Wave_File,read_buf,1024,WaveCtrlData.nchannels,WaveCtrlData.bps);

		//i2s_test_DEBUG("读取字节数:%d\r\n",read_len);
		i2s_write(I2S_NUM, read_buf, read_len, &i2s_bytes_write, 100);

		//i2s_test_DEBUG("写入字节数:%d\r\n",i2s_bytes_write);
		if(read_len<1024)
		{
			fclose(Wave_File);
			i2s_test_DEBUG("读取结束关闭文件\r\n");
			break;
		}
		if(++time>200)
		{
			time=0;
			wave_get_curtime(Wave_File,&WaveCtrlData);
			i2s_test_DEBUG("播放进度:%02d:%02d/%02d:%02d\r\n",WaveCtrlData.cursec/60,WaveCtrlData.cursec%60,WaveCtrlData.totsec/60,WaveCtrlData.totsec%60);
		}
	}
exit:
	vTaskDelete(NULL);
}

void i2s_test(void)
{
	uint8_t ret=0;
	ret =  wave_decode_init(wav_file_name,&WaveCtrlData);
	if(ret)
	{
		i2s_test_DEBUG("WAV初始化失败%d\r\n",ret);
		return 1;
	}
	xTaskCreate(wav_play_task,"wav_play_task",1024*5,NULL,3,NULL);
	return 0;
}
