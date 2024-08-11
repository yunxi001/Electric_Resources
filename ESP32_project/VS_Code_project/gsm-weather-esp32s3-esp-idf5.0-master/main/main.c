
#include "main.h"
#include "lvgl_project.h"

/******************************************************************************************************
GSM_Wheather_S3 ESP32S3代码仓库，使用ESP32官方esp-idf-v5.0开发，需要安装相关开发环境。
视频演示地址：https://www.bilibili.com/video/BV1VU4y1v7VD?vd_source=2847f5d974d73d579ce40eb1213b09ff
字库UI设计开源地址:https://gitee.com/gsm-wheather-project/gsm-weather-s3-comprehensive-information
硬件开源地址：https://oshwhub.com/yeshengchengxuyuan/b4b67ab1e8234aeebea054b4eda6f549
软件开源地址：https://gitee.com/gsm-wheather-project

食用过程中有疑问可以通过下面联系方式与我联系:
微信:GM8988
QQ:1063503277
QQ复刻群:746724179
********************************************************************************************************/


static void init_task_handler(void *pvParameters)
{
	main_debug("-------------gsm-weather-s3----------------\r\n");
	main_debug("编译时间:%s %s\r\n", __DATE__, __TIME__);
	main_debug("esp32 sdk version :%s\r\n", esp_get_idf_version());

	bsp_key_init();

	bsp_power_init();
    system_data.spiffs_sta = my_spiffs_init();
	system_data.sd_sta = bsp_sd_init();
	system_data.nvs_sta = nvs_init();
	system_data.wm8978_sta = WM8978_Init();
	bsp_ledc_init();
	system_init();

	//font_init();//使用SD卡中字库，由于太卡暂时放弃

	bsp_i2s_init();
	i2s_test();


	i2c2_init();
	cst816t_init();

    lcd_init(SPI3_HOST, 80000000);

    lvgl_init();
	lv_port_fs_init();
	lvgl_style_init();
    lvgl_desktop_create(lv_scr_act());


	system_set_backlight(5);
	bsp_power_on();

	wifi_init();

	//lv_demo_stress();
	//lv_demo_printer();
	//lv_demo_music();
	//lv_demo_widgets();

	main_debug("初始化结束\r\n");


	vTaskDelete(NULL);

}
void app_main()
{

	xTaskCreate(init_task_handler,
				"init_task_handler",
				1024 * 10,
				NULL,
				configMAX_PRIORITIES-1,
				NULL);
}

void ptintf_memory(char *file,int len)
{
    int free_size;

    ESP_LOGW("内存监控", "%s:%d",file,len);
    free_size = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    printf("内部RAM剩余%d.%dKB  ", free_size/1024,free_size%1024);
    free_size = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    printf("外部PSRAM剩余%d.%dkB\r\n",free_size/1024,free_size%1024);
}
