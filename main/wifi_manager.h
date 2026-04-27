#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "freertos/event_groups.h"

void wifi_init(void);

// Return the WiFi event group handle
EventGroupHandle_t wifi_get_event_group(void);

#define WIFI_CONNECTED_BIT BIT0

#endif