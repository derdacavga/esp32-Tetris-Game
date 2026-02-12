# 🕹️ ESP32 Handheld Tetris

A DIY handheld Tetris game built with an **ESP32** and an **ILI9341 TFT Display**. This project is a step back to the classics and serves as a foundational project for my ultimate goal: building a fully custom handheld game console (DIY PSP).

## 📖 Overview

This repository contains the source code and wiring guide to build a fully functional Tetris clone. It features classic gameplay mechanics, a scoring system, increasing difficulty levels, and sound effects.

The code is designed to be beginner-friendly while offering a robust example of game development on embedded systems.

[![Project Review](https://img.youtube.com/vi/fHoHQSKkdiA/0.jpg)](https://youtu.be/fHoHQSKkdiA)

## ✨ Features

* **Classic Gameplay:** Full Tetris logic (rotate, slide, fast drop).
* **Scoring System:** +100 points per cleared line.
* **Progressive Difficulty:** Game speed increases every 1000 points.
* **Next Block Preview:** See the upcoming piece on the side of the screen.
* **Sound Effects:** Audio feedback for dropping blocks and clearing lines via a passive buzzer.
* **Optimized Graphics:** Uses the `TFT_eSPI` library for fast rendering.

## 📦 Hardware Required

* **Microcontroller:** ESP32-S3 Development Board (or any standard ESP32 board).
* **Display:** ILI9341 TFT Display (320x240 resolution).
* **Controls:** 4x Tactile Push Buttons.
* **Audio:** Passive Buzzer.
* **Misc:** Breadboard, Jumper Wires.

## 🔌 Wiring Guide

### Display (ILI9341) - SPI Connection
*Note: The touch screen pins are not used in this project.*

| ILI9341 Pin | ESP32 GPIO | Description |
| :--- | :--- | :--- |
| **VCC** | 3.3V | Power |
| **GND** | GND | Ground |
| **CS** | GPIO 4 | Chip Select |
| **RESET** | GPIO 9 | Reset |
| **DC** | GPIO 15 | Data/Command |
| **MOSI** | GPIO 11 | SPI MOSI |
| **SCK** | GPIO 12 | SPI Clock |
| **LED** | 3.3V | Backlight |
| **MISO** | GPIO 13 | SPI MISO |

### Controls & Audio
I use the internal `INPUT_PULLUP` method, so no external resistors are required for the buttons.

| Component | Function | ESP32 GPIO | Connection |
| :--- | :--- | :--- | :--- |
| **Button 1** | Rotate | GPIO 16 | Pin to GPIO, other to GND |
| **Button 2** | Fast Drop | GPIO 17 | Pin to GPIO, other to GND |
| **Button 3** | Move Left | GPIO 18 | Pin to GPIO, other to GND |
| **Button 4** | Move Right | GPIO 8 | Pin to GPIO, other to GND |
| **Buzzer** | Sound | GPIO 14 | Positive to GPIO, Negative to GND |

---

## 🛠️ Installation & Setup

### 1. Arduino IDE Setup
1.  Install the **Arduino IDE**.
2.  Add the ESP32 Board Manager URL in `File > Preferences`.
3.  Go to `Tools > Board > Boards Manager`, search for **ESP32**, and install it.

### 2. Library Installation
This project relies on the **TFT_eSPI** library for display handling.
1.  Open the Library Manager in Arduino IDE.
2.  Search for `TFT_eSPI` and install it.

### 3. Library Configuration (CRITICAL STEP) ⚠️
To make the display work, you **must** configure the library files to match the wiring above.

1.  Navigate to your Arduino libraries folder: `Documents/Arduino/libraries/TFT_eSPI`.
2.  **Edit `User_Setup_Select.h`:**
    * Open the file and ensure the line user_setup.h is uncommented.
    * Comment out other drivers.
3.  **Edit `User_Setup.h`:**
    * Open the file and define the pins exactly as listed in the **Wiring Guide** above.
    * *Note: If you see a white screen after uploading, your pin definitions in this file are likely incorrect.*

### 4. Upload
1.  Open the `.ino` file from this repository.
2.  Select your Board and Port in `Tools`.
3.  Click **Upload**.

---

## 🎮 How to Play

* **Rotate:** Button 1 (GPIO 16)
* **Fast Drop:** Hold Button 2 (GPIO 17)
* **Slide Left:** Button 3 (GPIO 18)
* **Slide Right:** Button 4 (GPIO 8)

## 🤝 Support & Community

If you enjoyed this project, consider supporting the channel!

* **YouTube:** [https://www.youtube.com/@DsnIndustries/videos]
* **Patreon:** [https://www.patreon.com/c/dsnIndustries]

**Future Plans:**
I am working on a dedicated handheld game console with a custom PCB and game engine (like a DIY PSP). Stay tuned for updates!

## Games
* **Maze Escape:** https://play.google.com/store/apps/details?id=com.DsnMechanics.MazeEscape
* **Air Hockey:** https://play.google.com/store/apps/details?id=com.DsnMechanics.AirHockey
* **Click Challenge:** https://play.google.com/store/apps/details?id=com.DsNMechanics.ClickChallenge
* **Flying Triangels:** https://play.google.com/store/apps/details?id=com.DsnMechanics.Triangle
* **SkyScrapper:** https://play.google.com/store/apps/details?id=com.DsnMechanics.SkyScraper
