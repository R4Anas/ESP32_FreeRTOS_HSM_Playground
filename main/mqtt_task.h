#ifndef MQTT_TASK_H
#define MQTT_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

void mqtt_task_init(SemaphoreHandle_t ota_sem);

#endif
