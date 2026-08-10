#include <SPI.h>
#include <ACAN2517FD.h>

const byte MCP2518_CS = 10;
const byte MCP2518_INT = 2;

ACAN2517FD can(MCP2518_CS, SPI, MCP2518_INT);

// Moteus default ID: 1
// Source: 0, Dest: 1, Reply requested: yes -> ID 0x8001 (extended if needed, but <0x8000 so standard)
const uint32_t MOTEUS_QUERY_ID = 0x8001;
const uint32_t MOTEUS_REPLY_ID = 0x0100;  // Source 1, Dest 0, no reply

// Subframe for read 1 int8 register starting at 0 (Mode): type 0x11 (read int8, num=1 encoded in LSB), start=0x00, padding 0x50
uint8_t query_data[8] = {0x11, 0x00, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50};

// Subframe for write 1 int8 register: mode=0 (stopped) -> type 0x01 (write int8, num=1), start=0x00, value=0x00, padding
uint8_t stop_data[8] = {0x01, 0x00, 0x00, 0x50, 0x50, 0x50, 0x50, 0x50};

void setup() {
  Serial.begin(115200);
  while (!Serial) {}  // Wait for serial
  Serial.println("Starting Moteus C1 test...");

  SPI.begin();

  ACAN2517FDSettings settings(ACAN2517FDSettings::OSC_40MHz, 1000 * 1000, DataBitRateFactor::x1);
  const uint32_t error = can.begin(settings, [] { can.isr(); });

  if (error == 0) {
    Serial.println("✅ CAN FD OK (1Mbps nominal, 5Mbps data)");
  } else {
    Serial.print("❌ CAN Error: 0x");
    Serial.println(error, HEX);
    while (1) delay(1000);  // Halt
  }

  // Send stop command to ensure safe state
  CANFDMessage stop_frame;
  stop_frame.id = MOTEUS_QUERY_ID;
  stop_frame.len = 8;
  memcpy(stop_frame.data, stop_data, 8);
  const bool stop_tx_ok = can.tryToSend(stop_frame);
  if (stop_tx_ok) {
    Serial.println("📤 Sent STOP command to Moteus ID 1");
  } else {
    Serial.println("❌ Failed to send STOP");
    while (1) delay(1000);
  }
  delay(100);  // Brief pause
}

void loop() {
  // Send query for Mode register (0x000)
  CANFDMessage query_frame;
  query_frame.id = MOTEUS_QUERY_ID;
  query_frame.len = 8;
  memcpy(query_frame.data, query_data, 8);
  const bool tx_ok = can.tryToSend(query_frame);
  if (tx_ok) {
    Serial.println("📤 Sent Mode query to Moteus ID 1");
  } else {
    Serial.println("❌ Failed to send query");
  }

  // Try to receive reply
  CANFDMessage reply_frame;
  if (can.receive(reply_frame)) {
    if (reply_frame.id == MOTEUS_REPLY_ID) {
      Serial.print("📥 Reply received! Data: ");
      for (int i = 0; i < reply_frame.len; i++) {
        if (reply_frame.data[i] < 0x10) Serial.print("0");
        Serial.print(reply_frame.data[i], HEX);
        Serial.print(" ");
      }
      Serial.println();

      // Decode: Expect 0x21 (reply int8, num=1), 0x00 (start), then mode value (e.g., 0x00 for stopped)
      if (reply_frame.len >= 3 && reply_frame.data[0] == 0x21 && reply_frame.data[1] == 0x00) {
        uint8_t mode = reply_frame.data[2];
        Serial.print("✅ Moteus ALIVE! Mode: 0x");
        Serial.print(mode, HEX);
        if (mode == 0x00) Serial.println(" (Stopped)");
        else if (mode == 0x01) Serial.println(" (Fault - check faults!)");
        else Serial.println(" (Other mode)");
      } else {
        Serial.println("⚠️ Unexpected reply format");
      }
    } else {
      Serial.print("📥 Unexpected ID: 0x");
      Serial.println(reply_frame.id, HEX);
    }
  } else {
    Serial.println("⏳ No reply (check wiring, power, ID=1, terminations)");
  }

  delay(2000);  // Query every 2s
}