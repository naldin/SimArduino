#include <LiquidCrystal.h>
#include <Servo.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Servo myservo;

char readserial;
String strVal;
int intVal;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(6, OUTPUT);  //LCD pin setup
  analogWrite(6, 255); //LCD brightness
  myservo.attach(7);
}

void loop() {
  if (Serial.available()) {
    lcd.clear();
    delay(20);

    while (Serial.available() > 0) {
      readserial = Serial.read(); // recebe cada caracter da serial
      strVal += readserial; // carrega buffer string
    }
    //intVal = atoi(strVal.c_str()); // converte string para int
    intVal = (strVal.toInt());
    intVal = map(intVal, 0, 999, 0, 120);
    myservo.write(intVal);
    lcd.print(strVal);
    strVal = "";
  }
}
