# Design decisions and debugging notes

## Breadboard to PCB
Developed the core functions on a breadboard first, then moved to a two-layer PCB designed in KiCad with power distribution, support circuitry and a ground pour. This created a permanent, assembled implementation instead of a collection of development wiring.

## E-ink display driver
Tested several driver/controller definitions after encountering display compatibility problems. The operating configuration uses an SSD1683-compatible e-ink driver.

## Deep sleep and retained memory
The program needed to keep selected information across sleep/wake cycles. Data held in ordinary RAM did not persist as needed. Persistent values were moved to ESP32 RTC memory, and selected dynamic String usage was replaced with fixed-size character arrays to avoid retention/lifetime issues.

## Serial and networking interfaces
SPI drives the e-ink display; I²C connects the DS3231 RTC. The ESP32 makes Wi-Fi HTTP requests, parses JSON and formats weather/calendar information.


