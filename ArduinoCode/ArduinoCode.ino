#include <LiquidCrystal.h>
#include <Servo.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Servo myservoR;
Servo myservoL;

char readserial;
String strVal;
int intVal;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(6, OUTPUT);  //LCD pin setup
  analogWrite(6, 255); //LCD brightness
  myservoL.attach(7);
  myservoR.attach(A0);
  myservoL.write(118);
  myservoR.write(44);
}

void loop() {
  if (Serial.available()) {

    switch (Serial.read()) {
      case 'l':
        lcd.clear();
        while (Serial.available() > 0) {
          readserial = Serial.read(); // recebe cada caracter da serial
          strVal += readserial; // carrega buffer string
          delay(1);
        }
        intVal = (strVal.toInt());
        intVal = map(intVal, 0, 985, 105, 170);
        myservoL.write(intVal);
        lcd.setCursor(0, 0);
        lcd.print("L: ");
        lcd.print(strVal);
        strVal = "";
        delay(50);
        break;

      case 'r':
        lcd.clear();
        while (Serial.available() > 0) {
          readserial = Serial.read(); // recebe cada caracter da serial
          strVal += readserial; // carrega buffer string
          delay(1);
        }
        intVal = (strVal.toInt());
        intVal = map(intVal, 0, 985, 52, 10);
        myservoR.write(intVal);
        lcd.setCursor(0, 1);
        lcd.print("R: ");
        lcd.print(strVal);
        strVal = "";
        delay(50);
        break;
    }
  }
}
