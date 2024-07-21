# 🌟 cydOS

cydOS is a versatile and user-friendly graphical user interface (GUI) designed specifically for the ESP32-2432S028R device, also known as CYD. This project aggregates various applications and includes several standalone functionalities, offering a comprehensive environment for managing and running apps directly from an SD card.

## 🚀 Features

### 🖱️ Touchscreen Interface
cydOS utilizes the XPT2046 touchscreen controller to provide a responsive but not so accurate touch interface, so there are big buttons to make it finger-friendly (sorry, not for the keyboard).

### 🏠 Home Screen
The home screen is the central hub of cydOS, offering quick access to essential functions such as settings and the application launcher. It displays information and provides an intuitive navigation experience. I'm thinking about various UX solutions and customization, but for now, enjoy iPhone OS 1.0 vibes.

### 💾 Application Flasher
The application launcher scans the SD card for available binaries in the `apps` directory and presents them in a user-friendly interface. Users can easily select and flash applications from the SD card.

### ⚙ Settings
The settings interface allows users to adjust various system configurations. This module is designed for easy extensibility, enabling developers to add new settings options as needed.

### 📂 File Browser
Navigate the SD card directly from the UI, it enables also for new directories creation!

## 🛠️ Installation
For now, I'll publish only the source code because everything is unstable and there is a lot of work to do, so no real reason to install it.

### Prerequisites
- ESP32-2432S028R device (CYD)
- VSCODE IDE with PlatformIO
