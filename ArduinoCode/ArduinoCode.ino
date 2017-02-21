#include <LiquidCrystal.h>
#include <Servo.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Servo myservoR;
Servo myservoL;

char readserialL;
char readserialR;
String strValL;
String strValR;
int intValL;
int intValR;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(6, OUTPUT);  //LCD pin setup
  analogWrite(6, 255); //LCD brightness
  myservoR.attach(7);
  myservoL.attach(A0);
}

void loop() {
  if (Serial.available()) {
    lcd.clear();
    delay(5);
    if (Serial.read() == 'l') {
      while (Serial.available() > 0) {
        readserialL = Serial.read(); // recebe cada caracter da serial
        strValL += readserialL; // carrega buffer string
      }

      // strVal.remove(0, 1);
      intValL = (strValL.toInt());
      intValL = map(intValL, 0, 999, 0, 120);
      myservoL.write(intValL);
      lcd.print("l ");
      lcd.print(strValL);
      strValL = "";
    }
    
    else {
      while (Serial.available() > 0) {
        readserialR = Serial.read(); // recebe cada caracter da serial
        strValR += readserialR; // carrega buffer string
      }

      //strVal.remove(0, 1);
      intValR = (strValR.toInt());
      intValR = map(intValR, 0, 999, 0, 120);
      myservoR.write(intValR);
      lcd.print("r ");
      lcd.print(strValR);
      strValR = "";
    }


  }
}
