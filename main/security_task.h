#ifndef SECURITY_TASK_H
#define SECURITY_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

// Declare the event group externally so other tasks (e.g., OTA) can access it
extern EventGroupHandle_t security_event_group;

void security_task_init(void);

#define SECURE_BOOT_FAIL_BIT BIT0

#endif // SECURITY_TASK_H