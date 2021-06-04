#include <NintendoExtensionCtrl.h>

#define OSD_LATCH 12   // PIN 16
#define OSD_CLOCK 9     // PIN 13
#define OSD_DATA 8    // PIN 12
#define OSD_TOGGLE 10   // PIN 14

#define GB_A      4
#define GB_B      3
#define GB_UP     14
#define GB_DOWN   15
#define GB_LEFT   16
#define GB_RIGHT  17
#define GB_L      0
#define GB_R      1
#define GB_START  5
#define GB_SELECT 6

int btn_pins[12] = {
  GB_B, 99, GB_SELECT, GB_START, GB_UP, GB_DOWN, GB_LEFT, GB_RIGHT, GB_A, 99, GB_L, GB_R
};

bool osd_toggle = false;
bool osd_toggled = false;

SNESMiniController snes;

void setup() {
  pinMode(OSD_CLOCK, OUTPUT);
  pinMode(OSD_DATA, OUTPUT);
  pinMode(OSD_TOGGLE, OUTPUT);
  pinMode(OSD_LATCH, OUTPUT);

  digitalWrite(OSD_CLOCK, LOW);
  digitalWrite(OSD_DATA, LOW);
  digitalWrite(OSD_LATCH, LOW);
  digitalWrite(OSD_TOGGLE, osd_toggle);

  for(int i=0; i<12; i++){
    if(btn_pins[i] == 99) continue;
    pinMode(btn_pins[i], OUTPUT);
    digitalWrite(btn_pins[i], 1);
  }

  snes.begin();

  while (!snes.connect()) {
    delay(1000);
  }
}

// PAYLOAD => UP, DOWN, LEFT, RIGHT, A, B
bool payload[6] = {0,0,0,0,0,0};
bool old_payload[6] = {0,0,0,0,0,0};
bool controller_payload[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int test = 0;

bool payloadChange(){
  for(int i=0; i<6; i++){
    if(payload[i]!=old_payload[i]) return true;
  }
  return false;
}

void outputOSD(){
  digitalWrite(OSD_LATCH, HIGH);
  delayMicroseconds(1 );
  digitalWrite(OSD_LATCH, LOW);
  for(int i=0; i<6; i++){
    delayMicroseconds(1);
    digitalWrite(OSD_DATA, payload[i]);
    digitalWrite(OSD_CLOCK, HIGH);
    delayMicroseconds(1);
    digitalWrite(OSD_CLOCK, LOW);
    old_payload[i] = payload[i];
  }
  digitalWrite(OSD_DATA, LOW);
}

void outputGBA(){
  for(int i=0; i<12; i++){
    if(btn_pins[i] == 99) continue;
    digitalWrite(btn_pins[i], !controller_payload[i]);
  }
}

void readController(){
  boolean success = snes.update();
  if(!success){
    for(int i=0; i<6; i++){
      payload[i] = 0;
    }
  }
  controller_payload[0] = snes.buttonB();
  controller_payload[1] = snes.buttonY();
  controller_payload[2] = snes.buttonSelect();
  controller_payload[3] = snes.buttonStart();
  controller_payload[4] = snes.dpadUp();
  controller_payload[5] = snes.dpadDown();
  controller_payload[6] = snes.dpadLeft();
  controller_payload[7] = snes.dpadRight();
  controller_payload[8] = snes.buttonA();
  controller_payload[9] = snes.buttonX();
  controller_payload[10] = snes.buttonL();
  controller_payload[11] = snes.buttonR();

  payload[0] = controller_payload[0];
  payload[1] = controller_payload[8];
  payload[2] = controller_payload[7];
  payload[3] = controller_payload[6];
  payload[4] = controller_payload[5];
  payload[5] = controller_payload[4];

  if(controller_payload[10] && controller_payload[11] && controller_payload[3]){
    if(!osd_toggled){
      osd_toggle = !osd_toggle;
      digitalWrite(OSD_TOGGLE, osd_toggle);
      osd_toggled = true;
    }
  }else{
    osd_toggled = false;
  }
}

void loop() {
  readController();
  if(osd_toggle){
    outputOSD(); 
  }else{
    outputGBA();
  }
}