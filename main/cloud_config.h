#ifndef CLOUD_CONFIG_H
#define CLOUD_CONFIG_H

// WiFi credentials
#define WIFI_SSID        "Vodafone-FA4C"
#define WIFI_PASS        "pakistan123"

// Device info
#define DEVICE_ID        "esp32_1"

// OTA version
#define CURRENT_FW_VERSION   "1.0.0"

// Local OTA simulation path (for testing)
#define OTA_LOCAL_VERSION_FILE "file://aws_ota/version.json"
#define OTA_LOCAL_FIRMWARE_FILE "file://aws_ota/firmware.bin"

// AWS S3 OTA URLs (replace later)
#define OTA_S3_VERSION_URL  "https://esp32-ota-anas.s3.amazonaws.com/version.json"
#define OTA_S3_FIRMWARE_URL "https://esp32-ota-anas.s3.amazonaws.com/firmware.bin"

#endif