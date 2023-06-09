//BUTTON BOX 
//USE w ProMicro
//Tested in WIN10 + Assetto Corsa
//AMSTUDIO
//20.8.17

#include <Keypad.h>
//#include "C:\coding\TUES\10 Arduino\button_box\button_box\button_box.h"
//#include <Joystick.h>

#define ENABLE_PULLUPS
#define NUMROTARIES 4
#define NUMBUTTONS 20
#define NUMROWS 4
#define NUMCOLS 5


byte buttons[NUMROWS][NUMCOLS] = {
  {97,98,99,100,101},
  {102,103,104,105,106},
  {107,108,109,110,111},
  {112,113,114,115,116}
};

struct rotariesdef {
  byte pin1;
  byte pin2;
  int ccwchar;
  int cwchar;
  volatile unsigned char state;
};

rotariesdef rotaries[NUMROTARIES] {
  {2,3,50,51,0},
  {4,5,52,53,0},
  {6,7,54,55,0},
  {8,9,56,57,0}
};

//rotariesdef rotary = {8, 9, 50, 51, 0};

int AlastState[4];
int AcurrState[4];
//int counter;

#define DIR_CCW 0x10
#define DIR_CW 0x20
#define R_START 0x0
#define HALF_STEP

#ifdef HALF_STEP
#define R_CCW_BEGIN 0x1
#define R_CW_BEGIN 0x2
#define R_START_M 0x3
#define R_CW_BEGIN_M 0x4
#define R_CCW_BEGIN_M 0x5
const unsigned char ttable[6][4] = {
  // R_START (00)
  {R_START_M,            R_CW_BEGIN,     R_CCW_BEGIN,  R_START},
  // R_CCW_BEGIN
  {R_START_M | DIR_CCW, R_START,        R_CCW_BEGIN,  R_START},
  // R_CW_BEGIN
  {R_START_M | DIR_CW,  R_CW_BEGIN,     R_START,      R_START},
  // R_START_M (11)
  {R_START_M,            R_CCW_BEGIN_M,  R_CW_BEGIN_M, R_START},
  // R_CW_BEGIN_M
  {R_START_M,            R_START_M,      R_CW_BEGIN_M, R_START | DIR_CW},
  // R_CCW_BEGIN_M
  {R_START_M,            R_CCW_BEGIN_M,  R_START_M,    R_START | DIR_CCW},
};
#else
#define R_CW_FINAL 0x1
#define R_CW_BEGIN 0x2
#define R_CW_NEXT 0x3
#define R_CCW_BEGIN 0x4
#define R_CCW_FINAL 0x5
#define R_CCW_NEXT 0x6

const unsigned char ttable[7][4] = {
  // R_START
  {R_START,    R_CW_BEGIN,  R_CCW_BEGIN, R_START},
  // R_CW_FINAL
  {R_CW_NEXT,  R_START,     R_CW_FINAL,  R_START | DIR_CW},
  // R_CW_BEGIN
  {R_CW_NEXT,  R_CW_BEGIN,  R_START,     R_START},
  // R_CW_NEXT
  {R_CW_NEXT,  R_CW_BEGIN,  R_CW_FINAL,  R_START},
  // R_CCW_BEGIN
  {R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START},
  // R_CCW_FINAL
  {R_CCW_NEXT, R_CCW_FINAL, R_START,     R_START | DIR_CCW},
  // R_CCW_NEXT
  {R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START},
};
#endif

byte rowPins[NUMROWS] = {A4,A3,A2,A1}; 
byte colPins[NUMCOLS] = {A0,13,12,11,10};

Keypad buttbx = Keypad( makeKeymap(buttons), rowPins, colPins, NUMROWS, NUMCOLS); 

/*Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_JOYSTICK, 32, 0,
  false, false, false, false, false, false,
  false, false, false, false, false);*/

void setup() {
  //pinMode(rotary.pin1, INPUT);
  //pinMode(rotary.pin2, INPUT);

  Serial.begin(9600);

  rotary_init();
}
void loop() {

  CheckAllEncoders();

  CheckAllButtons();

}

void CheckAllButtons(void) {
    if (buttbx.getKeys())
    {
       for (int i=0; i<LIST_MAX; i++)   
        {
           if ( buttbx.key[i].stateChanged )   
            {
            switch (buttbx.key[i].kstate) {  
                    case PRESSED:
                    case HOLD:{
                             //Serial.flush();
                             Serial.write(buttbx.key[i].kchar);
                             break;
                    }
                    case RELEASED:
                    case IDLE:
                    break;
            }
           }   
         }
     }
}

/*void rotary_init(){
  for(int i = 0; i < NUMROTARIES; i++){
    pinMode(rotaries[i].pin1, INPUT);
    pinMode(rotaries[i].pin2, INPUT);
    AlastState[i] = digitalRead(rotaries[i].pin1);
  }
}*/

/*void CheckAllEncoders(void){
  for(int i = 0; i < NUMROTARIES; i++){
    AcurrState[i] = digitalRead(rotaries[i].pin1);
    if (AcurrState != AlastState){
      if(digitalRead(rotaries[i].pin2) != AcurrState){
        Serial.write()
      } else {
        counter--;
      }
      Serial.print("Position: ");
      Serial.println(counter);
    }
    //Serial.print(AcurrState);
    AlastState = AcurrState;
  }
}*/

void rotary_init() {
  	for (int i=0;i<NUMROTARIES;i++) {
  	  pinMode(rotaries[i].pin1, INPUT);
    	pinMode(rotaries[i].pin2, INPUT);
    	//#ifdef ENABLE_PULLUPS
      		//digitalWrite(rotaries[i].pin1, HIGH);
      		//digitalWrite(rotaries[i].pin2, HIGH);
    	//#endif
  }
}

unsigned char rotary_process(int _i) {
   	unsigned char pinstate = (digitalRead(rotaries[_i].pin2) << 1) | digitalRead(rotaries[_i].pin1);
  	rotaries[_i].state = ttable[rotaries[_i].state & 0xf][pinstate];
  	return (rotaries[_i].state & 0x30);
}

void CheckAllEncoders(void) {
  for (int i=0;i<NUMROTARIES;i++) {
    unsigned char result = rotary_process(i);
    if (result == DIR_CCW) {
        Serial.write(rotaries[i].ccwchar);
    };
    if (result == DIR_CW) {
        Serial.write(rotaries[i].cwchar);
    };
  }
}
