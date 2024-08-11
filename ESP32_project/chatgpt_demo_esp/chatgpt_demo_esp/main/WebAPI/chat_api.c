#include "chat_api.h"
#include "esp_log.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "cJSON.h"
#include "settings.h"
#include <sys/param.h>
/*！！！！！！！！！！！！！！使用百度ERNIE-Speed-128K！！！！！！！！！！！！！！！！！！！！*/
//message中的content总长度和system字段总内容不能超过516096个字符，且不能超过126976 tokens 
//总字节长度不能超过516096个字符 ,或ACcessToken失效或为空，重试即可
//sk-rfCLKg6hZIP4yEGTldpW362accwo1OCcy9RvzVfXVZRkKuB0 //Kimi的apikey
static const char *TAG = "chat_api";

static int total_tokens = 0;//总tokens,用于后面判断是否超出token限
static char access_token_chat[256] = {0}; // 存储用于API身份验证的toke
#define MAX_RESPONSE_BUF 1024 * 5 // 定义最大HTTP响应缓冲区大小
//百度api的人设
static const char *baidu_system = "你是一个服务老弱病残的智能健康助手，你会关心用户并且细心，耐心，专业地为用户解决问题";
// static const char polblem1[] = "#当后面的问题有“记备忘录”这四个字时，使用下面格式进行回答，只要返回回答#问题：记备忘录，12点半提醒我洗衣服。 回答：记备忘录，2024年7月7日12点30分，提醒我洗衣服#注意回答的格式为，记备忘录+年月日+要做的事#问题：「记备忘录，14点半给我辅导作业」回答：";
// static const char answer1[] = "记备忘录，2024年7月7日12点30分，给我辅导作业";
// static const char polblem2[] = "记备忘录，15点35提醒我打扫卫生。";
// static const char answer2[] = "记备忘录，2024年7月7日15点35分，提醒我打扫卫生";

// 聊天历史记录队列,最多100条
#define MAX_TOKENS 90000
#define MAX_HISTORY_LENGTH 100
static int history_length = 0;
static char *chat_history[MAX_HISTORY_LENGTH];
static char *answer_history[MAX_HISTORY_LENGTH];
/**
 * @brief HTTP客户端的事件处理回调函数
 *
 * 此函数用于处理HTTP客户端在不同事件发生时的逻辑，例如在连接建立、接收数据、请求完成或断开连接时。
 * 它会根据事件类型来更新或处理相关的数据。
 *
 * @param evt 指向esp_http_client_event_t结构体的指针，包含了事件的详细信息，例如事件类型、数据等。
 * @return esp_err_t 返回ESP_OK表示成功处理了事件，其他错误码表示处理过程中出现了错误。
 */
