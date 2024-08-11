/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_check.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "app_wifi.h"
#include "settings.h"

/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/


#define EXAMPLE_ESP_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY

#if CONFIG_ESP_WIFI_AUTH_OPEN
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif

/* FreeRTOS事件组，用于信号通知我们何时连接*/
static EventGroupHandle_t s_wifi_event_group;

/* 事件组允许每个事件有多个位，但我们只关心两个事件：
 * - 我们已连接到AP并获得IP
 * - 我们在最大重试次数后仍未能连接 */
#define WIFI_CONNECTED_BIT      BIT0
#define WIFI_FAIL_BIT           BIT1

#define portTICK_RATE_MS_WIFI         10

static const char *TAG = "wifi station";
static int s_retry_num = 0;
static bool s_reconnect = true;

static bool wifi_connected = false;
static QueueHandle_t wifi_event_queue = NULL;

scan_info_t scan_info_result = {
    .scan_done = WIFI_SCAN_IDLE,
    .ap_count = 0,
};

// 检查WiFi连接状态的函数
WiFi_Connect_Status wifi_connected_already(void)
{
    WiFi_Connect_Status status;
    if (true == wifi_connected) {
        status = WIFI_STATUS_CONNECTED_OK;
    } else {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            status = WIFI_STATUS_CONNECTING;
        } else {
            status = WIFI_STATUS_CONNECTED_FAILED;
        }
    }
    return status;
}


// 获取WiFi SSID的函数
esp_err_t app_wifi_get_wifi_ssid(char *ssid, size_t len)
{
    wifi_config_t wifi_cfg; // 定义WiFi配置结构体
    // 获取WiFi配置，如果失败则返回错误
    if (esp_wifi_get_config(WIFI_IF_STA, &wifi_cfg) != ESP_OK) {
        return ESP_FAIL;
    }
    // 将WiFi SSID复制到传入的ssid指针中
    strncpy(ssid, (const char *)wifi_cfg.sta.ssid, len);
    return ESP_OK; // 返回成功
}

// 发送网络事件的函数
esp_err_t send_network_event(net_event_t event)
{
    net_event_t eventOut = event; // 复制传入的事件
    BaseType_t ret_val = xQueueSend(wifi_event_queue, &eventOut, 0); // 将事件发送到队列

    // 如果事件是重新连接，则设置wifi_connected为false
    if (NET_EVENT_RECONNECT == event) {
        wifi_connected = false;
    }

    // 检查事件是否成功发送到队列，如果失败则返回错误
    ESP_RETURN_ON_FALSE(pdPASS == ret_val, ESP_ERR_INVALID_STATE,
                        TAG, "The last event has not been processed yet");
    return ESP_OK; // 返回成功
}


/* 初始化Wi-Fi为sta模式并设置扫描方法 */
static void wifi_scan(void)
{
    uint16_t number = DEFAULT_SCAN_LIST_SIZE; // 设置扫描列表的大小
    wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE]; // 定义AP信息数组
    uint16_t ap_count = 0; // 定义AP数量
    memset(ap_info, 0, sizeof(ap_info)); // 初始化AP信息数组

    app_wifi_state_set(WIFI_SCAN_BUSY); // 设置Wi-Fi扫描状态为忙碌

    esp_err_t ret = esp_wifi_scan_start(NULL, true); // 开始Wi-Fi扫描
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info)); // 获取扫描到的AP记录
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count)); // 获取扫描到的AP数量
    ESP_LOGI(TAG, "Total APs scanned = %u, ret:%d", ap_count, ret); // 打印扫描到的AP总数和返回值

    for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++) {
        ESP_LOGI(TAG, "SSID \t\t%s", ap_info[i].ssid); // 打印AP的SSID
        /*
        ESP_LOGI(TAG, "RSSI \t\t%d", ap_info[i].rssi); // 打印AP的RSSI
        print_auth_mode(ap_info[i].authmode); // 打印AP的认证模式
        if (ap_info[i].authmode != WIFI_AUTH_WEP) {
            print_cipher_type(ap_info[i].pairwise_cipher, ap_info[i].group_cipher); // 打印AP的加密类型
        }
        ESP_LOGI(TAG, "Channel \t\t%d\n", ap_info[i].primary); // 打印AP的信道
        */
    }

    if (ap_count && (ESP_OK == ret)) { // 如果扫描到AP且返回值为ESP_OK
        scan_info_result.ap_count = (ap_count < DEFAULT_SCAN_LIST_SIZE) ? ap_count : DEFAULT_SCAN_LIST_SIZE; // 设置扫描结果中的AP数量
        memcpy(&scan_info_result.ap_info[0], &ap_info[0], sizeof(wifi_ap_record_t)*scan_info_result.ap_count); // 复制AP信息到扫描结果中
    } else {
        vTaskDelay(pdMS_TO_TICKS(1000)); // 延迟1秒
        ESP_LOGI(TAG, "failed return"); // 打印失败返回信息
    }
    app_wifi_state_set(WIFI_SCAN_RENEW); // 设置Wi-Fi扫描状态为更新
}


