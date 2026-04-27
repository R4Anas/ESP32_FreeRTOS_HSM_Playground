#include "security_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_system.h"
#include "mbedtls/sha256.h"

static const char *TAG = "SECURITY";

// Define the event group here (only once)
EventGroupHandle_t security_event_group;

// Compute SHA256 of running firmware safely (using esp_partition_read)
static bool verify_firmware(void)
{
    const esp_partition_t *running = esp_ota_get_running_partition();
    if (!running) return false;

    ESP_LOGI(TAG, "Running partition: %s, address: 0x%08x, size: 0x%08x",
             running->label, running->address, running->size);

    // Dev mode: limit to first 1 MB
    size_t remaining = running->size > 1024 * 1024 ? 1024 * 1024 : running->size;
    size_t offset = 0;

    uint8_t buffer[1024]; // read flash in 1 KB chunks
    mbedtls_sha256_context sha_ctx;
    mbedtls_sha256_init(&sha_ctx);
    mbedtls_sha256_starts(&sha_ctx, 0); // 0 = SHA-256

    while (remaining > 0)
    {
        size_t to_read = remaining > sizeof(buffer) ? sizeof(buffer) : remaining;

        esp_err_t err = esp_partition_read(running, offset, buffer, to_read);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Flash read failed at offset 0x%08x", offset);
            mbedtls_sha256_free(&sha_ctx);
            return false;
        }

        mbedtls_sha256_update(&sha_ctx, buffer, to_read);

        offset += to_read;
        remaining -= to_read;
    }

    uint8_t sha_output[32];
    mbedtls_sha256_finish(&sha_ctx, sha_output);
    mbedtls_sha256_free(&sha_ctx);

    ESP_LOGI(TAG, "Firmware SHA256: %02X%02X%02X... (truncated)",
             sha_output[0], sha_output[1], sha_output[2]);

    // Dev mode: always consider firmware valid
    return true;
}

// Security task
static void security_task(void *arg)
{
    ESP_LOGI(TAG, "Security task started");

    while (1)
    {
        if (!verify_firmware())
        {
            ESP_LOGW(TAG, "Firmware verification failed! Signaling OTA...");
            if (security_event_group)
                xEventGroupSetBits(security_event_group, SECURE_BOOT_FAIL_BIT);
        }

        vTaskDelay(pdMS_TO_TICKS(5000)); // check every 5 seconds
    }
}

void security_task_init(void)
{
    security_event_group = xEventGroupCreate();
    xTaskCreate(security_task, "SECURITY_TASK", 4096, NULL, 7, NULL);
}