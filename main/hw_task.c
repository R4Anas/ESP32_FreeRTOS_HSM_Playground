#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "hw_task.h"
#include "comm_task.h"

#define LED_PIN GPIO_NUM_2
#define BUTTON_PIN GPIO_NUM_0
#define ADC_CHANNEL ADC1_CHANNEL_6 // GPIO34

static QueueHandle_t adc_queue;
static SemaphoreHandle_t button_sem;

static void button_isr_handler(void* arg) {
    xSemaphoreGiveFromISR(button_sem, NULL);
}

static void hw_task(void* arg)
{
    int adc_val;
    while(1) {
        // Blink LED
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500));

        // Read ADC
        adc_val = adc1_get_raw(ADC_CHANNEL);
        xQueueSend(adc_queue, &adc_val, 0);
    }
}

void hw_task_init(void)
{
    // Configure LED
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    // Configure Button
    gpio_reset_pin(BUTTON_PIN);
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
    gpio_set_intr_type(BUTTON_PIN, GPIO_INTR_NEGEDGE);

    // Create queue and semaphore
    adc_queue = xQueueCreate(10, sizeof(int));
    button_sem = xSemaphoreCreateBinary();

    // Install ISR
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_PIN, button_isr_handler, NULL);

    // Create task
    xTaskCreate(hw_task, "HW_TASK", 4096, NULL, 5, NULL);

    comm_task_set_queue(adc_queue);
}