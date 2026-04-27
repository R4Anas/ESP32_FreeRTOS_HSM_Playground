#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "comm_task.h"
#include "esp_log.h"

static QueueHandle_t hw_adc_queue;
static SemaphoreHandle_t uart_mutex;

void comm_task(void* arg)
{
    int adc_val;
    uart_mutex = xSemaphoreCreateMutex();

    while(1) {
        if(xQueueReceive(hw_adc_queue, &adc_val, pdMS_TO_TICKS(1000))) {
            xSemaphoreTake(uart_mutex, portMAX_DELAY);
            ESP_LOGI("COMM", "ADC Value: %d", adc_val);
            xSemaphoreGive(uart_mutex);
        }
    }
}

void comm_task_init(void)
{
    xTaskCreate(comm_task, "COMM_TASK", 4096, NULL, 4, NULL);
}

void comm_task_set_queue(QueueHandle_t q)
{
    hw_adc_queue = q;
}