#include "mqtt_task.h"
#include "cloud_config.h"
#include "wifi_manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "mqtt_client.h"
#include "cJSON.h"
#include "driver/adc.h"
#include <string.h>

static const char *TAG = "MQTT";

static SemaphoreHandle_t         s_ota_sem      = NULL;
static esp_mqtt_client_handle_t  s_client       = NULL;
static volatile bool             s_connected    = false;

// Symbols injected by CMakeLists EMBED_TXTFILES
extern const uint8_t root_ca_pem_start[]     asm("_binary_root_ca_pem_start");
extern const uint8_t device_cert_pem_start[] asm("_binary_device_cert_pem_start");
extern const uint8_t device_key_pem_start[]  asm("_binary_device_key_pem_start");

// ==========================
// MQTT Event Handler
// ==========================
static void mqtt_event_handler(void *arg, esp_event_base_t base,
                               int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch ((esp_mqtt_event_id_t)event_id) {

        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "Connected to AWS IoT Core");
            s_connected = true;
            esp_mqtt_client_subscribe(s_client, MQTT_TOPIC_OTA, 1);
            ESP_LOGI(TAG, "Subscribed to %s", MQTT_TOPIC_OTA);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "Disconnected — will retry");
            s_connected = false;
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "Received on [%.*s]", event->topic_len, event->topic);
            if (strncmp(event->topic, MQTT_TOPIC_OTA, event->topic_len) == 0) {
                ESP_LOGI(TAG, "OTA trigger received via MQTT");
                if (s_ota_sem) xSemaphoreGive(s_ota_sem);
            }
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT error event");
            break;

        default:
            break;
    }
}

// ==========================
// JSON Payload Builder
// ==========================
static char *build_payload(void)
{
    int raw      = adc1_get_raw(ADC1_CHANNEL_6);
    int moisture = (raw * 100) / 4095;
    float temp   = 22.0f + (raw % 50) / 10.0f; // simulated temperature

    cJSON *root   = cJSON_CreateObject();
    cJSON *sensor = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "device_id", DEVICE_ID);
    cJSON_AddNumberToObject(root, "timestamp", (double)(esp_timer_get_time() / 1000000ULL));
    cJSON_AddNumberToObject(sensor, "moisture",    moisture);
    cJSON_AddNumberToObject(sensor, "temperature", (double)((int)(temp * 10)) / 10.0);
    cJSON_AddItemToObject(root, "sensor", sensor);
    cJSON_AddStringToObject(root, "firmware_version", CURRENT_FW_VERSION);
    cJSON_AddStringToObject(root, "status", "ok");

    char *out = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return out;
}

// ==========================
// Publish Task
// ==========================
static void mqtt_publish_task(void *arg)
{
    // Block until WiFi has an IP
    EventGroupHandle_t wifi_eg = wifi_get_event_group();
    xEventGroupWaitBits(wifi_eg, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

    ESP_LOGI(TAG, "WiFi ready — starting MQTT client");
    esp_mqtt_client_start(s_client);

    while (1) {
        if (s_connected) {
            char *payload = build_payload();
            if (payload) {
                int id = esp_mqtt_client_publish(s_client, MQTT_TOPIC_PUBLISH,
                                                 payload, 0, 1, 0);
                ESP_LOGI(TAG, "Published msg_id=%d: %s", id, payload);
                cJSON_free(payload);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(PUBLISH_INTERVAL_MS));
    }
}

// ==========================
// Public Init
// ==========================
void mqtt_task_init(SemaphoreHandle_t ota_sem)
{
    s_ota_sem = ota_sem;

    const esp_mqtt_client_config_t cfg = {
        .broker.address.hostname  = AWS_IOT_ENDPOINT,
        .broker.address.transport = MQTT_TRANSPORT_OVER_SSL,
        .broker.address.port      = AWS_IOT_PORT,
        .broker.verification.certificate = (const char *)root_ca_pem_start,
        .credentials.client_id    = MQTT_CLIENT_ID,
        .credentials.authentication.certificate = (const char *)device_cert_pem_start,
        .credentials.authentication.key         = (const char *)device_key_pem_start,
    };

    s_client = esp_mqtt_client_init(&cfg);
    esp_mqtt_client_register_event(s_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);

    xTaskCreate(mqtt_publish_task, "MQTT_TASK", 8192, NULL, 5, NULL);
}
