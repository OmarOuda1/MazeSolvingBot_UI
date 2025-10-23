#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

const char *ssid = "ESP32-Robot-Controller";
const char *password = "1234567-8";

WebSocketsServer webSocket = WebSocketsServer(81);

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
    // Add maze solving logic here
    // For demonstration, send a dummy solution back after a delay
    delay(5000);
    String solution = "RFRFRF"; // Dummy solution
    String message = "maze_solution:" + mazeName + ";" + solution;
    webSocket.broadcastTXT(message);
}

void handleLoadMaze(String mazeSolution) {
    Serial.printf("Load maze command received with solution: %s\n", mazeSolution.c_str());
    // Add logic to execute the maze solution
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
