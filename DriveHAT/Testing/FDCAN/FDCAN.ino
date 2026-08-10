#include <SPI.h>
#include <ACAN2517FD.h>

const byte MCP2518_CS = 10;
const byte MCP2518_INT = 2;

ACAN2517FD can (MCP2518_CS, SPI, MCP2518_INT);

void setup() {
  Serial.begin(115200);
  SPI.begin();

  ACAN2517FDSettings settings(ACAN2517FDSettings::OSC_40MHz, 1000 * 1000, DataBitRateFactor::x1);
  uint32_t error = can.begin(settings, [] { can.isr(); });

  if (error == 0) Serial.println("✅ MCP2518FD OK");
  else {
    Serial.print("❌ Error: 0x");
    Serial.println(error, HEX);
  }
}

void loop() {
  CANFDMessage frame;
  frame.id = 0x123;
  frame.len = 8;
  frame.data[0] = 0xAB;
  can.tryToSend(frame);
  delay(1000);
}
