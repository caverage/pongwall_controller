/* Pongwall Controller

Globals:
  CONTROL_BYTE: Control byte as per PongWall Protocol Specification
  LITERAL_CONTROL_BYTE_MODIFIER: Byte to send a literal control byte in the data
    portion of the packet as per PongWall Protocol Specification
  START_MARKER: Control modifier for starting a packet as per PongWall Protocol
    Specification
  END_MARKER: Control modifier for ending a packet as per PongWall Protocol
    Specification
*/

// FastLED stuff
#include <FastLED.h>

#define DATA_PIN 2
#define NUM_LEDS 100

CRGB leds[NUM_LEDS];
#define BYTES_PER_LED 3


// PongWall Serial Protocol stuff
#define CONTROL_BYTE 0x10

// Control Modifiers
#define LITERAL_CONTROL_BYTE_MODIFIER CONTROL_BYTE
#define START_MARKER 0x01
#define END_MARKER 0x02

void addByte(boolean newPacket, byte rb) {
  static enum colors {
    red,
    green,
    blue,
    numColors,
  } color = red;
  static int ledNdx = 0;

  if (newPacket == true) {
    ledNdx = 0;
    color = red;
  }
  else {
    leds[ledNdx][color] = rb;

    color = (color + 1) % numColors;
    // if color is now red, it is time for a new LED
    if (color == red) {
      ledNdx++;
    }
  }
}

void displayFrame() {
  FastLED.show();
  Serial.println("ACK");
}

void processByte(byte rb) {
  static enum packet_states {
    CONTROL,
    CONTROL_MODIFIER,
    DATA,
  } packet_state;

  switch (packet_state) {
  // waiting for a control byte
  case CONTROL:
    packet_state = (rb == CONTROL_BYTE) ? CONTROL_MODIFIER : CONTROL_BYTE;
  break;

  // take action according to control modifier
  case CONTROL_MODIFIER:
    switch (rb) {
    case LITERAL_CONTROL_BYTE_MODIFIER:
      addByte(false, rb);
      packet_state = DATA;
    break;

    case START_MARKER:
      packet_state = DATA;
      addByte(true, 0x00);
    break;

    case END_MARKER:
      displayFrame();
      packet_state = CONTROL;
    break;
    }
    break;

  case DATA:
    if (rb == CONTROL_BYTE) {
      packet_state = CONTROL_MODIFIER;
    }
    else {
      addByte(false, rb);
    }
  break;
  }
}

void setup() {
  Serial.begin(9600);

  // initialize FastLED
  FastLED.addLeds<WS2811, DATA_PIN>(leds, NUM_LEDS);

  // blank out display
  fill_solid(leds, 100, CRGB::Black);
  FastLED.show();
}

void loop() {
  byte rb; // read byte
  while (Serial.available()) {
    rb = Serial.read();

    processByte(rb);
  }
}