static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    // 处理WIFI事件
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        // 发送网络启动扫描事件
        send_network_event(NET_EVENT_POWERON_SCAN);
        ESP_LOGI(TAG, "开始连接到AP");
    } 
    // 处理WIFI断开事件
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        // 如果允许重连且重试次数未达到最大值
        if (s_reconnect && ++s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            // 尝试重新连接
            esp_wifi_connect();
            ESP_LOGI(TAG, "sta断开连接，重试次数 %d...", s_retry_num);
        } else {
            ESP_LOGI(TAG, "sta已断开连接");
        }
        // 清除连接成功标志位
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        // 设置连接失败标志位
        xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        // 设置WIFI连接状态为false
        wifi_connected = false;
    } 
    // 处理获取IP事件
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        // 获取IP事件数据
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        ESP_LOGI(TAG, "已获取IP:" IPSTR, IP2STR(&event->ip_info.ip));
        // 重置重试次数
        s_retry_num = 0;
        // 设置WIFI连接状态为true
        wifi_connected = true;
        // 设置连接成功标志位
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}


static void wifi_reconnect_sta(void)
{
    // 等待事件组中的WIFI_CONNECTED_BIT位，返回事件组中设置的位
    int bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, 0, 1, 0);

    // 初始化wifi配置结构体
    wifi_config_t wifi_config = { 0 };

    // 获取系统参数
    sys_param_t *sys_param = settings_get_parameter();
    // 将系统参数中的SSID复制到wifi配置结构体中
    memcpy(wifi_config.sta.ssid, sys_param->ssid, sizeof(wifi_config.sta.ssid));
    // 将系统参数中的密码复制到wifi配置结构体中
    memcpy(wifi_config.sta.password, sys_param->password, sizeof(wifi_config.sta.password));
    //ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );

    // 如果已经连接到WIFI，则断开连接
    if (bits & WIFI_CONNECTED_BIT) {
        s_reconnect = false;
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        ESP_ERROR_CHECK( esp_wifi_disconnect() );
        xEventGroupWaitBits(s_wifi_event_group, WIFI_FAIL_BIT, 0, 1, portTICK_RATE_MS_WIFI);
    }

    // 设置重新连接标志和重试次数
    s_reconnect = true;
    s_retry_num = 0;
    // 设置WIFI模式为STA（站模式）
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    // 设置WIFI配置
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    // 连接到WIFI
    esp_wifi_connect();

    // 打印日志，显示连接的SSID和密码
    ESP_LOGI(TAG, "wifi_reconnect_sta finished.%s, %s", \
            wifi_config.sta.ssid, wifi_config.sta.password);

    // 等待WIFI连接成功的事件
    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, 0, 1, 5000 / portTICK_RATE_MS_WIFI);
}


static void wifi_init_sta(void)
{
    // 创建一个事件组
    s_wifi_event_group = xEventGroupCreate();

    // 初始化网络接口
    ESP_ERROR_CHECK(esp_netif_init());
    // 创建默认的事件循环
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    // 创建默认的WiFi站接口
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    // 使用默认配置初始化WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 注册事件处理函数
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                    ESP_EVENT_ANY_ID,
                    &event_handler,
                    NULL,
                    &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                    IP_EVENT_STA_GOT_IP,
                    &event_handler,
                    NULL,
                    &instance_got_ip));

    // 配置WiFi连接的SSID和密码
    wifi_config_t wifi_config = { 
        .sta = {
            .ssid = {0}, 
            .password = {0},
        },
    };
    sys_param_t *sys_param = settings_get_parameter();
    memcpy(wifi_config.sta.ssid, sys_param->ssid, sizeof(wifi_config.sta.ssid));
    memcpy(wifi_config.sta.password, sys_param->password, sizeof(wifi_config.sta.password));

    // 设置WiFi模式为站模式
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    // 设置WiFi配置
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    // 启动WiFi
    ESP_ERROR_CHECK(esp_wifi_start() );

    // 打印WiFi初始化完成的信息
    ESP_LOGI(TAG, "wifi_init_sta finished.%s, %s", \
             wifi_config.sta.ssid, wifi_config.sta.password);
}


