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

/*
   通过项目配置菜单设置的WiFi配置示例
   
   如果不想使用，只需将下面的条目更改为所需的配置 - 例如 #define EXAMPLE_WIFI_SSID "mywifissid"
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

/* 用于指示连接和重连状态的FreeRTOS事件组 */
static EventGroupHandle_t s_wifi_event_group;

/* 事件组允许每个事件使用多个位，但我们只关心两个事件：
 * - 我们已连接到具有IP的AP
 * - 我们在最大重试次数后仍然无法连接 */
#define WIFI_CONNECTED_BIT      BIT0
#define WIFI_FAIL_BIT           BIT1

#define portTICK_RATE_MS        10

static const char *TAG = "wifi station";
static int s_retry_num = 0;
static bool s_reconnect = true;

static bool wifi_connected = false;
static QueueHandle_t wifi_event_queue = NULL;

scan_info_t scan_info_result = {
    .scan_done = WIFI_SCAN_IDLE,
    .ap_count = 0,
};
WiFi_Connect_Status wifi_connected_already(void)
{
    // 定义WiFi连接状态
    WiFi_Connect_Status status;
    // 如果wifi_connected为真
    if (true == wifi_connected) {
        // 设置状态为已连接
        status = WIFI_STATUS_CONNECTED_OK;
    // 否则，如果重试次数小于最大重试次数
    } else if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
        // 设置状态为正在连接
        status = WIFI_STATUS_CONNECTING;
    // 否则，设置状态为连接失败
    } else {
        status = WIFI_STATUS_CONNECTED_FAILED;
    }
    // 返回状态
    return status;
}

// 功能：从软件栈中获取STA模式的SSID
// 参数：ssid：存储获取到的SSID；len：SSID的最大长度
esp_err_t app_wifi_get_wifi_ssid(char *ssid, size_t len)
{
    // 定义wifi配置结构体
    wifi_config_t wifi_cfg;
    // 获取STA模式的配置
    if (esp_wifi_get_config(WIFI_IF_STA, &wifi_cfg) != ESP_OK) {
        return ESP_FAIL;
    }
    // 将获取到的SSID复制到ssid中
    strncpy(ssid, (const char *)wifi_cfg.sta.ssid, len);
    return ESP_OK;
}

// 功能：发送网络事件到队列
// 参数：event：网络事件
esp_err_t send_network_event(net_event_t event)
{
    // 定义网络事件
    net_event_t eventOut = event;
    // 将网络事件发送到队列
    BaseType_t ret_val = xQueueSend(wifi_event_queue, &eventOut, 0);

    // 如果事件为重新连接，则将wifi连接标志置为false
    if (NET_EVENT_RECONNECT == event) {
        wifi_connected = false;
    }

    // 判断队列是否发送成功
    ESP_RETURN_ON_FALSE(pdPASS == ret_val, ESP_ERR_INVALID_STATE,
                        TAG, "The last event has not been processed yet");
    return ESP_OK;
}
/* 
   将Wi-Fi初始化为sta并设置扫描方法
*/
static void wifi_scan(void)
{
    uint16_t number = DEFAULT_SCAN_LIST_SIZE; // 默认扫描列表大小
    wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE]; // Wi-Fi接入点信息数组
    uint16_t ap_count = 0; // 接入点数量
    memset(ap_info, 0, sizeof(ap_info)); // 将ap_info数组清零

    app_wifi_state_set(WIFI_SCAN_BUSY); // 设置Wi-Fi状态为忙碌

    esp_err_t ret = esp_wifi_scan_start(NULL, true); // 启动Wi-Fi扫描
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info)); // 获取扫描到的接入点记录
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count)); // 获取扫描到的接入点数量
    ESP_LOGI(TAG, "Total APs scanned = %u, ret:%d", ap_count, ret); // 记录扫描到的接入点数量和返回值

    for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++) {
        ESP_LOGI(TAG, "SSID \t\t%s", ap_info[i].ssid); // 记录接入点的SSID
        /*
        ESP_LOGI(TAG, "RSSI \t\t%d", ap_info[i].rssi);
        print_auth_mode(ap_info[i].authmode);
        if (ap_info[i].authmode != WIFI_AUTH_WEP) {
            print_cipher_type(ap_info[i].pairwise_cipher, ap_info[i].group_cipher);
        }
        ESP_LOGI(TAG, "Channel \t\t%d\n", ap_info[i].primary);
        */
    }

    if (ap_count && (ESP_OK == ret)) {
        scan_info_result.ap_count = (ap_count < DEFAULT_SCAN_LIST_SIZE) ? ap_count : DEFAULT_SCAN_LIST_SIZE; // 记录实际接入点数量
        memcpy(&scan_info_result.ap_info[0], &ap_info[0], sizeof(wifi_ap_record_t)*scan_info_result.ap_count); // 复制接入点信息
    } else {
        vTaskDelay(pdMS_TO_TICKS(1000)); // 延迟1秒
        ESP_LOGI(TAG, "failed return"); // 输出失败信息
    }
    app_wifi_state_set(WIFI_SCAN_RENEW); // 设置Wi-Fi状态为更新
}

