#include "chat_api.h"
#include "esp_log.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "cJSON.h"
#include "settings.h"

static char *TAG = "chat_api";

//const char *url = "https://api.chatanywhere.tech";
//const char *apiKey = "sk-UMnNg19PCayc8JRdqp4O48OeYeFE9HuEjRZDETC1ldvfhhO1"; // 替换为您的OpenAI API密钥

static char response_data[4096 * 2];

// 聊天历史记录队列
#define MAX_CHAT_HISTORY 20
static char *chat_history[MAX_CHAT_HISTORY];
static int chat_history_length = 0;
/*百度apitoken
{
    "refresh_token": "25.f87a4dec5d71c262d35b1acdeadc6a5d.315360000.2031885360.282335-72056114",
    "expires_in": 2592000,
    "session_key": "9mzdCS/ZzJysYsOMUCIeP93hpXk6M7nnvA/jbtzcBh476yKWmD7zFmGiMQgH4ryMGFp1IW/bKd+YED+8WQowITMjybbCMw==",
    "access_token": "24.291503f642c33eb94bc671683e4c6e45.2592000.1719117360.282335-72056114",
    "scope": "public brain_all_scope wenxinworkshop_mgr ai_custom_yiyan_com ai_custom_yiyan_com_eb_instant ai_custom_yiyan_com_bloomz7b1 ai_custom_yiyan_com_emb_text ai_custom_yiyan_com_llama2_7b ai_custom_yiyan_com_llama2_13b ai_custom_yiyan_com_llama2_70b ai_custom_yiyan_com_chatglm2_6b_32k ai_custom_yiyan_com_aquilachat_7b ai_custom_yiyan_com_emb_bge_large_zh ai_custom_yiyan_com_emb_bge_large_en ai_custom_yiyan_com_qianfan_chinese_llama_2_7b ai_custom_qianfan_bloomz_7b_compressed ai_custom_yiyan_com_eb_pro ai_custom_yiyan_com_adv_pro ai_custom_yiyan_com_sd_xl ai_custom_yiyan_com_tokenizer_eb ai_custom_yiyan_com_ai_apaas ai_custom_yiyan_com_qf_chinese_llama_2_13b ai_custom_yiyan_com_sqlcoder_7b ai_custom_yiyan_com_codellama_7b_ins ai_custom_yiyan_com_xuanyuan_70b_chat ai_custom_yiyan_com_yi_34b ai_custom_yiyan_com_chatlaw ai_custom_yiyan_com_emb_tao_8k ai_custom_yiyan_com_128k ai_custom_yiyan_com_eb_turbo_pro ai_custom_yiyan_com_mixtral_8x7b ai_custom_yiyan_com_prmtv ai_custom_yiyan_com_eb_pro_prmtv ai_custom_yiyan_com_eb_turbo_pro_128k ai_custom_yiyan_com_ernie_35_4k_0205 ai_custom_yiyan_com_ernie_35_8k_0205 ai_custom_yiyan_com_ernie_35_8k_1222 ai_custom_yiyan_com_ernie_lite_8k ai_custom_yiyan_com_gemma_7b_it ai_custom_yiyan_com_bce_reranker_base ai_custom_yiyan_com_fuyu_8b ai_custom_yiyan_com_ernie_tiny_8k ai_custom_yiyan_com_ernie_char_8k ai_custom_yiyan_com_ernie_35_8k_preview ai_custom_yiyan_com_ernie_40_8k_preview ai_custom_yiyan_com_ernie_func_8k ai_custom_yiyan_com_llama3_8b ai_custom_yiyan_com_llama3_70b ai_custom_yiyan_com_ernie_40_8k_0104 ai_custom_yiyan_com_ernie_40_8k_0329 ai_custom_yiyan_com_ernie_35_8k_0329 ai_custom_yiyan_com_qf_chinese_llama_2_70b wise_adapt lebo_resource_base lightservice_public hetu_basic lightcms_map_poi kaidian_kaidian ApsMisTest_Test权限 vis-classify_flower lpq_开放 cop_helloScope ApsMis_fangdi_permission smartapp_snsapi_base smartapp_mapp_dev_manage iop_autocar oauth_tp_app smartapp_smart_game_openapi oauth_sessionkey smartapp_swanid_verify smartapp_opensource_openapi smartapp_opensource_recapi fake_face_detect_开放Scope vis-ocr_虚拟人物助理 idl-video_虚拟人物助理 smartapp_component smartapp_search_plugin avatar_video_test b2b_tp_openapi b2b_tp_openapi_online smartapp_gov_aladin_to_xcx",
    "session_secret": "ef638a7cbd5d094606a30399c467c5d9"
}
*/
/**
 * @brief HTTP客户端的事件处理回调函数
 *
 * 此函数用于处理HTTP客户端在不同事件发生时的逻辑，例如在连接建立、接收数据、请求完成或断开连接时。
 * 它会根据事件类型来更新或处理相关的数据。
 *
 * @param evt 指向esp_http_client_event_t结构体的指针，包含了事件的详细信息，例如事件类型、数据等。
 * @return esp_err_t 返回ESP_OK表示成功处理了事件，其他错误码表示处理过程中出现了错误。
 */
