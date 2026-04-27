#include "ota_task.h"
#include "cloud_config.h"
#include "wifi_manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_https_ota.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_http_client.h"

static const char *TAG = "OTA";

static SemaphoreHandle_t button_sem = NULL;

static void ota_task(void *arg)
{
    ESP_LOGI(TAG, "OTA task started");

    while (1)
    {
        // wait for trigger (button or security)
        if (button_sem && xSemaphoreTake(button_sem, portMAX_DELAY) == pdTRUE)
        {
            ESP_LOGI(TAG, "OTA triggered");

            // wait until WiFi connected
            EventGroupHandle_t wifi_events = wifi_get_event_group();
            xEventGroupWaitBits(wifi_events, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

            ESP_LOGI(TAG, "WiFi ready, checking OTA version...");

            // Use local version file for simulation
            const char *ota_url = OTA_LOCAL_FIRMWARE_FILE; // your URL

            esp_http_client_config_t http_config = {
                .url = ota_url,
                .cert_pem = NULL};

            esp_https_ota_config_t ota_config = {
                .http_config = &http_config};

            esp_err_t ret = esp_https_ota(&ota_config);
            if (ret == ESP_OK)
            {
                ESP_LOGI(TAG, "OTA successful, restarting...");
                esp_restart();
            }
            else
            {
                ESP_LOGE(TAG, "OTA failed: %d", ret);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // small delay before next loop
    }
}

void ota_task_init(void)
{
    xTaskCreate(ota_task, "OTA_TASK", 8192, NULL, 6, NULL);
}

void ota_task_set_button_sem(SemaphoreHandle_t sem)
{
    button_sem = sem;
}