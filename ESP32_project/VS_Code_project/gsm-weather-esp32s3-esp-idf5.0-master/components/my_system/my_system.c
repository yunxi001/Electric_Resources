#include "my_system.h"


_system_data system_data;

static void system_time_task(void *pvParameters)
{

	int time=60*10-10;
    TickType_t tick=0;

    tick = xTaskGetTickCount();
	esp_task_wdt_add(NULL);
    while(1)
    {
		esp_task_wdt_reset();

		system_data.RiQi_Data.miao += 1;
		if (system_data.RiQi_Data.miao >= 60)
		{
			system_data.RiQi_Data.miao = 0;

			system_data.RiQi_Data.fen += 1;
			if (system_data.RiQi_Data.fen >= 60)
			{
				system_data.RiQi_Data.fen = 0;
				system_data.RiQi_Data.shi += 1;
				if (system_data.RiQi_Data.shi >= 12)
					system_data.RiQi_Data.shi = 0;
			}
		}
		if(system_data.RiQi_Data.shi==24)
		{
			system_data.RiQi_Data.shi = 0;
		}
		clock_Data.Miao = system_data.RiQi_Data.miao*10;
		clock_Data.Fen = system_data.RiQi_Data.fen;
		clock_Data.Shi = system_data.RiQi_Data.shi;

		if(system_data.RiQi_Data.shi == 0)
		{
			clock_Data.Shi = 12;
		}else
		if(system_data.RiQi_Data.shi>12)
		{
			clock_Data.Shi = system_data.RiQi_Data.shi-12;
		}


		if(++time>60*5*1)
		{
			time=0;
			if(system_data.WIFI_Sta == wifi_qingqiuduankai || system_data.WIFI_Sta == wifi_lianjiehzong)
			{

			}else
			{
				system_data.HuoQu_ShiJian_Flag=true;//同步一次时间
				xSemaphoreGive(system_data.https_request_Semaphore);//释放信号量
			}
		}

		vTaskDelayUntil(&tick, pdMS_TO_TICKS(1000));
    }
	vTaskDelete(NULL);
	esp_task_wdt_delete(NULL);
}

int system_init(void)
{
	system_set_Language(JianTiZhongWen);

	system_data.https_request_Semaphore = xSemaphoreCreateCounting(10,0);//动态创建一个计数型信号量


	system_data.RiQi_Data.nian=2021;
	system_data.RiQi_Data.yue=1;
	system_data.RiQi_Data.ri=14;
	sprintf(system_data.bilibili_Data.id,"%s",HTTPS_BILIBILI_ID);
	system_set_wifi_kaiguan(true);

	system_wifi_data = wifi_list_create("NULL","NULL","NULL");
	if(system_wifi_data)
	{
		system_debug("wifi链表创建成功");
	}
	system_read_config();

	xTaskCreate(system_time_task, "system_time_task", 1024*2, NULL, 2, NULL);

    return 0;
}
int system_set_Language(int dat)
{
	system_data.Language = dat;
	system_data.uart_rx_flag=0;
	system_debug("设置语言:%d",system_data.Language);
	return 0;
}

int system_get_Language(void)
{
	return system_data.Language;
}

int system_set_wifi_kaiguan(int dat)
{
	int dat1 = system_data.wifi_kaiguan;
	system_data.wifi_kaiguan = dat;
	return dat1;
}

int system_get_wifi_kaiguan(void)
{
	return system_data.wifi_kaiguan;
}

void system_power_off(void)
{
	//bsp_power_off();
}

int system_set_backlight(int dat)
{
	system_data.backlight = dat;
	bsp_ledc_set_duty(system_data.backlight);
	return system_data.backlight;
}

int system_get_backlight(void)
{
	return system_data.backlight;
}

uint32_t system_get_file_size(char * path)
{
	uint32_t fileSize=0;
	FILE *pread = fopen(path,"rb");
	if (pread==NULL)
	{
		system_debug("文件打开失败");
		return;
	}

	fseek(pread, 0, SEEK_END);
	fileSize = ftell(pread);
	fseek(pread,0,SEEK_SET);
	fclose(pread);
	system_debug("文件:%s 大小:%d",path,fileSize);
	return fileSize;
}