static esp_err_t http_client_event_handler1(esp_http_client_event_t *evt)
{
    static int recived_len; // 用于记录已接收数据的长度

    switch (evt->event_id) // 根据事件ID执行相应的处理逻辑
    {
    case HTTP_EVENT_ON_CONNECTED: // 连接建立事件
        recived_len = 0; // 重置接收数据长度计数器
        break;
    case HTTP_EVENT_ON_DATA: // 接收到数据事件
        if (evt->user_data)
        {
            memcpy(evt->user_data + recived_len, evt->data, evt->data_len); // 将接收到的数据复制到用户指定的缓冲区
            recived_len += evt->data_len; // 更新已接收数据长度
        }
        break;
    case HTTP_EVENT_ON_FINISH: // 请求完成事件
        recived_len = 0; // 重置接收数据长度计数器
        break;
    case HTTP_EVENT_DISCONNECTED: // 连接断开事件
        recived_len = 0; // 重置接收数据长度计数器
        break;
    case HTTP_EVENT_ERROR: // 错误事件
        recived_len = 0; // 重置接收数据长度计数器
        break;
    default:
        break;
    }

    return ESP_OK; // 表示事件已成功处理
}
/**
 * @brief 获取GPT（Generative Pre-trained Transformer）的回答。
 * 
 * 该函数基于用户提供的提示(prompt)生成一个GPT回答。它首先将新的聊天记录添加到聊天历史记录队列中，
 * 然后构建一个JSON对象，包含聊天历史和用户输入的消息，将这个JSON对象发送给GPT模型进行处理，
 * 并从GPT的响应中提取出回答内容返回。
 * 
 * @param prompt 用户提供的提示，用于请求GPT生成回答。
 * @return char* GPT生成的回答字符串。如果请求失败，则返回一个错误信息字符串。
 */
char *getGPTAnswer(char *prompt)
{
    char *answer = NULL;

    // 管理聊天历史记录，确保不超过最大长度
    if (chat_history_length >= MAX_CHAT_HISTORY)
    {
        free(chat_history[0]);
        for (int i = 0; i < chat_history_length - 1; i++)
        {
            chat_history[i] = chat_history[i + 1];
        }
        chat_history_length--;
    }
    chat_history[chat_history_length] = strdup(prompt);  // 添加新的聊天记录
    chat_history_length++;

    // 构建和填充JSON对象，包括聊天历史和用户输入的消息
    cJSON *root = cJSON_CreateObject();
    cJSON *messages_array = cJSON_CreateArray();

    // 添加聊天历史记录到JSON数组
    for (int i = 0; i < chat_history_length; i++)
    {
        cJSON *message_item = cJSON_CreateObject();
        cJSON_AddStringToObject(message_item, "role", "user");
        cJSON_AddStringToObject(message_item, "content", chat_history[i]);
        cJSON_AddItemToArray(messages_array, message_item);
    }

    // 添加用户输入的消息到JSON数组
    cJSON *user_message = cJSON_CreateObject();
    // 向 JSON 对象中添加一个字符串类型的键值对
    cJSON_AddStringToObject(user_message, "role", "user");
    cJSON_AddStringToObject(user_message, "content", prompt);
    cJSON_AddItemToArray(messages_array, user_message);

    // 设置模型参数和消息数组到JSON根对象
    cJSON_AddStringToObject(root, "model", "gpt-3.5-turbo");
    cJSON_AddItemToObject(root, "messages", messages_array);

    // 设置温度和惩罚参数
    cJSON_AddNumberToObject(root, "temperature", 1);
    cJSON_AddNumberToObject(root, "presence_penalty", 0);
    cJSON_AddNumberToObject(root, "frequency_penalty", 0);

    // 将JSON对象转换为HTTP请求参数
    char *request_params = cJSON_PrintUnformatted(root);
    ESP_LOGE(TAG, "Chat request: %s\n", request_params);
    cJSON_Delete(root);

    // 配置HTTP客户端并发起POST请求
    sys_param_t *sys_param = settings_get_parameter();
    esp_http_client_config_t config = {
        .url = sys_param->url_chat,
        .event_handler = http_client_event_handler1,
        .user_data = response_data,
        .crt_bundle_attach = esp_crt_bundle_attach};
    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Authorization", sys_param->key_chat_api);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, request_params, strlen(request_params));
    esp_http_client_set_timeout_ms(client, 20000);

    // 处理HTTP请求并提取回答内容
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "Chat Response Data: %s", response_data);

        cJSON *json = cJSON_Parse(response_data);
        if (json != NULL)
        {
            cJSON *choices_array = cJSON_GetObjectItem(json, "choices");
            if (choices_array != NULL && cJSON_IsArray(choices_array) && cJSON_GetArraySize(choices_array) > 0)
            {
                cJSON *message_obj = cJSON_GetObjectItem(cJSON_GetArrayItem(choices_array, 0), "message");
                if (message_obj != NULL)
                {
                    answer = strdup(cJSON_GetObjectItem(message_obj, "content")->valuestring);
                }
            }
            cJSON_Delete(json);
        }
    }
    else
    {
        answer = "Chat HTTP Post failed";
    }

    free(request_params);
    esp_http_client_cleanup(client);
    return answer;
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "cJSON.h"

