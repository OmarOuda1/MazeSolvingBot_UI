# ESP32 Maze Solving Robot Web UI

This project provides a web-based user interface for controlling and configuring an ESP32-powered maze-solving robot. The UI communicates with the robot over WebSockets, allowing for real-time control and data visualization.

## Features

*   **RC Mode:** Manually control the robot's movements using a virtual joystick.
*   **Autonomous Modes:**
    *   **Line Following:** Initiate a line-following sequence.
    *   **Maze Solving:** Command the robot to solve a maze and save the solution under a given name.
*   **Maze Management:**
    *   Load previously solved mazes onto the robot to execute a known path.
    *   Solutions are stored in the browser's `localStorage`.
*   **Live PID Tuning:** Adjust the robot's `Kp`, `Ki`, and `Kd` PID controller values and `maxSpeed` in real-time from the settings panel.
*   **Real-time Data Visualization:** A live chart displays error data from the robot's sensors, helping with PID tuning and performance monitoring.
*   **Stop Button:** An emergency stop button to halt any ongoing operation.

## Getting Started

### Prerequisites

*   PlatformIO CLI or VSCode extension for building and uploading the ESP32 firmware.
*   A web browser that supports WebSockets.

### ESP32 Setup

1.  **Dependencies:** The firmware requires the following libraries:
    *   `links2004/WebSockets`
    *   `bblanchon/ArduinoJson`
2.  **Configuration:** Open `esp32/main.cpp` and configure the Wi-Fi credentials for the Access Point.
3.  **Build and Upload:** Use PlatformIO to build the project and upload the firmware to your ESP32.

### Web UI Usage

1.  **Connect to the ESP32:** Connect your computer or mobile device to the Wi-Fi network hosted by the ESP32 (default SSID: `ESP32-Robot-Controller`).
2.  **Access the UI:** Open a web browser and navigate to the IP address of the ESP32's web server (default: `192.168.4.1`).
3.  **Control the Robot:** Use the on-screen buttons to switch between different modes and control the robot.

## WebSocket Protocol

The UI communicates with the ESP32 using a simple string-based protocol over a WebSocket connection (`ws://192.168.4.1/ws`).

### Client to Server

*   `rc:x,y`: Sends joystick coordinates for manual control.
*   `start_line_following`: Starts the line-following mode.
*   `start_solving:<name>`: Starts the maze-solving process with a given name.
*   `load_maze:<solution>`: Sends a previously saved maze solution to the robot.
*   `abort`: Stops any current action.
*   `settings:{"max_speed":50,"kp":1.0,"ki":0.5,"kd":0.2}`: Sends updated PID and speed settings as a JSON string.

### Server to Client

*   `maze_solution:<name>;<solution>`: Sent when the robot successfully solves a maze.
*   `maze_fail:<reason>`: Sent when the robot fails to solve a maze.
*   `error_data:<value>`: Streams real-time error data for the chart.
