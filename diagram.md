               ┌───────────────────────┐
               │     Security Task     │
               │-----------------------│
               │ - Verify firmware     │
               │ - Monitor secure boot │
               │ - Signal OTA errors   │
               └─────────┬─────────────┘
                         │ Task Notification / Event Group
                         ▼
               ┌───────────────────────┐
               │   OTA Task (High Pri) │
               │-----------------------│
   Button ISR ─►│ Waits on Binary Sem   │
               │ - Connect WiFi        │
               │ - Download firmware   │
               │ - Verify signature    │
               │ - Switch partition    │
               └─────────┬─────────────┘
                         │ Uses Mutex for UART
                         ▼
               ┌───────────────────────┐
               │ Communication Task    │
               │-----------------------│
               │ - UART logging        │
               │ - Print sensor data   │
               │ - Receive commands    │
               └─────────┬─────────────┘
                         │ Reads from Queue
                         ▼
               ┌───────────────────────┐
               │   Hardware Task       │
               │-----------------------│
               │ - LED Blink           │
               │ - Read ADC / Sensors  │
               │ - Push data to Queue  │
               │ - Give binary sem on  │
               │   button press        │
               └───────────────────────┘