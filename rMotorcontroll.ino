#include <ESP8266WiFi.h>
extern "C" {
  #include <espnow.h>
}

// Define motor control pins
const int motorA1 = D4; // Motor A - Input 1
const int motorA2 = D5; // Motor A - Input 2
const int motorB1 = D6; // Motor B - Input 1
const int motorB2 = D7; // Motor B - Input 2

// Structure to receive the data
typedef struct struct_message {
    char message[32];
    int motorCommand; // Command: 0=Stop, 1=Forward, 2=Backward, 3=Left, 4=Right
} struct_message;

struct_message myData;

// Function to stop the motor
void stopMotor() {
    digitalWrite(motorA1, LOW);
    digitalWrite(motorA2, LOW);
    digitalWrite(motorB1, LOW);
    digitalWrite(motorB2, LOW);
}

// Function to move forward
void moveForward() {
    digitalWrite(motorA1, HIGH);
    digitalWrite(motorA2, LOW);
    digitalWrite(motorB1, HIGH);
    digitalWrite(motorB2, LOW);
}

// Function to move backward
void moveBackward() {
    digitalWrite(motorA1, LOW);
    digitalWrite(motorA2, HIGH);
    digitalWrite(motorB1, LOW);
    digitalWrite(motorB2, HIGH);
}

// Function to turn left
void turnLeft() {
    digitalWrite(motorA1, LOW);
    digitalWrite(motorA2, HIGH);
    digitalWrite(motorB1, HIGH);
    digitalWrite(motorB2, LOW);
}

// Function to turn right
void turnRight() {
    digitalWrite(motorA1, HIGH);
    digitalWrite(motorA2, LOW);
    digitalWrite(motorB1, LOW);
    digitalWrite(motorB2, HIGH);
}

// Callback function when data is received
void OnDataRecv(uint8_t *mac_addr, uint8_t *incomingData, uint8_t len) {
    memcpy(&myData, incomingData, sizeof(myData));
    Serial.print("Message received: ");
    Serial.println(myData.message);

    // Control motor based on the received command
    switch (myData.motorCommand) {
        case 0:
            Serial.println("Command: Stop");
            stopMotor();
            break;
        case 1:
            Serial.println("Command: Forward");
            moveForward();
            break;
        case 2:
            Serial.println("Command: Backward");
            moveBackward();
            break;
        case 3:
            Serial.println("Command: Left");
            turnLeft();
            break;
        case 4:
            Serial.println("Command: Right");
            turnRight();
            break;
        default:
            Serial.println("Unknown Command");
            stopMotor();
            break;
    }
}

void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);

    // Set motor control pins as outputs
    pinMode(motorA1, OUTPUT);
    pinMode(motorA2, OUTPUT);
    pinMode(motorB1, OUTPUT);
    pinMode(motorB2, OUTPUT);

    // Stop the motor initially
    stopMotor();

    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // Init ESP-NOW
    if (esp_now_init() != 0) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Register the receive callback
    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
    // Nothing to do here
}
