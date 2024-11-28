#include <ESP8266WiFi.h>
extern "C" {
  #include <espnow.h>
}

// MAC Address of the receiver NodeMCU
uint8_t broadcastAddress[] = {0x8C, 0xAA, 0xB5, 0x16, 0x05, 0x92}; // Replace with actual receiver MAC address

typedef struct struct_message {
    char message[32];
    int motorCommand; // Command: 0=Stop, 1=Forward, 2=Reverse, 3=Left, 4=Right
} struct_message;

struct_message myData;

const int buttonPins[4] = {D3, D4, D5, D6}; // GPIO pins for 4 buttons
int lastButtonStates[4] = {HIGH, HIGH, HIGH, HIGH}; // Initial states of the buttons

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
    Serial.print("Last Packet Send Status: ");
    Serial.println(sendStatus == 0 ? "Delivery Success" : "Delivery Fail");
}

void setup() {
    Serial.begin(115200);
    Serial.println();

    // Initialize button pins as input with pull-up
    for (int i = 0; i < 4; i++) {
        pinMode(buttonPins[i], INPUT_PULLUP);
    }

    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // Init ESP-NOW
    if (esp_now_init() != 0) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Register the send callback
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    esp_now_register_send_cb(OnDataSent);

    // Add peer
    esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {
    for (int i = 0; i < 4; i++) {
        int buttonState = digitalRead(buttonPins[i]);

        // Check if button state has changed
        if (buttonState != lastButtonStates[i]) {
            lastButtonStates[i] = buttonState;

            // Prepare the message
            if (buttonState == LOW) { // Button pressed
                myData.motorCommand = i + 1; // Map buttons to motor commands
                sprintf(myData.message, "Motor Command: %d", myData.motorCommand);
            } else { // Button released
                myData.motorCommand = 0; // Stop
                sprintf(myData.message, "Motor Command: Stop");
            }

            // Send the message
            esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));

            // Print status to Serial Monitor
            Serial.println(myData.message);
        }
    }

    delay(100); // Debounce delay
}
