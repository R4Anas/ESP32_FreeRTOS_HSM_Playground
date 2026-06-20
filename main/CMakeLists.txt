idf_component_register(
    SRCS "aws_ota.c" "wifi_manager.c" "main.c" "hw_task.c" "comm_task.c"
         "ota_task.c" "security_task.c" "mqtt_task.c"
    INCLUDE_DIRS "."
    EMBED_TXTFILES
        "../certs/root_ca.pem"
        "../certs/device_cert.pem"
        "../certs/device_key.pem"
)