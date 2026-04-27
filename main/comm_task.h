#ifndef COMM_TASK_H
#define COMM_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize communication task:
 * - UART logging
 * - Receives ADC values from queue
 * - Uses mutex to protect UART
 */
void comm_task_init(void);

/**
 * @brief Set the ADC queue handle from HW task
 * @param q QueueHandle_t of ADC readings
 */
void comm_task_set_queue(QueueHandle_t q);

/**
 * @brief Get UART mutex for thread-safe logging
 * @return SemaphoreHandle_t
 */
SemaphoreHandle_t comm_task_get_uart_mutex(void);

#ifdef __cplusplus
}
#endif

#endif // COMM_TASK_H