static esp_err_t http_client_event_handler_chat(esp_http_client_event_t *evt)
{

//    static char *output_buffer;//只能接收一次数据，建议自己定义了数据接收区，删掉这个，后面的output_buffer都要删掉
    // 存储读取的字节数
    static int output_len;
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            // 处理HTTP请求错误事件
            ESP_LOGW(TAG, "HTTP_EVENT_ERROR");
            break;
/*          case HTTP_EVENT_ON_CONNECTED:
            // 处理HTTP连接成功事件
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            // 处理HTTP头部已发送事件
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            // 处理HTTP头部接收事件
//            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break; 
*/           
                 
        case HTTP_EVENT_ON_DATA:
            // 处理HTTP数据接收事件
//            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
//            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, data=%s", (char*)evt->data);
            // 在新请求的情况下清理缓冲区
            if (output_len == 0 && evt->user_data) {
                memset(evt->user_data, 0, MAX_RESPONSE_BUF);
            }
//            if (!esp_http_client_is_chunked_response(evt->client)) / 检查分块编码
                int copy_len = 0;
                if (evt->user_data) {
                    copy_len = MIN(evt->data_len, (MAX_RESPONSE_BUF - output_len));
                    if (copy_len) {
                        memcpy(evt->user_data + output_len, evt->data, copy_len);
                    }
                } 
/*               else {//else这部分定义了数据接收区，可不要
                    int content_len = esp_http_client_get_content_length(evt->client);
                    if (output_buffer == NULL) {
                        output_buffer = (char *) calloc(content_len + 1, sizeof(char));
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    copy_len = MIN(evt->data_len, (content_len - output_len));
                    if (copy_len) {
                        memcpy(output_buffer + output_len, evt->data, copy_len);
                    }
                }
*/ 
                output_len += copy_len;           
            break;
        case HTTP_EVENT_ON_FINISH:
            // 处理HTTP请求完成事件
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            // 解析 JSON 响应数据
            cJSON *json = cJSON_Parse(evt->user_data);
            if (json != NULL)
            {
                //清空user_data，接下来把解析完的数据放到user_data中
                memset(evt->user_data, 0, MAX_RESPONSE_BUF);

                //获取usage数组中的total_tokens对象的值赋给total_tokens
                cJSON *usage_array = cJSON_GetObjectItem(json, "usage");
                if(usage_array != NULL)
                {
                    total_tokens =  cJSON_GetArrayItem(usage_array, 2)->valueint;
                    ESP_LOGI(TAG, "Total tokens: %d", total_tokens);
                }
                //获取result字符串对象的值赋给user_data
                cJSON *result_json = cJSON_GetObjectItem(json, "result");
                if (result_json != NULL && cJSON_IsString(result_json))
                {
                        memcpy(evt->user_data, result_json->valuestring, strlen(result_json->valuestring));
//                        ESP_LOGI(TAG, "Result: %s", (char *)evt->user_data);
                }
            }
            cJSON_Delete(json); // 释放JSON解析器分配的内存  
/*            if (output_buffer != NULL) {
                // 释放输出缓冲区
                free(output_buffer);
                output_buffer = NULL;
            }
            */
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            // 处理HTTP断开连接事件
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
/*if (output_buffer != NULL) {
                // 释放输出缓冲区
                free(output_buffer);
                output_buffer = NULL;
            }
*/            
            output_len = 0;
            break;
/*          case HTTP_EVENT_REDIRECT:
            // 处理HTTP重定向事件
            ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
            esp_http_client_set_header(evt->client, "From", "user@example.com");
            esp_http_client_set_header(evt->client, "Accept", "text/html");
            esp_http_client_set_redirection(evt->client);
            break;        
*/
        default:
            break;   
    }
    return ESP_OK;
}
// 获取AccessToken函数


/**
 * @brief HTTP客户端的事件处理回调函数，处理CHAT_TOKEN数据
 * 
 * 此函数用于处理HTTP客户端在连接、数据传输、请求完成或断开连接时触发的事件。
 * 根据事件类型，执行相应的日志记录和数据处理操作。
 * 
 * @param evt 指向esp_http_client_event_t结构体的指针，包含了事件的详细信息。
 * @return esp_err_t 返回ESP_OK表示成功处理了事件，无错误发生。
 */