// 定义事件处理函数event_handler，用于处理WiFi和IP事件
static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    // 如果事件基点是WiFi，事件ID是STA_START（客户端开始连接到AP）
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        // 发送网络事件，启动扫描
        send_network_event(NET_EVENT_POWERON_SCAN);
        ESP_LOGI(TAG, "start connect to the AP");
    // 如果事件基点是WiFi，事件ID是STA_DISCONNECTED（客户端断开连接）
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        // 如果重连模式，重试次数加1
        if (s_reconnect && ++s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            // 重新连接
            esp_wifi_connect();
            ESP_LOGI(TAG, "sta disconnect, retry attempt %d...", s_retry_num);
        // 否则，打印断开连接信息
        } else {
            ESP_LOGI(TAG, "sta disconnected");
        }
        // 清除WIFI连接状态
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        wifi_connected = false;
    // 如果事件基点是IP，事件ID是STA_GOT_IP（客户端获取到IP地址）
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        // 获取到IP地址的信息
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        // 打印获取到IP地址的信息
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        // 重试次数清零
        s_retry_num = 0;
        // 设置WiFi连接状态
        wifi_connected = true;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}
// 重新连接到存储在NVS分区中的WiFi接入点设置。
static void wifi_reconnect_sta(void)
{
    // 等待ESP32连接到WiFi接入点。
    int bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, 0, 1, 0);

    // 创建WiFi配置结构。
    wifi_config_t wifi_config = { 0 };

    // 从NVRAM分区获取WiFi设置。
    sys_param_t *sys_param = settings_get_parameter();
    memcpy(wifi_config.sta.ssid, sys_param->ssid, sizeof(wifi_config.sta.ssid));
    memcpy(wifi_config.sta.password, sys_param->password, sizeof(wifi_config.sta.password));
    //ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );

    // 如果ESP32连接到WiFi接入点，则断开连接并在事件组中清除连接位。
    if (bits & WIFI_CONNECTED_BIT) {
        s_reconnect = false;
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        ESP_ERROR_CHECK( esp_wifi_disconnect() );
        xEventGroupWaitBits(s_wifi_event_group, WIFI_FAIL_BIT, 0, 1, portTICK_RATE_MS);
    }

    // 将重新连接标志设置为true。
    s_reconnect = true;
    s_retry_num = 0;
    // 将WiFi模式设置为station。
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    // 设置WiFi配置。
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    // 连接到WiFi接入点。
    esp_wifi_connect();

    // 记录重新连接状态。
    ESP_LOGI(TAG, "wifi_reconnect_sta finished.%s, %s", \
            wifi_config.sta.ssid, wifi_config.sta.password);

    // 等待ESP32连接到WiFi接入点。
    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, 0, 1, 5000 / portTICK_RATE_MS);
}

