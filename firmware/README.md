# Firmware

The existing Arduino sketch remains at the repository root:

[ESP32_Dashboard_Project_Code.ino](../ESP32_Dashboard_Project_Code.ino)

It implements dashboard integration using the ESP32, display, RTC, Wi-Fi and cloud APIs. Please document the tested ESP32 core version, exact required libraries and board settings once they are recovered.

**Credential safety:** use locally stored secrets for working Wi-Fi/API credentials and never commit them. The public sketch currently uses `XXX` placeholders in the credential fields. Do not replace these placeholders with actual secret values in Git.
