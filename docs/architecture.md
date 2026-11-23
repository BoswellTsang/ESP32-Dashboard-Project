# System architecture

```text
OpenWeatherMap / Notion cloud APIs
          | HTTP/JSON over Wi-Fi
          v
LiPo -> charger -> 3.3 V rail -> ESP32 -- SPI --> 4.2" e-ink panel
                                |
                                +------ I²C --> DS3231 RTC
                                |
                                +------ Deep sleep / RTC-retained variables
```

The ESP32 initializes peripherals, reads RTC information, connects to Wi-Fi when needed, parses remote API responses, refreshes the e-ink panel and returns to deep sleep.

**Main interfaces:** SPI for display; I²C for RTC; Wi-Fi and HTTP/JSON for remote services.

**Hardware:** custom two-layer PCB with charging/regulation, reset circuitry, decoupling and status LED. The final KiCad files need to be uploaded to document exact net names, component values and footprints.
