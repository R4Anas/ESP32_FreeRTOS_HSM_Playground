#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hw_task.h"
#include "comm_task.h"
#include "security_task.h"
#include "ota_task.h"
#include "esp_log.h"
#include "wifi_manager.h"

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_INFO);

    // WiFi
    wifi_init();

    // Create OTA button semaphore
    static SemaphoreHandle_t ota_button_sem = NULL;
    ota_button_sem = xSemaphoreCreateBinary();
    ota_task_set_button_sem(ota_button_sem);

    // Init tasks
    hw_task_init();
    comm_task_init();
    security_task_init();
    ota_task_init();

    // simulate button press to test local OTA
    xSemaphoreGive(ota_button_sem);

    while(1) vTaskDelay(pdMS_TO_TICKS(1000));
}