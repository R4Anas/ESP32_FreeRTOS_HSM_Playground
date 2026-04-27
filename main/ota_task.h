#ifndef OTA_TASK_H
#define OTA_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

void ota_task_init(void);
void ota_task_set_button_sem(SemaphoreHandle_t sem);

#endif // OTA_TASK_H