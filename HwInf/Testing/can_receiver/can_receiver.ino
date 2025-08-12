#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

uint32_t txCount = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  
  Serial.println("=== Simple CAN Transmitter - Teensy 4.1 ===");
  Serial.println("Initializing CAN interface...");
  
  can1.begin();
  can1.setBaudRate(500000); // Match Jetson's bitrate
  Serial.println("CAN interface initialized. Ready to send messages...\n");
}

void loop() {
  // Create a CAN message
  CAN_message_t msg;
  msg.id = 0x123; // CAN ID matching Jetson's test message
  msg.len = 4;    // Data length (4 bytes)
  msg.buf[0] = 0xAB; // Sample data
  msg.buf[1] = 0xCD;
  msg.buf[2] = 0xAB;
  msg.buf[3] = 0xCD;

  // Send the message
  if (can1.write(msg)) {
    txCount++;
    Serial.printf("[%6d] Sent message ID: 0x%03X, Data: ", millis(), msg.id);
    for (int i = 0; i < msg.len; i++) {
      Serial.printf("%02X ", msg.buf[i]);
    }
    Serial.println();
  } else {
    Serial.println("Failed to send CAN message");
  }

  // Print sent message count every 5 seconds
  static uint32_t lastCount = 0;
  if (millis() - lastCount >= 5000) {
    Serial.printf("Messages sent: %d\n", txCount);
    lastCount = millis();
  }

  delay(1000); // Send every 1 second
}