static esp_err_t http_client_event_handler_chat_token(esp_http_client_event_t *evt)
{
//    static char *output_buffer;//只能接收一次数据，建议自己定义了数据接收区，删掉这个，后面的output_buffer都要删掉
    // 存储读取的字节数
    static int output_len;
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            // 处理HTTP请求错误事件
            ESP_LOGW(TAG, "HTTP_EVENT_ERROR");
            break;
/*          case HTTP_EVENT_ON_CONNECTED:
            // 处理HTTP连接成功事件
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            // 处理HTTP头部已发送事件
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            // 处理HTTP头部接收事件
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;        
*/
        case HTTP_EVENT_ON_DATA:
            // 处理HTTP数据接收事件
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            // 在新请求的情况下清理缓冲区
            if (output_len == 0 && evt->user_data) {
                memset(evt->user_data, 0, MAX_RESPONSE_BUF);
            }
//            if (!esp_http_client_is_chunked_response(evt->client)) / 检查分块编码
                int copy_len = 0;
                if (evt->user_data) {
                    copy_len = MIN(evt->data_len, (MAX_RESPONSE_BUF - output_len));
                    if (copy_len) {
                        memcpy(evt->user_data + output_len, evt->data, copy_len);
                    }
                } 
/*               else {//else这部分定义了数据接收区，可不要
                    int content_len = esp_http_client_get_content_length(evt->client);
                    if (output_buffer == NULL) {
                        output_buffer = (char *) calloc(content_len + 1, sizeof(char));
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    copy_len = MIN(evt->data_len, (content_len - output_len));
                    if (copy_len) {
                        memcpy(output_buffer + output_len, evt->data, copy_len);
                    }
                }
*/ 
                output_len += copy_len;
                       
            break;
        case HTTP_EVENT_ON_FINISH:
            // 处理HTTP请求完成事件
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            //接受完毕解析json
                cJSON *json = cJSON_Parse(evt->user_data);
                if (json != NULL)
                {
                    cJSON *access_token_json = cJSON_GetObjectItem(json, "access_token");
                        if (access_token_json != NULL)
                        {
                            strncpy(access_token_chat, access_token_json->valuestring, 256 - 1);
                            access_token_chat[256 - 1] = '\0'; // 确保字符串以 null 结尾
                            // 打印访问令牌
//                            ESP_LOGI(TAG, "Access token: %s", access_token_chat);
                        }
                        else
                        {
                            ESP_LOGE(TAG, "Failed to get access token from response");
                        }
                }
                else
                {
                    ESP_LOGE(TAG, "Failed to parse response as JSON");
                }            
                cJSON_Delete(json); // 释放JSON解析器分配的内存  
/*            if (output_buffer != NULL) {
                // 释放输出缓冲区
                free(output_buffer);
                output_buffer = NULL;
            }
            */
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            // 处理HTTP断开连接事件
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
/*if (output_buffer != NULL) {
                // 释放输出缓冲区
                free(output_buffer);
                output_buffer = NULL;
            }
*/            
            output_len = 0;
            break;
/*          case HTTP_EVENT_REDIRECT:
            // 处理HTTP重定向事件
            ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
            esp_http_client_set_header(evt->client, "From", "user@example.com");
            esp_http_client_set_header(evt->client, "Accept", "text/html");
            esp_http_client_set_redirection(evt->client);
            break;        
*/
        default:
            break;   
    }
    return ESP_OK;
}


/**
 * @brief 获取百度语音识别的AccessToken
 * 
 * 该函数通过发送HTTP POST请求到百度语音识别服务的授权端点，获取访问令牌。
 * 
 * @return  成功返回ESP_OK，如果获取失败，返回NULL。
 */
