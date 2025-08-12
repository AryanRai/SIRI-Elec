#include <CAN.h> // Sandeep Mistry's arduino-CAN library

// Define CAN pins for ESP32 with SN65HVD230
#define CAN_RX_PIN 16 // CRX pin connected to SN65HVD230
#define CAN_TX_PIN 17 // CTX pin connected to SN65HVD230

void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  while (!Serial && millis() < 3000); // Wait for Serial, timeout after 3s
  delay(1000); // Ensure Serial stability

  Serial.println("=== ESP32 CAN Receiver (All Messages) ===");
  Serial.println("Initializing CAN interface...");

  // Set CAN pins
  CAN.setPins(CAN_RX_PIN, CAN_TX_PIN);

  // Initialize CAN bus at 500 kbps to match sender
  if (!CAN.begin(500E3)) {
    Serial.println("Failed to start CAN controller!");
    while (1); // Halt if CAN initialization fails
  }

  Serial.println("CAN interface initialized. Waiting for messages...");
}

void loop() {
  // Check for incoming CAN messages
  int packetSize = CAN.parsePacket();

  if (packetSize) {
    // Received a packet
    Serial.printf("[%6d] Received message ID: 0x%03X, Data: ", millis(), CAN.packetId());
    for (int i = 0; i < packetSize; i++) {
      Serial.printf("%02X ", CAN.read());
    }
    Serial.println();
  }
}