// 日志标签，用于标识日志来源
static const char *TAG = "baidu_ai";

// 替换为实际的客户端ID和客户端密钥
static const char *client_id = "B8X2lL****JV7nI6";
static const char *client_secret = "k1LxbR****Z5h6q2";

// 存储访问令牌的字符数组
static char access_token[256] = {0};

// 处理HTTP事件的回调函数
static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer = NULL;  // 用于存储HTTP请求响应的缓冲区
    static int output_len = 0;       // 存储已读取的字节数
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");  // 记录HTTP事件错误
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");  // 记录HTTP连接成功
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");  // 记录HTTP头部已发送
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);  // 记录HTTP头部信息
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);  // 记录HTTP数据接收
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // 如果响应不是分块的，将其复制到缓冲区
                if (evt->user_data) {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                } else {
                    if (output_buffer == NULL) {
                        output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client));
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");  // 记录内存分配失败
                            return ESP_FAIL;
                        }
                    }
                    memcpy(output_buffer + output_len, evt->data, evt->data_len);
                }
                output_len += evt->data_len;
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");  // 记录HTTP请求完成
            if (output_buffer != NULL) {
                // 完整响应已接收
                ESP_LOGI(TAG, "Output: %s", output_buffer);
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");  // 记录HTTP连接断开
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
    }
    return ESP_OK;
}

