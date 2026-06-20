#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "hw_task.h"
#include "comm_task.h"
#include "security_task.h"
#include "ota_task.h"
#include "mqtt_task.h"
#include "esp_log.h"
#include "wifi_manager.h"

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_INFO);

    wifi_init();

    // Single semaphore shared between mqtt_task (gives) and ota_task (takes)
    SemaphoreHandle_t ota_sem = xSemaphoreCreateBinary();
    ota_task_set_button_sem(ota_sem);

    hw_task_init();
    comm_task_init();
    security_task_init();
    ota_task_init();
    mqtt_task_init(ota_sem);

    while(1) vTaskDelay(pdMS_TO_TICKS(1000));
}