static void network_task(void *args)
{
    net_event_t net_event; // 定义网络事件变量

    wifi_init_sta(); // 初始化WiFi为STA模式

    while (1) {
        // 从WiFi事件队列中接收事件
        if (pdPASS == xQueueReceive(wifi_event_queue, &net_event, portTICK_RATE_MS_WIFI / 5)) {
            switch (net_event) {
            case NET_EVENT_RECONNECT:
                ESP_LOGI(TAG, "NET_EVENT_RECONNECT"); // 打印重连事件日志
                wifi_reconnect_sta(); // 执行WiFi重连
                break;
            case NET_EVENT_SCAN:
                ESP_LOGI(TAG, "NET_EVENT_SCAN"); // 打印扫描事件日志
                wifi_scan(); // 执行WiFi扫描
                break;
            case NET_EVENT_NTP:
                ESP_LOGI(TAG, "NET_EVENT_NTP"); // 打印NTP事件日志
                break;
            case NET_EVENT_WEATHER:
                ESP_LOGI(TAG, "NET_EVENT_WEATHER"); // 打印天气事件日志
                break;

            case NET_EVENT_POWERON_SCAN:
                ESP_LOGI(TAG, "NET_EVENT_POWERON_SCAN"); // 打印开机扫描事件日志
                wifi_scan(); // 执行WiFi扫描
                esp_wifi_connect(); // 连接WiFi
                wifi_connected = false; // 设置WiFi连接状态为未连接
                break;
            default:
                break;
            }
        }
    }
    vTaskDelete(NULL); // 删除任务
}

// 锁定WiFi功能的函数，带超时参数
bool app_wifi_lock(uint32_t timeout_ms)
{
    // 确保WiFi互斥锁已初始化，否则提示需要先调用bsp_display_start函数
    assert(scan_info_result.wifi_mux && "bsp_display_start must be called first");

    // 根据超时时间计算Tick值，如果timeout_ms为0则使用最大延迟
    const TickType_t timeout_ticks = (timeout_ms == 0) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    // 尝试获取互斥锁，返回是否成功
    return xSemaphoreTakeRecursive(scan_info_result.wifi_mux, timeout_ticks) == pdTRUE;
}

// 解锁WiFi功能的函数
void app_wifi_unlock(void)
{
    // 确保WiFi互斥锁已初始化，否则提示需要先调用bsp_display_start函数
    assert(scan_info_result.wifi_mux && "bsp_display_start must be called first");
    // 释放互斥锁
    xSemaphoreGiveRecursive(scan_info_result.wifi_mux);
}

// 设置WiFi扫描状态的函数
void app_wifi_state_set(wifi_scan_status_t status)
{
    // 锁定WiFi功能
    app_wifi_lock(0);
    // 设置扫描状态
    scan_info_result.scan_done = status;
    // 解锁WiFi功能
    app_wifi_unlock();
}

// 启动网络功能的函数
void app_network_start(void)
{
    BaseType_t ret_val;
    // 创建WiFi互斥锁
    scan_info_result.wifi_mux = xSemaphoreCreateRecursiveMutex();
    // 检查互斥锁是否创建成功
    ESP_ERROR_CHECK_WITHOUT_ABORT((scan_info_result.wifi_mux) ? ESP_OK : ESP_FAIL);
    // 创建网络事件队列
    wifi_event_queue = xQueueCreate(4, sizeof(net_event_t));
    // 检查事件队列是否创建成功
    ESP_ERROR_CHECK_WITHOUT_ABORT((wifi_event_queue) ? ESP_OK : ESP_FAIL);
    // 创建网络任务并绑定到核心0
    ret_val = xTaskCreatePinnedToCore(network_task, "NetWork Task", 5 * 1024, NULL, 1, NULL, 0);
    // 检查任务是否创建成功
    ESP_ERROR_CHECK_WITHOUT_ABORT((pdPASS == ret_val) ? ESP_OK : ESP_FAIL);
}
