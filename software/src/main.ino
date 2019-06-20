#include <Arduino.h>
#include <Homie.h>

#define NEST_Y1 D5 //#
#define NEST_Y2 D6 //#
#define NEST_G D7 //D3
#define NEST_S D2
#define NEST_W1 D1 //#
#define NEST_W2 D0


int lastState_W1 = -1;
int lastState_W2 = -1;
int lastState_Y1 = -1;
int lastState_Y2 = -1;
int lastState_S = -1;
int lastState_G = -1;

bool state_w1 = false;
bool state_w2 = false;
bool state_y1 = false;
bool state_y2 = false;
bool state_s = false;
bool state_g = false;

HomieNode nestNode("nest", "node");

bool resetHandler(String value) {
  //remove config =)
  SPIFFS.remove("/homie/config.json");

  Homie.setNodeProperty(nestNode, "reset", value, false);
  return true;
}

void setupHandler() {
  pinMode(NEST_W1, INPUT_PULLUP);
  pinMode(NEST_W2, INPUT_PULLUP);
  pinMode(NEST_Y1, INPUT_PULLUP);
  pinMode(NEST_Y2, INPUT_PULLUP);
  pinMode(NEST_S, INPUT_PULLUP);
  pinMode(NEST_G, INPUT_PULLUP);

  nestNode.subscribe("reset", resetHandler);
}

void loopHandler() {
}

void setup() {
  SPIFFS.begin();
  Homie.disableResetTrigger();

  Homie.setFirmware("nest-firmware", "1.0.0");
  Homie.setSetupFunction(setupHandler);
  Homie.setLoopFunction(loopHandler);
  Homie.registerNode(nestNode);
  Homie.setup();
}

void read_pin(int pin, bool &pin_state) {
  int state = digitalRead(pin);
  // Serial.print(pin);
  // Serial.print("=");
  // Serial.println(state);
  if (state == 0) {
    pin_state = true;
  }
}

void send_state(int state, int& lastState, const String name) {  
  if (state != lastState) {
    Serial.print(name);
    Serial.print(" state changed from ");
    Serial.print(lastState);
    Serial.print(" to ");
    Serial.println(state);

    Homie.setNodeProperty(nestNode, name, String(state), true);
    lastState = state;
  }
}

void measure_pins() {
  state_w1 = false;
  state_w2 = false;
  state_y1 = false;
  state_y2 = false;
  state_s = false;
  state_g = false;
  for(int i = 0; i<250; i++) {
    read_pin(NEST_W1, state_w1);
    read_pin(NEST_W2, state_w2);
    read_pin(NEST_Y1, state_y1);
    read_pin(NEST_Y2, state_y2);
    read_pin(NEST_S, state_s);
    read_pin(NEST_G, state_g);
    Homie.loop();
    delay(1);
  }
  send_state(state_w1, lastState_W1, "w1");
  send_state(state_w2, lastState_W2, "w2");
  send_state(state_y1, lastState_Y1, "y1");
  send_state(state_y2, lastState_Y2, "y2");
  send_state(state_s, lastState_S, "s");
  send_state(state_g, lastState_G, "g");
}

void loop() {
    measure_pins();
}