// 静态函数 wifi_init_sta，用于初始化 station
static void wifi_init_sta(void)
{
    // 创建一个事件组
    s_wifi_event_group = xEventGroupCreate();

    // 初始化网络接口
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    // 初始化 wifi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 注册 wifi 事件处理函数
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

    // 从系统参数中获取 wifi 配置信息
    wifi_config_t wifi_config = { 
        .sta = {
            .ssid = {0}, 
            .password = {0},
        },
    };
    sys_param_t *sys_param = settings_get_parameter();
    memcpy(wifi_config.sta.ssid, sys_param->ssid, sizeof(wifi_config.sta.ssid));
    memcpy(wifi_config.sta.password, sys_param->password, sizeof(wifi_config.sta.password));
    // 设置为station模式
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    // 启动 wifi
    ESP_ERROR_CHECK(esp_wifi_start() );
    // 打印 wifi 初始化完成的信息
    ESP_LOGI(TAG, "wifi_init_sta finished.%s, %s", \
             wifi_config.sta.ssid, wifi_config.sta.password);
}
// 用于处理网络事件并管理WiFi连接的任务
static void network_task(void *args)
{
    // 用于保存从xQueue接收到的事件的类型
    net_event_t net_event;

    // 初始化为站点模式的WiFi
    wifi_init_sta();

    // 无限循环
    while (1) {
        // 从xQueue接收事件
        if (pdPASS == xQueueReceive(wifi_event_queue, &net_event, portTICK_RATE_MS / 5)) {
            // 处理接收到的事件
            switch (net_event) {
            case NET_EVENT_RECONNECT:
                // 请求重新连接
                ESP_LOGI(TAG, "NET_EVENT_RECONNECT");
                wifi_reconnect_sta();
                break;
            case NET_EVENT_SCAN:
                // 请求WiFi扫描
                ESP_LOGI(TAG, "NET_EVENT_SCAN");
                wifi_scan();
                break;
            case NET_EVENT_NTP:
                // 请求NTP更新
                ESP_LOGI(TAG, "NET_EVENT_NTP");
                break;
            case NET_EVENT_WEATHER:
                // 请求天气更新
                ESP_LOGI(TAG, "NET_EVENT_WEATHER");
                break;

            case NET_EVENT_POWERON_SCAN:
                // 上电后执行WiFi扫描
                ESP_LOGI(TAG, "NET_EVENT_POWERON_SCAN");
                wifi_scan();
                esp_wifi_connect();
                wifi_connected = false;
                break;
            default:
                // 未处理的事件
                break;
            }
        }
    }
    // 删除任务
    vTaskDelete(NULL);
}
// 在超时时间内尝试获取 Wi-Fi 互斥锁
bool app_wifi_lock(uint32_t timeout_ms)
{
    assert(scan_info_result.wifi_mux && "bsp_display_start must be called first"); // 断言确保在调用该函数前已经调用了 bsp_display_start

    const TickType_t timeout_ticks = (timeout_ms == 0) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return xSemaphoreTakeRecursive(scan_info_result.wifi_mux, timeout_ticks) == pdTRUE; // 尝试获取递归互斥锁
}

// 释放 Wi-Fi 互斥锁
void app_wifi_unlock(void)
{
    assert(scan_info_result.wifi_mux && "bsp_display_start must be called first"); // 断言确保在调用该函数前已经调用了 bsp_display_start
    xSemaphoreGiveRecursive(scan_info_result.wifi_mux); // 释放递归互斥锁
}

// 设置 Wi-Fi 状态
void app_wifi_state_set(wifi_scan_status_t status)
{
    app_wifi_lock(0); // 获取互斥锁
    scan_info_result.scan_done = status;  // 设置 Wi-Fi 扫描状态
    app_wifi_unlock(); // 释放互斥锁
}

// 启动网络
void app_network_start(void)
{
    BaseType_t ret_val;

    scan_info_result.wifi_mux = xSemaphoreCreateRecursiveMutex(); // 创建递归互斥锁
    ESP_ERROR_CHECK_WITHOUT_ABORT((scan_info_result.wifi_mux) ? ESP_OK : ESP_FAIL); // 检查互斥锁是否成功创建

    wifi_event_queue = xQueueCreate(4, sizeof(net_event_t)); // 创建 Wi-Fi 事件队列
    ESP_ERROR_CHECK_WITHOUT_ABORT((wifi_event_queue) ? ESP_OK : ESP_FAIL); // 检查队列是否成功创建

    ret_val = xTaskCreatePinnedToCore(network_task, "NetWork Task", 5 * 1024, NULL, 1, NULL, 0); // 创建网络任务
    ESP_ERROR_CHECK_WITHOUT_ABORT((pdPASS == ret_val) ? ESP_OK : ESP_FAIL); // 检查任务是否成功创建
}