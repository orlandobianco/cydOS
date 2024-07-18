# cydOS

cydOS is a versatile and user-friendly graphical user interface (GUI) designed specifically for the ESP32-2432S028R device, also known as CYD. This project aggregates various applications and includes several standalone functionalities, offering a comprehensive environment for managing and running apps directly from an SD card.

## Features

### 1. Touchscreen Interface
cydOS utilizes the XPT2046 touchscreen controller to provide a responsive and accurate touch interface. The system includes automatic calibration to ensure precise touch detection and smooth interaction.

### 2. Home Screen
The home screen is the central hub of cydOS, offering quick access to essential functions such as settings and the application launcher. It displays crucial information and provides an intuitive navigation experience.

### 3. Application Launcher
The application launcher scans the SD card for available applications in the `apps` directory and presents them in a user-friendly interface. If no applications are found, it displays a clear message indicating "NO APPS FOUND." Users can easily select and launch applications from the SD card.

### 4. Settings
The settings interface allows users to adjust various system configurations. This module is designed for easy extensibility, enabling developers to add new settings options as needed.

## Installation

### Prerequisites
- ESP32-2432S028R device (CYD)
- VSCODE IDE with PlatformIO

### Setup

1. **Clone the Repository**
   ```sh
   git clone https://github.com/orlandobianco/cydOS.git

