#ifndef HW_TASK_H
#define HW_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "driver/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize hardware task:
 * - LED blinking
 * - ADC reading
 * - Button interrupt
 * - Creates queue & binary semaphore
 */
void hw_task_init(void);

/**
 * @brief Get the ADC queue handle for other tasks
 * @return QueueHandle_t
 */
QueueHandle_t hw_task_get_adc_queue(void);

/**
 * @brief Get the button semaphore handle for OTA task
 * @return SemaphoreHandle_t
 */
SemaphoreHandle_t hw_task_get_button_sem(void);

#ifdef __cplusplus
}
#endif

#endif // HW_TASK_H