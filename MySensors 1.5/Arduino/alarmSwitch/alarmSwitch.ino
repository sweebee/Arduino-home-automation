#include <Keypad.h>
#include <Password.h>
#include <MySensor.h>
#include <SPI.h>

#define NODE_ID 53
#define CHILD_ID 1
#define LED_OFF A0
#define LED_ON A1
#define ON 1
#define OFF 0

Password password = Password( "1463" );
long previousMillis = 0;
long interval = 1000;
boolean ALARM;
int LED_STATE;
int FALSE = 0;
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {8, 7, 6, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 3, 2}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
MySensor node;
MyMessage msg(CHILD_ID, V_LIGHT);

void setup() {
  node.begin(incomingMessage, NODE_ID, false);
  node.present(CHILD_ID, S_LIGHT);
  pinMode(LED_ON,OUTPUT);
  pinMode(LED_OFF,OUTPUT);
  digitalWrite(LED_ON, node.loadState(0));
  digitalWrite(LED_OFF, node.loadState(1));
  ALARM = node.loadState(2);
  keypad.addEventListener(keypadEvent); //add an event listener for this keypad
}

void loop() {
  node.process();
  keypad.getKey();
  blink();
}

void incomingMessage(const MyMessage &message) {
  if (message.type==V_LIGHT && message.sensor == CHILD_ID) {
     turnAlarm(message.getBool());
   } 
}

void keypadEvent(KeypadEvent eKey){
  switch (keypad.getState()){
    case PRESSED:
	Serial.print("Pressed: ");
	Serial.println(eKey);
	switch (eKey){
	  case '*': checkPassword(); password.reset(); break;
	  case '#': password.reset(); break;
	  default: password.append(eKey);
     }
  }
}

void checkPassword(){
  if (password.evaluate()){
    Serial.println("Success");
    FALSE = 0;
    if(ALARM == ON) {
      ALARM = OFF;
    } else {
      ALARM = ON;
    }
    turnAlarm(ALARM);
    node.send(msg.set(ALARM));
  }else{
    Serial.println("Wrong");
    FALSE++;
    if(FALSE > 5) {
      Serial.println("To many tries");
      digitalWrite(LED_ON, LOW);
      digitalWrite(LED_OFF, LOW);
      delay(200);
      for (int i=0; i <= 3; i++){
        digitalWrite(LED_ON, HIGH);
        delay(200);
        digitalWrite(LED_ON, LOW);
        delay(200);
     }
      setLed();
    } else {
      digitalWrite(LED_ON, LOW);
      digitalWrite(LED_OFF, LOW);
      delay(50);
      for (int i=0; i <= 3; i++){
        digitalWrite(LED_ON, HIGH);
        delay(50);
        digitalWrite(LED_ON, LOW);
        delay(50);
     }
      setLed();
    }
  } 
}

void turnAlarm(boolean STATE) {
     Serial.print("Alarm: ");
     Serial.println(STATE);
     ALARM = STATE;
     node.saveState(2, ALARM);
     setLed();
}

void setLed() {
  if(ALARM == true) {
       digitalWrite(LED_OFF, LOW);
       node.saveState(0, 1);
       node.saveState(1, 0);
     } else {
       digitalWrite(LED_OFF, HIGH);
       digitalWrite(LED_ON, LOW);
       node.saveState(0, 0);
       node.saveState(1, 1);
     }
}

void blink() {
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    if(ALARM == true) {
      if(LED_STATE == 0){
        LED_STATE = 1;
      } else {
        LED_STATE = 0;
      }
      digitalWrite(LED_ON, LED_STATE);
    }
  }
}
