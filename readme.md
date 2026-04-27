# ESP32 FreeRTOS + Secure OTA Learning Project

A personal project built to strengthen my understanding of **FreeRTOS**, **embedded firmware architecture**, and **practical device security concepts** using the ESP32.

## What I focused on

* Building a **multi-tasking FreeRTOS application**
* Learning **task scheduling**, **queues**, **binary semaphores**, and **event groups**
* Working directly with **hardware peripherals** (ADC, GPIO, UART)
* Exploring **firmware integrity verification** using SHA256
* Implementing **development-safe secure boot concepts** without burning eFuse
* Designing a real-world style **OTA update system** using partition switching
* Integrating **AWS S3-based HTTPS OTA delivery**

---

## Core FreeRTOS Concepts Practiced

* **Tasks** for modular firmware design
* **Queues** for ADC data transfer
* **Semaphores** for event-based OTA triggers
* **Event Groups** for security-to-OTA communication
* **Non-blocking task scheduling** with `vTaskDelay()`

---

## Project Architecture

```text
hw_task        -> Hardware interaction
comm_task      -> UART logging
security_task  -> Firmware verification
ota_task       -> OTA updates
```

---

## Security + OTA Highlights

* SHA256 firmware hashing
* Partition-based OTA updates
* Safe rollback-friendly design
* AWS S3 firmware hosting
* HTTPS firmware delivery

---

## Why I Built This

This project was mainly a hands-on way for me to move beyond basic ESP32 examples and better understand how modern embedded systems handle:

* RTOS design
* Secure firmware validation
* Remote firmware deployment
* Production-style modular architecture

---

## Current Scope

* FreeRTOS fundamentals
* Embedded security concepts
* OTA infrastructure
* Cloud-connected firmware updates

---

## Next Learning Goals

* MQTT / AWS IoT integration
* Firmware signing
* Secure boot eFuse deployment
* Rollback and fleet management

---

A practical learning project focused on building stronger embedded systems development skills while following real-world design patterns.
