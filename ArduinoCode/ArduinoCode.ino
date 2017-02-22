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
  myservoL.write(120);
  myservoR.write(45);
}

void loop() {
  if (Serial.available()) {
    lcd.clear();
    delay(5);

    if (Serial.read() == 'r') {
      while (Serial.available() > 0) {
        readserial = Serial.read(); // recebe cada caracter da serial
        strVal += readserial; // carrega buffer string
      }
      intVal = (strVal.toInt());
      intVal = map(intVal, 0, 999, 45, 0);
      myservoR.write(intVal);
      lcd.print("R: ");
      lcd.print(strVal);
      strVal = "";
    }

    else {
      while (Serial.available() > 0) {
        readserial = Serial.read(); // recebe cada caracter da serial
        strVal += readserial; // carrega buffer string
      }
      intVal = (strVal.toInt());
      intVal = map(intVal, 0, 999, 120, 180);
      myservoL.write(intVal);
      lcd.print("L: ");
      lcd.print(strVal);
      strVal = "";
    }
  }
}
