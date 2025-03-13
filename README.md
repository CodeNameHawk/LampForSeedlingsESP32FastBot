# Grow Light Controller for Seedlings

ESP32-based smart grow light controller with Telegram bot interface. A weekend project for plant enthusiasts.

## Overview

Supplemental lighting plays a crucial role in ensuring healthy growth and development of seedlings. Insufficient natural light can cause seedlings to become leggy, weak, and susceptible to diseases.

Plants typically need 12-16 hours of light daily, depending on their requirements. This controller helps maintain optimal lighting by automatically managing grow lights in the morning and evening to supplement natural daylight.

## Features

- ESP32-based timer control
- Telegram bot interface for remote control and monitoring
- Configurable lighting schedules
- Real-time status monitoring
- Over-the-air (OTA) updates
- LittleFS support for storing schedules

## Hardware Requirements

- ESP32 development board
- 5V power supply (2A USB charger works well)
- Relay module (single or dual channel, get dual channel as "spare never hurts")
- Grow light

## Wiring Diagram

   Mains voltage is supplied to the power supply and through the normally open relay contact to the lamp.
   +5V and GND from the power supply are connected to the ESP module and relay power supply at the JC-VCC pin.
   GPIO33 pin of the ESP module connects to In2 on the relay module.
   +3.3V pin connects to VCC.

## Setup Instructions

1. Create a Telegram bot (name it "Lamp for Seedlings") and get your token and ID
   - Follow [FastBot Telegram tutorial](https://kit.alexgyver.ru/tutorials/telegram-basic/)
   - Write down your bot token and user ID on paper

2. Configure the device
   - Use Arduino Release v3.1.3 [Arduino core for esp32](https://github.com/espressif/arduino-esp32)
   - For ESP32 setup guide, see [ESP32 Programming Guide](https://alash-electronics.kz/blogs/projects-arduino/esp32)
   - Download and install FastBot library [FastBot](https://github.com/GyverLibs/FastBot)
   - Enter your WiFi credentials and Telegram tokens in `Secrets.ino`
   - Connect ESP32 module to computer and upload code by pressing Upload button
   - If everything is done correctly, you should receive a program start message in Telegram
   - Test if the lamp turns on when pressing "/Light_on"

3. Set up timer intervals
   - Use [Timer Intervals Generator](https://github.com/CodeNameHawk/TimerIntervals)
   - Upload generated schedule via Telegram bot by sending timerintervals.txt file

## Commands

- `/Light_on` - Turn on the grow light
- `/Light_off` - Turn off the grow light
- `/Status` - Check current status
- `/Timer` - View timer intervals

## Project Structure

- `LampForSeedlingsESP32FastBot.ino` - Main program file
- `LampControl.ino` - Lamp control logic
- `TelegramControl.ino` - Telegram bot handlers
- `Variables.ino` - Global variables and configurations
- `OTA.ino` - Over-the-air update functionality
- `Secrets.ino` - WiFi and Telegram credentials (create from template)

## License

This project is open source and available under the MIT License.
