#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Keypad.h>
#include <Password.h>
#include <MySensor.h>
#include <SPI.h>


LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address


const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {8, 7, 6, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 3, 2}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


Password password = Password( "1463" ); // Password


#define NODE_ID 53
MySensor node;
MyMessage msg(1, V_LIGHT);    // Alarm switch
MyMessage msg2(2, V_TRIPPED); // Sends if password was wrong after 5 time
MyMessage msg3(3, V_LIGHT);   // Buzzer
#define ON 1
#define OFF 0
#define BEEPER A0             // Buzzer pin



long previousMillis = 0;
unsigned long currentMillis = millis();
long interval = 10000; // Turn the backlight off after ... seconds when nothing is touched
boolean ALARM;
boolean FIRST_INPUT = true;
int TRIES = 0;
boolean LCD_LED;

void setup() {
  node.begin(incomingMessage, NODE_ID, false);
  lcd.begin(16, 2);
  lcd.print("Opstarten...");
  pinMode(BEEPER, OUTPUT);
  node.present(1, S_LIGHT);
  node.present(2, S_MOTION);
  node.present(3, S_LIGHT);
  ALARM = node.loadState(0);
  keypad.addEventListener(keypadEvent); //add an event listener for this keypad
  lcd.clear();
  if (ALARM == ON) {
    lcd.print("Alarm: actief   ");
  } else {
    lcd.print("Alarm: uit      ");
  }
}

void loop() {
  currentMillis = millis();
  node.process();
  keypad.getKey();
  timer();
}

void incomingMessage(const MyMessage &message) {
  if (message.type == V_LIGHT && message.sensor == 1) {
    lcd.backlight();
    LCD_LED = true;
    turnAlarm(message.getBool());
  }

  if (message.type == V_LIGHT && message.sensor == 3) {
    digitalWrite(BEEPER, message.getBool());
  }
}

void keypadEvent(KeypadEvent eKey) {
  if (LCD_LED == true) { // Do something if LCD backlight is on
    switch (keypad.getState()) {
      case PRESSED:
        previousMillis = currentMillis;
        analogWrite(BEEPER, 150);
        delay(50);
        analogWrite(BEEPER, 0);
        Serial.print("Pressed: ");
        Serial.println(eKey);
        if (FIRST_INPUT == true) {
          lcd.clear();
          lcd.print("Pincode:");
          lcd.setCursor ( 0, 1 );
          FIRST_INPUT = false;
        }
        switch (eKey) {
          case '*':
            checkPassword();
            password.reset();
            break;
          case '#':
            password.reset();
            lcd.clear();
            lcd.print("Pincode:");
            FIRST_INPUT == true;
            lcd.setCursor ( 0, 1 );
            break;
          default:
            password.append(eKey);
            lcd.print("*");
        }
    }
  } else { // Turn backlight on first
    lcd.backlight();
    LCD_LED = true;
  }
}

void checkPassword() {
  FIRST_INPUT = true;
  if (password.evaluate()) { // IF PASSWORD IS CORRECT
    Serial.println("Success");
    node.send(msg3.set(LOW));
    digitalWrite(BEEPER, LOW);
    TRIES = 0;
    if (ALARM == ON) {
      ALARM = OFF;
    } else {
      ALARM = ON;
    }
    turnAlarm(ALARM);
    node.send(msg.set(ALARM));
    analogWrite(BEEPER, 150);
    delay(50);
    analogWrite(BEEPER, 0);
    delay(50);
    analogWrite(BEEPER, 150);
    delay(50);
    analogWrite(BEEPER, 0);

  } else { // IF PASSWORD IS WRONG
    Serial.println("Wrong");
    analogWrite(BEEPER, 150);
    delay(500);
    analogWrite(BEEPER, 0);
    lcd.clear();
    lcd.print("Foute pincode");
    TRIES++;
    if (TRIES > 5 && ALARM == ON) {
      Serial.println("To many tries");
      lcd.setCursor ( 0, 1 );
      lcd.print("Alarm gaat af   ");
      node.send(msg2.set(HIGH));
    }
  }
}


void turnAlarm(boolean STATE) {
  Serial.print("Alarm: ");
  Serial.println(STATE);
  ALARM = STATE;
  node.saveState(0, ALARM);
  FIRST_INPUT = true;
  //LCD
  lcd.clear();
  if (ALARM == ON) {
    lcd.print("Alarm: actief");
  } else {
    lcd.print("Alarm: uit");
  }
}



void timer() {
  if (currentMillis - previousMillis > interval) {
    FIRST_INPUT = true;
    lcd.clear();
    if (ALARM == ON) {
      lcd.print("Alarm: actief");
    } else {
      lcd.print("Alarm: uit");
    }
    previousMillis = currentMillis;
    lcd.noBacklight();
    LCD_LED = false;
    Serial.println("Standby");
  }
}
