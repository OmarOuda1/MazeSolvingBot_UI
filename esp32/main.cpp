#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

const char *ssid = "ESP32-Robot-Controller";
const char *password = "1234567-8";

WebSocketsServer webSocket = WebSocketsServer(81);
volatile bool isSolving = false;

// Default settings
int maxSpeed = 50;
float Kp = 1.0, Ki = 0.5, Kd = 0.2;

void handleSettings(String payload) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    maxSpeed = doc["max_speed"];
    Kp = doc["kp"];
    Ki = doc["ki"];
    Kd = doc["kd"];

    Serial.printf("Settings updated: max_speed=%d, Kp=%.2f, Ki=%.2f, Kd=%.2f\n", maxSpeed, Kp, Ki, Kd);
}

void handleRC(String payload) {
    int commaIndex = payload.indexOf(',');
    if (commaIndex != -1) {
        int x = payload.substring(0, commaIndex).toInt();
        int y = payload.substring(commaIndex + 1).toInt();
        Serial.printf("RC command: x=%d, y=%d\n", x, y);
        // Add motor control logic here
    }
}

void handleStartLineFollowing() {
    Serial.println("Start line following command received");
    // Add line following logic here
}

void handleStartSolving(String mazeName) {
    Serial.printf("Start solving command received for maze: %s\n", mazeName.c_str());
    isSolving = true;

    // Simulate a long-running maze-solving process
    for (int i = 0; i < 10; i++) {
        if (!isSolving) {
            Serial.println("Maze solving aborted!");
            webSocket.broadcastTXT("maze_fail:aborted by user");
            return;
        }
        Serial.printf("Solving step %d...\n", i + 1);
        delay(1000);
    }

    if (isSolving) {
        String solution = "RFRFRF"; // Dummy solution
        String message = "maze_solution:" + mazeName + ";" + solution;
        webSocket.broadcastTXT(message);
    }
    isSolving = false;
}

void handleLoadMaze(String mazeSolution) {
    Serial.printf("Load maze command received with solution: %s\n", mazeSolution.c_str());
    // Add logic to execute the maze solution
}

void handleAbort() {
    Serial.println("Abort command received");
    isSolving = false;
    // Add any other necessary cleanup or motor stop logic here
}

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
            break;
        }
        case WStype_TEXT: {
            Serial.printf("[%u] get Text: %s\n", num, payload);
            String message = String((char *)payload);
            int colonIndex = message.indexOf(':');
            String command = (colonIndex != -1) ? message.substring(0, colonIndex) : message;
            String data = (colonIndex != -1) ? message.substring(colonIndex + 1) : "";

            if (command == "rc") {
                handleRC(data);
            } else if (command == "start_line_following") {
                handleStartLineFollowing();
            } else if (command == "start_solving") {
                handleStartSolving(data);
            } else if (command == "load_maze") {
                handleLoadMaze(data);
            } else if (command == "abort") {
                handleAbort();
            } else if (command == "settings") {
                handleSettings(data);
            } else {
                Serial.println("Unknown command received");
            }
            break;
        }
        case WStype_BIN:
        case WStype_ERROR:
        case WStype_FRAGMENT_TEXT_START:
        case WStype_FRAGMENT_BIN_START:
        case WStype_FRAGMENT:
        case WStype_FRAGMENT_FIN:
            break;
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.print("Setting up AP...");

    WiFi.softAP(ssid, password);

    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);

    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);
}

void loop() {
    webSocket.loop();
}
