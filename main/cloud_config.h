#ifndef CLOUD_CONFIG_H
#define CLOUD_CONFIG_H

// WiFi credentials
#define WIFI_SSID        "Vodafone-FA4C"
#define WIFI_PASS        "pakistan123"

// Device info
#define DEVICE_ID            "esp32-node-01"
#define CURRENT_FW_VERSION   "1.0.0"

// AWS IoT Core
#define AWS_IOT_ENDPOINT     "a16c7wazpnwyp7-ats.iot.eu-central-1.amazonaws.com"
#define AWS_IOT_PORT         8883
#define MQTT_CLIENT_ID       "esp32-node-01"
#define MQTT_TOPIC_PUBLISH   "esp32/sensor/data"
#define MQTT_TOPIC_OTA       "esp32/ota/trigger"
#define PUBLISH_INTERVAL_MS  60000

// AWS S3 OTA URLs
#define OTA_S3_VERSION_URL   "https://esp32-ota-anas.s3.amazonaws.com/version.json"
#define OTA_S3_FIRMWARE_URL  "https://esp32-ota-anas.s3.amazonaws.com/firmware.bin"

#endif