/*
 * CAN Bus Communication Test for Teensy 4.1
 * 
 * Hardware Setup:
 * - TJA1050 CAN Transceiver connected to Teensy 4.1
 * - TXD (TJA1050) -> Pin 22 (CAN1_TX)
 * - RXD (TJA1050) -> Pin 23 (CAN1_RX)
 * - VCC (TJA1050) -> 5V
 * - GND (TJA1050) -> GND
 * - CANH/CANL connected to CAN bus with 120Ω termination
 * 
 * Features:
 * - Transmit CAN messages
 * - Receive CAN messages
 * - Message filtering
 * - Status monitoring
 */

#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

// Message counters
uint32_t txCount = 0;
uint32_t rxCount = 0;
uint32_t lastTxTime = 0;
uint32_t lastRxTime = 0;

// Configuration
const uint32_t CAN_BAUD_RATE = 500000;  // 500 kbps
const uint16_t TX_INTERVAL = 1000;      // Transmit every 1000ms

// Message IDs
const uint32_t HEARTBEAT_ID = 0x100;
const uint32_t SENSOR_DATA_ID = 0x200;
const uint32_t COMMAND_ID = 0x300;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10); // Wait for Serial or timeout after 3 seconds
  }
  
  Serial.println("=== CAN Bus Test - Teensy 4.1 ===");
  Serial.println("Initializing CAN interface...");
  
  // Initialize CAN bus
  can1.begin();
  can1.setBaudRate(CAN_BAUD_RATE);
  
  // Set up message filters (optional - receives all by default)
  // Uncomment to filter specific message IDs
  // can1.setMBFilter(ACCEPT_ALL);
  // can1.setMBFilter(MB0, HEARTBEAT_ID);
  // can1.setMBFilter(MB1, SENSOR_DATA_ID);
  
  // Enable interrupts for receiving
  can1.enableFIFO();
  can1.enableFIFOInterrupt();
  can1.onReceive(canSniff); // Set callback function
  
  Serial.println("CAN interface initialized successfully!");
  Serial.printf("Baud Rate: %d bps\n", CAN_BAUD_RATE);
  Serial.println("Commands:");
  Serial.println("  'h' - Send heartbeat message");
  Serial.println("  's' - Send sensor data");
  Serial.println("  'c' - Send command message");
  Serial.println("  'i' - Show interface statistics");
  Serial.println("Listening for messages...\n");
}

void loop() {
  // Handle serial commands
  handleSerialCommands();
  
  // Automatic heartbeat transmission
  if (millis() - lastTxTime >= TX_INTERVAL) {
    sendHeartbeat();
    lastTxTime = millis();
  }
  
  // Process CAN events
  can1.events();
  
  // Show periodic statistics
  static uint32_t lastStatsTime = 0;
  if (millis() - lastStatsTime >= 10000) { // Every 10 seconds
    showStatistics();
    lastStatsTime = millis();
  }
  
  delay(10); // Small delay to prevent excessive CPU usage
}

// CAN message receive callback
void canSniff(const CAN_message_t &msg) {
  rxCount++;
  lastRxTime = millis();
  
  Serial.printf("[RX] ID: 0x%03X, DLC: %d, Data: ", msg.id, msg.len);
  
  // Print data bytes
  for (int i = 0; i < msg.len; i++) {
    Serial.printf("%02X ", msg.buf[i]);
  }
  
  // Decode message based on ID
  Serial.print("| ");
  decodeMessage(msg);
  
  Serial.println();
}

void decodeMessage(const CAN_message_t &msg) {
  switch (msg.id) {
    case HEARTBEAT_ID:
      Serial.printf("HEARTBEAT - Counter: %d", 
                   (msg.buf[0] << 8) | msg.buf[1]);
      break;
      
    case SENSOR_DATA_ID:
      if (msg.len >= 4) {
        int16_t temp = (msg.buf[0] << 8) | msg.buf[1];
        uint16_t humidity = (msg.buf[2] << 8) | msg.buf[3];
        Serial.printf("SENSOR - Temp: %d°C, Humidity: %d%%", temp, humidity);
      }
      break;
      
    case COMMAND_ID:
      Serial.printf("COMMAND - Code: 0x%02X, Value: %d", 
                   msg.buf[0], msg.buf[1]);
      break;
      
    default:
      Serial.printf("UNKNOWN MESSAGE TYPE");
      break;
  }
}

