#ifndef AWS_OTA_H
#define AWS_OTA_H

#include "esp_err.h"

// Check cloud/local version and perform OTA if needed
esp_err_t aws_ota_check_for_updates(void);

#endif