// 获取访问令牌的函数
char *getAccessToken()
{
    // 本地响应缓冲区，用于存储HTTP响应
    char local_response_buffer[1024] = {0};
    // HTTP客户端配置
    esp_http_client_config_t config = {
        .url = "https://aip.baidubce.com/oauth/2.0/token",
        .event_handler = http_event_handler,
        .user_data = local_response_buffer,        // 传递本地缓冲区的地址以获取响应
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    // 初始化HTTP客户端
    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return NULL;
    }

    // 准备POST数据
    char post_data[512];
    snprintf(post_data, sizeof(post_data), "grant_type=client_credentials&client_id=%s&client_secret=%s", client_id, client_secret);

    // 设置HTTP客户端的URL、方法和头部
    esp_http_client_set_url(client, "https://aip.baidubce.com/oauth/2.0/token");
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    // 执行HTTP POST请求
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
        
        // 解析JSON响应
        cJSON *json = cJSON_Parse(local_response_buffer);
        if (json == NULL) {
            ESP_LOGE(TAG, "Error parsing JSON response");
            esp_http_client_cleanup(client);
            return NULL;
        }
        cJSON *access_token_json = cJSON_GetObjectItem(json, "access_token");
        if (access_token_json == NULL) {
            ESP_LOGE(TAG, "access_token not found in response");
            cJSON_Delete(json);
            esp_http_client_cleanup(client);
            return NULL;
        }
        strncpy(access_token, access_token_json->valuestring, sizeof(access_token));
        cJSON_Delete(json);
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return NULL;
    }

    // 清理HTTP客户端
    esp_http_client_cleanup(client);
    return access_token;
}
// 函数：baidu_ai_request
// 功能：向百度AI接口发送请求并处理响应
void baidu_ai_request()
{
    // 定义本地响应缓冲区
    char local_response_buffer[4096] = {0};
    // 配置HTTP客户端
    esp_http_client_config_t config = {
        .url = "https://aip.baidubce.com/rpc/2.0/ai_custom/v1/wenxinworkshop/chat/ernie-speed-128k",
        .event_handler = http_event_handler,
        .user_data = local_response_buffer,        
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    // 初始化HTTP客户端
    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return;
    }

    // 准备POST数据
    char post_data[512];
    snprintf(post_data, sizeof(post_data), "{\"access_token\":\"%s\"}", getAccessToken());

    // 设置HTTP客户端的URL、方法、头部和POST字段
    esp_http_client_set_url(client, "https://aip.baidubce.com/rpc/2.0/ai_custom/v1/wenxinworkshop/chat/ernie-speed-128k");
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    // 执行HTTP POST请求并处理响应
    // 执行 HTTP 客户端请求并获取错误码
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        // 如果请求成功，打印 HTTP 状态码和内容长度
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
        // 打印响应内容
        ESP_LOGI(TAG, "Response: %s", local_response_buffer);
    } else {
        // 如果请求失败，打印错误信息
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    // 清理HTTP客户端
    esp_http_client_cleanup(client);
}

void app_main()
{
    ESP_LOGI(TAG, "Starting Baidu AI request example");
    baidu_ai_request();
}


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "cJSON.h"

static const char *TAG = "http_event_handler";

static char *output_buffer = NULL;  // Buffer to store response of HTTP request
static int output_len = 0;          // Stores number of bytes read

static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // If the response is not chunked, copy it into the buffer
                if (evt->user_data) {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                } else {
                    if (output_buffer == NULL) {
                        output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client));
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    memcpy(output_buffer + output_len, evt->data, evt->data_len);
                }
                output_len += evt->data_len;
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                // Complete response received
                ESP_LOGI(TAG, "Output: %s", output_buffer);
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
    }
    return ESP_OK;
}
static const char *TAG = "access_token";
static char access_token[256] = {0};

char *get_access_token(const char *client_id, const char *client_secret)
{
    char local_response_buffer[1024] = {0};
    esp_http_client_config_t config = {
        .url = "https://aip.baidubce.com/oauth/2.0/token",
        .event_handler = http_event_handler,
        .user_data = local_response_buffer,        // Pass address of local buffer to get response
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    char post_data[512];
    snprintf(post_data, sizeof(post_data), "grant_type=client_credentials&client_id=%s&client_secret=%s", client_id, client_secret);

    esp_http_client_set_url(client, "https://aip.baidubce.com/oauth/2.0/token");
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
        
        cJSON *json = cJSON_Parse(local_response_buffer);
        if (json == NULL) {
            ESP_LOGE(TAG, "Error parsing JSON response");
            return NULL;
        }
        cJSON *access_token_json = cJSON_GetObjectItem(json, "access_token");
        if (access_token_json == NULL) {
            ESP_LOGE(TAG, "access_token not found in response");
            return NULL;
        }
        strncpy(access_token, access_token_json->valuestring, sizeof(access_token));
        cJSON_Delete(json);
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
        return NULL;
    }

    esp_http_client_cleanup(client);
    return access_token;
}
static const char *TAG = "baidu_ai_request";

