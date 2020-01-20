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
const byte BYTES_PER_LED = 3;


// PongWall Serial Protocol stuff
const byte CONTROL_BYTE = 0x10;

// Control Modifiers
const byte LITERAL_CONTROL_BYTE_MODIFIER = CONTROL_BYTE;
const byte START_MARKER = 0x01;
const byte END_MARKER = 0x02;

void addByte(byte rb) {
  static enum colors {
    red,
    green,
    blue,
    numColors,
  } color = red;
  static int ledNdx = 0;

  ledNdx = (ledNdx >= 100) ? 0 : ledNdx;

  // leds[ledNdx][color] = rb;
  // leds[ledNdx] = CRGB(0x05, 0x05, 0x05);


  color = (color + 1) % numColors;
  // if color is now red, it is time for a new LED
  if (color == red) {
    ledNdx++;
  }
  // ledNdx = (color == red) ? ledNdx++ : ledNdx;
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
      addByte(rb);
      packet_state = DATA;
      break;

    case START_MARKER:
      packet_state = DATA;
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
      break;
    }
    else {
      addByte(rb);
      break;
    }
  }
}

void setup() {
  Serial.begin(115200);

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
