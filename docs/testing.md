# Testing and validation

This log distinguishes tested behavior from missing measurements.

| Test | Evidence / outcome | Status |
| --- | --- | --- |
| PCB assembly | Fabricated, soldered and integrated the two-layer board | Completed |
| Display output | Weather, time/date and upcoming events appear on e-ink | Completed |
| RTC communication | DS3231 timekeeping integrated through I²C | Completed |
| Networking | OpenWeatherMap and Notion data accessed by Wi-Fi / HTTP | Completed |
| Sleep/wake state | RTC memory persistence issue investigated and resolved | Completed |
| Sleep current | No recorded numerical measurement | Not measured |
| Active current | No recorded numerical measurement | Not measured |
| Battery operating life | No recorded numerical measurement | Not measured |
| PCB DRC/fabrication exports | Original project files not in this repo | Not yet uploaded |

## Future measurement procedure
1. Measure supply current during boot, Wi-Fi, display update and deep sleep.
2. Record sampling instrument, configuration, update interval and battery capacity.
3. Calculate average current from measured active and sleep durations.
4. Compare estimated runtime with an actual battery discharge test.