void baidu_ai_request(const char *access_token, const char *user_question)
{
    char local_response_buffer[4096] = {0};
    esp_http_client_config_t config = {
        .url = "https://aip.baidubce.com/rpc/2.0/ai_custom/v1/wenxinworkshop/chat/ernie-speed-128k",
        .event_handler = http_event_handler,
        .user_data = local_response_buffer,        
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    char post_data[512];
    snprintf(post_data, sizeof(post_data), "{\"access_token\":\"%s\",\"user_question\":\"%s\"}", access_token, user_question);

    esp_http_client_set_url(client, "https://aip.baidubce.com/rpc/2.0/ai_custom/v1/wenxinworkshop/chat/ernie-speed-128k");
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
        ESP_LOGI(TAG, "Response: %s", local_response_buffer);
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "cJSON.h"

// Include the modules


static const char *TAG = "app_main";

void app_main()
{
    ESP_LOGI(TAG, "Starting Baidu AI request example");

    // Replace with your actual client ID and client secret
    const char *client_id = "B8X2lL****JV7nI6";
    const char *client_secret = "k1LxbR****Z5h6q2";

    char *token = get_access_token(client_id, client_secret);
    if (token != NULL) {
        ESP_LOGI(TAG, "Access Token: %s", token);
        
        // Example user question
        const char *user_question = "What is the weather like today?";
        baidu_ai_request(token, user_question);
    } else {
        ESP_LOGE(TAG, "Failed to get access token");
    }
}

void baidu_ai_request(const char *access_token, const char *user_question)
{
    // Check if access token is not empty
    if (strlen(access_token) == 0) {
        ESP_LOGE(TAG, "Access token is empty");
        return;
    }

    char local_response_buffer[4096] = {0};
    esp_http_client_config_t config = {
        .url = "https://aip.baidubce.com/rpc/2.0/ai_custom/v1/wenxinworkshop/chat/ernie-speed-128k",
        .event_handler = http_event_handler,
        .user_data = local_response_buffer,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    char post_data[512];
    snprintf(post_data, sizeof(post_data), "{\"access_token\":\"%s\",\"user_question\":\"%s\"}", access_token, user_question);

    esp_http_client_set_url(client, "https://aip.baidubce.com/rpc/2.0/ai_custom/v1/wenxinworkshop/chat/ernie-speed-128k");
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
        ESP_LOGI(TAG, "Response: %s", local_response_buffer);
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
        // Handle the error, e.g., retry the request or notify the user
    }

    esp_http_client_cleanup(client);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "cJSON.h"
#include "settings.h"

static const char *TAG = "chat_api";

// 示例函数：发送HTTP请求并处理响应
void send_http_request(const char *url, const char *apiKey) {
    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .cert_pem = NULL, // 如果需要SSL证书，可以在这里设置
        .timeout_ms = 10000,
        .disable_auto_redirect = false,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return;
    }

    // 设置HTTP头
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_header(client, "Authorization", apiKey);

    // 设置请求体
    const char *request_body = "{\"model\": \"gpt-3.5-turbo\", \"messages\": [{\"role\": \"user\", \"content\": \"Hello!\"}]}";
    esp_http_client_set_post_field(client, request_body, strlen(request_body));

    // 发送请求
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP request sent successfully");
        int status_code = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "Status code: %d", status_code);

        // 读取响应
        char response_buffer[4096];
        int content_length = esp_http_client_get_content_length(client);
        int total_read_len = 0;
        while (total_read_len < content_length) {
            int read_len = esp_http_client_read(client, response_buffer + total_read_len, sizeof(response_buffer) - total_read_len);
            if (read_len <= 0) {
                break;
            }
            total_read_len += read_len;
        }
        response_buffer[total_read_len] = '\0';
        ESP_LOGI(TAG, "Response: %s", response_buffer);

        // 解析JSON响应
        cJSON *root = cJSON_Parse(response_buffer);
        if (root == NULL) {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL) {
                ESP_LOGE(TAG, "Error before: %s", error_ptr);
            }
        } else {
            // 处理JSON数据
            cJSON *choices = cJSON_GetObjectItemCaseSensitive(root, "choices");
            if (cJSON_IsArray(choices)) {
                cJSON *choice = cJSON_GetArrayItem(choices, 0);
                if (choice != NULL) {
                    cJSON *message = cJSON_GetObjectItemCaseSensitive(choice, "message");
                    if (message != NULL) {
                        cJSON *content = cJSON_GetObjectItemCaseSensitive(message, "content");
                        if (cJSON_IsString(content) && (content->valuestring != NULL)) {
                            ESP_LOGI(TAG, "Response content: %s", content->valuestring);
                        }
                    }
                }
            }
            cJSON_Delete(root);
        }
    } else {
        ESP_LOGE(TAG, "Failed to send HTTP request: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

int main() {
    const char *url = "https://api.chatanywhere.tech";
    const char *apiKey = "sk-UMnNg19PCayc8JRdqp4O48OeYeFE9HuEjRZDETC1ldvfhhO1"; // 替换为您的OpenAI API密钥

    send_http_request(url, apiKey);

    return 0;
}
