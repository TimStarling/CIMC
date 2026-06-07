<div align="right">

[中文](README.md) | [English](README_EN.md)

</div>

# 2025 Siemens Cup Industrial Embedded System Project

This project is an entry for the Industrial Embedded System Development track of the 2025 Siemens Cup China Intelligent Manufacturing Challenge. The team number is `2025244579`. The work won a provincial first prize and qualified for the national competition.

## Project Introduction

The system is built on the `GD32F470VET6` microcontroller and implements an industrial voltage acquisition, display, alarm, storage, and serial-interaction system. It samples voltage through ADC, records timestamped data through RTC, writes categorized logs to a TF card, and supports serial parameter configuration, external Flash persistence, OLED status display, LED alarm indication, and simple hidden-data encoding.

## Core Functions

1. System initialization and self-test for LED, keys, UART, SPI Flash, RTC, ADC, OLED, and TF card.
2. RTC time management and serial time query/configuration.
3. Configuration loading from `config.ini`, including `Ratio` and `Limit` parameters.
4. ADC voltage acquisition, scaling, threshold judgment, and alarm output.
5. TF-card logging with categorized data files.
6. OLED and serial interaction for debugging and demonstration.
