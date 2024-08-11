#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "esp_check.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "app_mqtt.h"

static const char *TAG = "APP_MQTT";

static bool mqtt_connected = false;
static esp_mqtt_client_handle_t client;

esp_err_t mqtt_air_purifer_on(void)
{
    ESP_RETURN_ON_FALSE(mqtt_connected, ESP_ERR_INVALID_STATE, TAG,  "MQTT is not connected");

    int msg_id = esp_mqtt_client_publish(client, "air_purifier/control",
                        "{\"input\": \"Turn On the Air Purifier\", \"siteId\": \"esp32\"}", 0, 1, 0);

    ESP_RETURN_ON_FALSE(msg_id >= 0, ESP_FAIL, TAG, "Sent publish failed, msg_id=%d", msg_id);

    ESP_LOGI(TAG, "Sent publish successful, msg_id=%d", msg_id);

    return ESP_OK;
}

esp_err_t mqtt_air_purifer_off(void)
{
    ESP_RETURN_ON_FALSE(mqtt_connected, ESP_ERR_INVALID_STATE, TAG,  "MQTT is not connected");

    int msg_id = esp_mqtt_client_publish(client, "air_purifier/control",
                        "{\"input\": \"Turn Off the Air Purifier\", \"siteId\": \"esp32\"}", 0, 1, 0);

    ESP_RETURN_ON_FALSE(msg_id >= 0, ESP_FAIL, TAG, "Sent publish failed, msg_id=%d", msg_id);

    ESP_LOGI(TAG, "Sent publish successful, msg_id=%d", msg_id);

    return ESP_OK;
}

esp_err_t mqtt_lamp_on(void)
{
    ESP_RETURN_ON_FALSE(mqtt_connected, ESP_ERR_INVALID_STATE, TAG,  "MQTT is not connected");

    int msg_id = esp_mqtt_client_publish(client, "xiaomi_lamp/control",
                        "{\"input\": \"Turn On the Lamp\", \"siteId\": \"esp32\"}", 0, 1, 0);

    ESP_RETURN_ON_FALSE(msg_id >= 0, ESP_FAIL, TAG, "Sent publish failed, msg_id=%d", msg_id);

    ESP_LOGI(TAG, "Sent publish successful, msg_id=%d", msg_id);

    return ESP_OK;
}

esp_err_t mqtt_lampr_off(void)
{
    ESP_RETURN_ON_FALSE(mqtt_connected, ESP_ERR_INVALID_STATE, TAG,  "MQTT is not connected");

    int msg_id = esp_mqtt_client_publish(client, "xiaomi_lamp/control",
                        "{\"input\": \"Turn Off the Lamp\", \"siteId\": \"esp32\"}", 0, 1, 0);

    ESP_RETURN_ON_FALSE(msg_id >= 0, ESP_FAIL, TAG, "Sent publish failed, msg_id=%d", msg_id);

    ESP_LOGI(TAG, "Sent publish successful, msg_id=%d", msg_id);

    return ESP_OK;
}

esp_err_t mqtt_lamp_brighter(void)
{
    ESP_RETURN_ON_FALSE(mqtt_connected, ESP_ERR_INVALID_STATE, TAG,  "MQTT is not connected");

    int msg_id = esp_mqtt_client_publish(client, "xiaomi_lamp/control",
                        "{\"input\": \"Turn Lamp Brighter\", \"siteId\": \"esp32\"}", 0, 1, 0);

    ESP_RETURN_ON_FALSE(msg_id >= 0, ESP_FAIL, TAG, "Sent publish failed, msg_id=%d", msg_id);

    ESP_LOGI(TAG, "Sent publish successful, msg_id=%d", msg_id);

    return ESP_OK;
}

esp_err_t mqtt_lampr_dimmer(void)
{
    ESP_RETURN_ON_FALSE(mqtt_connected, ESP_ERR_INVALID_STATE, TAG,  "MQTT is not connected");

    int msg_id = esp_mqtt_client_publish(client, "xiaomi_lamp/control",
                        "{\"input\": \"Turn Lamp Dimmer\", \"siteId\": \"esp32\"}", 0, 1, 0);

    ESP_RETURN_ON_FALSE(msg_id >= 0, ESP_FAIL, TAG, "Sent publish failed, msg_id=%d", msg_id);

    ESP_LOGI(TAG, "Sent publish successful, msg_id=%d", msg_id);

    return ESP_OK;
}

esp_err_t mqtt_led_on(void)
{
    ESP_RETURN_ON_FALSE(mqtt_connected, ESP_ERR_INVALID_STATE, TAG,  "MQTT is not connected");

    int msg_id = esp_mqtt_client_publish(client, "led_strip/control",
                        "{\"input\": \"Turn On the LED Strip\", \"siteId\": \"esp32\"}", 0, 1, 0);

    ESP_RETURN_ON_FALSE(msg_id >= 0, ESP_FAIL, TAG, "Sent publish failed, msg_id=%d", msg_id);

    ESP_LOGI(TAG, "Sent publish successful, msg_id=%d", msg_id);

    return ESP_OK;
}

esp_err_t mqtt_led_off(void)
{
    ESP_RETURN_ON_FALSE(mqtt_connected, ESP_ERR_INVALID_STATE, TAG,  "MQTT is not connected");

    int msg_id = esp_mqtt_client_publish(client, "led_strip/control",
                        "{\"input\": \"Turn Off the LED Strip\", \"siteId\": \"esp32\"}", 0, 1, 0);

    ESP_RETURN_ON_FALSE(msg_id >= 0, ESP_FAIL, TAG, "Sent publish failed, msg_id=%d", msg_id);

    ESP_LOGI(TAG, "Sent publish successful, msg_id=%d", msg_id);

    return ESP_OK;
}

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        mqtt_connected = true;

        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        mqtt_connected = false;
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri                  = CONFIG_BROKER_URL,
        .credentials.username                = CONFIG_BROKER_USERNAME,
        .credentials.authentication.password = CONFIG_BROKER_PASSWORD,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}