static esp_err_t  getAccessToken_chat()
{
//   char *access_token = NULL;
        char *response_buf = heap_caps_malloc(MAX_RESPONSE_BUF, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (response_buf == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory for response buffer");
            return ESP_FAIL;
        }
    // 初始化HTTP客户端配置，设置请求的URL和证书绑定
    esp_http_client_config_t config = {
        .url = "https://aip.baidubce.com/oauth/2.0/token",
        .event_handler = http_client_event_handler_chat_token,
        .user_data = response_buf,
        .crt_bundle_attach = esp_crt_bundle_attach};
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // 构建请求参数，包含grant_type、client_id和client_secret
    sys_param_t *sys_param = settings_get_parameter();
    char request_params[256];
    snprintf(request_params, sizeof(request_params),
             "grant_type=client_credentials&client_id=%s&client_secret=%s",
             sys_param->key_chat_api, sys_param->key_chat_secret);

    // 设置请求方法为POST，设置请求头的Content-Type，设置POST请求体
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");
    esp_http_client_set_header(client, "Accept", "application/json");
    esp_http_client_set_post_field(client, request_params, strlen(request_params));

    // 执行HTTP请求
    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK) { // 检查HTTP POST请求是否成功
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err)); // 记录HTTP POST请求失败的原因
    }
    free(response_buf); // 释放HTTP响应缓冲区
    esp_http_client_cleanup(client); // 清理HTTP客户端资源
    // 检查访问令牌是否为空
    if (strlen(access_token_chat) == 0) {
        return ESP_FAIL;
    }
    else {
        return ESP_OK;
    }   
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
//出错可能是access_token_chat过期，清空access_token_chat，现在运行即可重新获取
char *getCHATAnswer(char *prompt)
{
        // 检查Access Token是否为空，如果为空则获取新的Access Token
    if (strlen(access_token_chat) == 0) {
//        ESP_LOGI(TAG, "Access token is empty, fetching new token");
        if(getAccessToken_chat() != ESP_OK) 
        {
            ESP_LOGE(TAG, "Failed to get access token");
            return NULL;
        } 
    } 
    char *response_buf = heap_caps_malloc(MAX_RESPONSE_BUF, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (response_buf == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory for response buffer");
            return NULL;
        }

    // 创建一个 JSON 对象
    cJSON *root = cJSON_CreateObject();
    // 创建一个 JSON 数组
    cJSON *messages_array = cJSON_CreateArray();

// 添加聊天历史记录到JSON数组
    for (int i = 0; i < history_length; i++)
    {
        //问题
        cJSON *history_chat = cJSON_CreateObject();
        cJSON_AddStringToObject(history_chat, "role", "user");//问题用user
        cJSON_AddStringToObject(history_chat, "content", chat_history[i]);
        cJSON_AddItemToArray(messages_array, history_chat);
        //回答
        cJSON *history_answer = cJSON_CreateObject();
        cJSON_AddStringToObject(history_answer, "role", "assistant");//回答用assistant
        cJSON_AddStringToObject(history_answer, "content", answer_history[i]);
        cJSON_AddItemToArray(messages_array, history_answer);
    }
    // cJSON *User_history_chat1 = cJSON_CreateObject();
    // cJSON_AddStringToObject(User_history_chat1, "role", "user");//问题用user
    // cJSON_AddStringToObject(User_history_chat1, "content", polblem1);
    // cJSON_AddItemToArray(messages_array, User_history_chat1);

    // cJSON *User_history_answer1 = cJSON_CreateObject();
    // cJSON_AddStringToObject(User_history_answer1, "role", "assistant");//问题用user
    // cJSON_AddStringToObject(User_history_answer1, "content", answer1);
    // cJSON_AddItemToArray(messages_array, User_history_answer1);

    // cJSON *User_history_chat2 = cJSON_CreateObject();
    // cJSON_AddStringToObject(User_history_chat2, "role", "user");//问题用user
    // cJSON_AddStringToObject(User_history_chat2, "content", polblem2);
    // cJSON_AddItemToArray(messages_array, User_history_chat2);

    // cJSON *User_history_answer2 = cJSON_CreateObject();
    // cJSON_AddStringToObject(User_history_answer2, "role", "assistant");//问题用user
    // cJSON_AddStringToObject(User_history_answer2, "content", answer2);
    // cJSON_AddItemToArray(messages_array, User_history_answer2);

    
    // 添加用户输入的消息到JSON数组
    cJSON *user_message = cJSON_CreateObject();
    cJSON_AddStringToObject(user_message, "role", "user");
    cJSON_AddStringToObject(user_message, "content", prompt);
    cJSON_AddItemToArray(messages_array, user_message);

    //开启流式响应，参数为true
//    cJSON_AddStringToObject(root, "stream", "true");
    // 将 "user_id" 项添加到根对象中
    char user_id[] = "yunxi001";
    cJSON_AddStringToObject(root, "user_id", user_id);
    // 将 "messages" 项添加到根对象中
    cJSON_AddItemToObject(root, "messages", messages_array);
    //添加模型的角色设定等内容到JSON根对象
    cJSON_AddStringToObject(root, "system", baidu_system);
/*
// 设置模型参数和消息数组到JSON根对象
    cJSON_AddStringToObject(root, "model", "gpt-3.5-turbo");
    
    // 设置温度和惩罚参数
    cJSON_AddNumberToObject(root, "temperature", 1);
    cJSON_AddNumberToObject(root, "presence_penalty", 0);
    cJSON_AddNumberToObject(root, "frequency_penalty", 0);

*/  
    // 将JSON对象转换为HTTP请求参数
    char *request_params = cJSON_PrintUnformatted(root);
    
    //检查root的总字节长度不能超过516096个字符 
        if (strlen(request_params) >= 516096) {
        ESP_LOGE(TAG, "JSON length exceeds 516096 bytes,please reattempt");
        // 释放最早的聊天记录内存
        free(chat_history[0]);
        free(answer_history[0]);
        // 将所有聊天记录向前移动一位
        for (int i = 0; i < history_length - 1; i++)
        {
            chat_history[i] = chat_history[i + 1];
            answer_history[i] = answer_history[i + 1];
        }
        // 减少聊天记录长度
        history_length--;        
        cJSON_Delete(root);
       free(request_params);
       return NULL;
    }
    
    cJSON_Delete(root);
    ESP_LOGI(TAG, "Chat buf: %s\n", request_params);
   
  //传入access_token_chat
    char url[512];
//    sys_param_t *sys_param = settings_get_parameter();
    snprintf(url, sizeof(url), "https://aip.baidubce.com/rpc/2.0/ai_custom/v1/wenxinworkshop/chat/ernie-speed-128k?access_token=%s",access_token_chat);
    
    // 配置HTTP客户端参数
    esp_http_client_config_t config = {
        .url = url, // 设置请求的URL
        .event_handler = http_client_event_handler_chat, // 设置事件处理函数
        .user_data = response_buf, // 设置用户数据
        .crt_bundle_attach = esp_crt_bundle_attach}; // 设置证书捆绑附加函数
    // 初始化HTTP客户端
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // 设置HTTP请求方法为POST
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    /*
    // 设置HTTP请求头中的Authorization字段
    esp_http_client_set_header(client, "Authorization", sys_param->key_chat_api);
    */
    // 设置HTTP请求头中的Content-Type字段为application/json
    esp_http_client_set_header(client, "Content-Type", "application/json");
    // 设置HTTP POST请求的请求体内容
    esp_http_client_set_post_field(client, request_params, strlen(request_params));
    // 设置HTTP请求的超时时间为20000毫秒
    esp_http_client_set_timeout_ms(client, 20000);

    // 处理HTTP请求并提取回答内容
    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
        //可能是access_token_chat过期，清空access_token_chat，现在运行即可重新获取
        memset(access_token_chat, 0, sizeof(access_token_chat));
    }
    //检查response_buf内数据的长度，将其分配空间储存,返回空间的指针到asr_data
    char *answer = NULL;
    if (response_buf != NULL) {
        answer =  strdup(response_buf); 
    }
    free(request_params);
    esp_http_client_cleanup(client);

// 保存聊天记录
    //message中的content总长度和system字段总内容不能超过516096个字符，且不能超过126976 tokens        
    // 管理问题和回答，确保不超过最大长度
    if (total_tokens >= MAX_TOKENS || history_length >= 2) {
        // 释放最早的聊天记录内存
        free(chat_history[0]);
        free(answer_history[0]);
        // 将所有聊天记录向前移动一位
        for (int i = 0; i < history_length - 1; i++)
        {
            chat_history[i] = chat_history[i + 1];
            answer_history[i] = answer_history[i + 1];
        }
        // 减少聊天记录长度
        history_length--;
    }
    // 添加新的聊天问题,和回答到历史记录中
    chat_history[history_length] = (char *)heap_caps_malloc(strlen(prompt) + 1, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if( chat_history[history_length] != NULL) {
            memcpy(chat_history[history_length], prompt, strlen(prompt));
        }
        else {
            ESP_LOGE(TAG, "Failed to allocate memory for chat history");
            return NULL;
        }
    answer_history[history_length] = (char *)heap_caps_malloc(strlen(answer) + 1, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (answer_history[history_length] != NULL) {
            memcpy(answer_history[history_length], answer, strlen(answer));
        }
        else {
            ESP_LOGE(TAG, "Failed to allocate memory for answer history");
            return NULL;
        }

    // 增加聊天记录长度
    history_length++;

    return answer;
}