void handleSerialCommands() {
  if (Serial.available()) {
    char cmd = Serial.read();
    
    switch (cmd) {
      case 'h':
        sendHeartbeat();
        break;
        
      case 's':
        sendSensorData();
        break;
        
      case 'c':
        sendCommand();
        break;
        
      case 'i':
        showStatistics();
        break;
        
      case '\n':
      case '\r':
        // Ignore newlines
        break;
        
      default:
        Serial.printf("Unknown command: '%c'\n", cmd);
        break;
    }
  }
}

void sendHeartbeat() {
  CAN_message_t msg;
  msg.id = HEARTBEAT_ID;
  msg.len = 8;
  
  // Pack counter in first 2 bytes
  msg.buf[0] = (txCount >> 8) & 0xFF;
  msg.buf[1] = txCount & 0xFF;
  
  // Add timestamp in next 4 bytes
  uint32_t timestamp = millis();
  msg.buf[2] = (timestamp >> 24) & 0xFF;
  msg.buf[3] = (timestamp >> 16) & 0xFF;
  msg.buf[4] = (timestamp >> 8) & 0xFF;
  msg.buf[5] = timestamp & 0xFF;
  
  // Status bytes
  msg.buf[6] = 0xAA; // Status indicator
  msg.buf[7] = 0x55; // Checksum placeholder
  
  if (can1.write(msg)) {
    txCount++;
    Serial.printf("[TX] Heartbeat sent - Counter: %d\n", txCount);
  } else {
    Serial.println("[TX] Failed to send heartbeat!");
  }
}

void sendSensorData() {
  CAN_message_t msg;
  msg.id = SENSOR_DATA_ID;
  msg.len = 6;
  
  // Simulate sensor readings
  int16_t temperature = random(-40, 85);  // -40°C to 85°C
  uint16_t humidity = random(0, 100);     // 0% to 100%
  uint16_t pressure = random(800, 1200);  // 800 to 1200 hPa
  
  msg.buf[0] = (temperature >> 8) & 0xFF;
  msg.buf[1] = temperature & 0xFF;
  msg.buf[2] = (humidity >> 8) & 0xFF;
  msg.buf[3] = humidity & 0xFF;
  msg.buf[4] = (pressure >> 8) & 0xFF;
  msg.buf[5] = pressure & 0xFF;
  
  if (can1.write(msg)) {
    txCount++;
    Serial.printf("[TX] Sensor data sent - Temp: %d°C, Humidity: %d%%, Pressure: %d hPa\n", 
                  temperature, humidity, pressure);
  } else {
    Serial.println("[TX] Failed to send sensor data!");
  }
}

void sendCommand() {
  CAN_message_t msg;
  msg.id = COMMAND_ID;
  msg.len = 4;
  
  // Example command
  msg.buf[0] = 0x01;      // Command code
  msg.buf[1] = 0xFF;      // Parameter 1
  msg.buf[2] = 0x00;      // Parameter 2
  msg.buf[3] = 0xAB;      // Checksum
  
  if (can1.write(msg)) {
    txCount++;
    Serial.printf("[TX] Command sent - Code: 0x%02X\n", msg.buf[0]);
  } else {
    Serial.println("[TX] Failed to send command!");
  }
}

void showStatistics() {
  Serial.println("\n=== CAN Interface Statistics ===");
  Serial.printf("Messages Transmitted: %d\n", txCount);
  Serial.printf("Messages Received: %d\n", rxCount);
  Serial.printf("Last TX: %d ms ago\n", millis() - lastTxTime);
  
  if (rxCount > 0) {
    Serial.printf("Last RX: %d ms ago\n", millis() - lastRxTime);
  } else {
    Serial.println("Last RX: Never");
  }
  
  Serial.printf("Uptime: %d seconds\n", millis() / 1000);
  Serial.println("================================\n");
}
