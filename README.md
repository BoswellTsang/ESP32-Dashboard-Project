# ESP32 E-Ink Smart Dashboard

**Status:** Completed (August 2026) · **Type:** Personal Project  
**Tools:** KiCad, ESP32, C/C++, SPI, I²C, Wi-Fi, HTTP/JSON, Arduino

> A battery-powered, 4.2-inch e-ink dashboard built from a breadboard prototype into a custom two-layer PCB. It automatically displays weather, date/time, and upcoming events from online services.

<img width="582" height="518" alt="image" src="https://github.com/user-attachments/assets/be87185e-1681-42f7-aede-ff714b443325" />

## At a glance

| Subsystem | Implementation |
| --- | --- |
| Microcontroller | ESP32 |
| Display | 4.2-inch Waveshare e-ink, SSD1683-compatible driver |
| Timekeeping | DS3231 real-time clock (I²C) |
| PCB | Custom two-layer KiCad design, fabricated and hand assembled |
| Power | LiPo battery, charging circuit and regulated 3.3 V |
| Display communication | SPI |
| Online services | Wi-Fi, HTTP/JSON, OpenWeatherMap and Notion |
| Low-power operation | Periodic ESP32 deep sleep; retained variables in RTC memory |

## Project overview

I designed and assembled a standalone dashboard to combine electronic hardware design, power management, and firmware in one working device. I initially developed the system on a breadboard, then created a custom PCB incorporating the ESP32, RTC, battery charging and regulation, reset circuitry, status LED and supporting passives. Firmware retrieves online data, updates the display and enters deep sleep between refreshes.

## Hardware development

- Captured the complete schematic and routed a custom **two-layer PCB** using KiCad.
- Designed 3.3 V distribution, LiPo charging connections, reset/status circuits, decoupling, and SPI/I²C interfaces.
- Planned component placement, trace routing and a ground copper pour.
- Fabricated, soldered, assembled and integrated the PCB with the display and firmware.

KiCad Files are published in [hardware/README.md](hardware/README.md).

## Firmware

- Communicates with the display through SPI and with the DS3231 through I²C.
- Retrieves weather and calendar data using Wi-Fi and HTTP/JSON, with OpenWeatherMap and Notion integrations.
- Formats information for the e-ink display.
- Coordinates deep-sleep wake/refresh cycles.
- Preserves required data in RTC-retained memory; fixed-size character arrays were used where dynamic string storage caused persistence problems.

**Source:** [ESP32_Dashboard_Project_Code.ino](ESP32_Dashboard_Project_Code.ino) — the existing sketch is preserved at the repository root. 

## Testing and results

| Feature | Status |
| --- | --- |
| Fabricated PCB and completed assembly | Completed |
| Weather, date/time and calendar display | Completed |
| SPI, I²C and Wi-Fi data access | Completed |
| Deep-sleep state retention | Implemented and debugged |
| Measured sleep/active current | Not documented |
| Battery runtime | Not documented |

See [test record](docs/testing.md).

## Repository guide

- [System architecture](docs/architecture.md)
- [Engineering decisions and debugging](docs/design-decisions.md)
- [Test record](docs/testing.md)
- [Project progress log](docs/progress-log.md)
- [Hardware file guide](hardware/README.md)
- [Firmware notes](firmware/README.md)

