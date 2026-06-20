#include "aws_ota.h"
#include "cloud_config.h"

#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_system.h"

#include "cJSON.h"
#include <string.h>
#include <stdlib.h>

extern const uint8_t root_ca_pem_start[] asm("_binary_root_ca_pem_start");

static const char *TAG = "AWS_OTA";

// Buffer for version.json
#define MAX_HTTP_OUTPUT_BUFFER 512
static char ota_response_buffer[MAX_HTTP_OUTPUT_BUFFER];


// ==========================
// HTTP Event Handler
// ==========================
static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    static int output_len = 0;

    switch(evt->event_id) {

        case HTTP_EVENT_ON_DATA:
            if (!esp_http_client_is_chunked_response(evt->client)) {
                int copy_len = evt->data_len;

                if (output_len + copy_len < MAX_HTTP_OUTPUT_BUFFER) {
                    memcpy(ota_response_buffer + output_len, evt->data, copy_len);
                    output_len += copy_len;
                    ota_response_buffer[output_len] = 0;
                }
            }
            break;

        case HTTP_EVENT_ON_FINISH:
            output_len = 0;
            break;

        default:
            break;
    }

    return ESP_OK;
}


// ==========================
// Version Check
// ==========================
static bool is_new_firmware_available(char *firmware_url, size_t url_size)
{
    esp_http_client_config_t config = {
        .url = OTA_S3_VERSION_URL,
        .event_handler = http_event_handler,
        .cert_pem = (const char *)root_ca_pem_start,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    if (esp_http_client_perform(client) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to fetch version.json");
        esp_http_client_cleanup(client);
        return false;
    }

    esp_http_client_cleanup(client);

    cJSON *root = cJSON_Parse(ota_response_buffer);
    if (!root) {
        ESP_LOGE(TAG, "JSON parse failed");
        return false;
    }

    cJSON *version = cJSON_GetObjectItem(root, "version");
    cJSON *firmware = cJSON_GetObjectItem(root, "firmware");

    if (!cJSON_IsString(version) || !cJSON_IsString(firmware)) {
        cJSON_Delete(root);
        return false;
    }

    ESP_LOGI(TAG, "Current FW: %s, Cloud FW: %s",
             CURRENT_FW_VERSION,
             version->valuestring);

    if (strcmp(version->valuestring, CURRENT_FW_VERSION) != 0) {
        strncpy(firmware_url, firmware->valuestring, url_size);
        firmware_url[url_size - 1] = '\0';

        cJSON_Delete(root);
        return true;
    }

    cJSON_Delete(root);
    return false;
}


// ==========================
// Public OTA API
// ==========================
esp_err_t aws_ota_check_for_updates(void)
{
    char firmware_url[256] = {0};

    if (!is_new_firmware_available(firmware_url, sizeof(firmware_url))) {
        ESP_LOGI(TAG, "No new firmware available");
        return ESP_OK;
    }

    ESP_LOGI(TAG, "New firmware found: %s", firmware_url);

    esp_http_client_config_t http_config = {
        .url = firmware_url,
        .cert_pem = (const char *)root_ca_pem_start,
    };

    esp_https_ota_config_t ota_config = {
        .http_config = &http_config,
    };

    esp_err_t ret = esp_https_ota(&ota_config);

    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "OTA successful. Restarting...");
        esp_restart();
    } else {
        ESP_LOGE(TAG, "OTA failed");
    }

    return ret;
}