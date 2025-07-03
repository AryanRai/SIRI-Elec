/*
 * Simple CAN Receiver for Testing - Teensy 4.1
 * 
 * This is a minimal receiver for testing CAN communication
 * Use this on a second Teensy to receive messages from the main CAN test
 * 
 * Hardware Setup: Same as can_test.ino
 */

#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

uint32_t rxCount = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  
  Serial.println("=== Simple CAN Receiver - Teensy 4.1 ===");
  Serial.println("Initializing CAN interface...");
  
  can1.begin();
  can1.setBaudRate(500000);
  can1.enableFIFO();
  can1.enableFIFOInterrupt();
  can1.onReceive(canReceive);
  
  Serial.println("Ready to receive CAN messages...\n");
}

void loop() {
  can1.events();
  
  // Show count every 5 seconds
  static uint32_t lastCount = 0;
  if (millis() - lastCount >= 5000) {
    Serial.printf("Messages received: %d\n", rxCount);
    lastCount = millis();
  }
  
  delay(10);
}

void canReceive(const CAN_message_t &msg) {
  rxCount++;
  
  Serial.printf("[%6d] ID: 0x%03X, Len: %d, Data: ", 
                millis(), msg.id, msg.len);
  
  for (int i = 0; i < msg.len; i++) {
    Serial.printf("%02X ", msg.buf[i]);
  }
  Serial.println();
} 