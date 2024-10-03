#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

int humsuelo = 33;

/* Caracteres Especiales */
byte hume[] = {
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111,
  B01110,
  B01110
};

void setup() {
  // put your setup code here, to run once:
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, hume);
  lcd.home();
  
  Serial.begin(115200);
  pinMode(humsuelo, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  //Convertir el valor en porcentaje
  
  int humedad = analogRead(humsuelo);
  int valHumsuelo = analogRead(humsuelo);
  //Imprimir valor
  Serial.print("Humedad del suelo: ");
  Serial.println(humedad);
  valHumsuelo = map(analogRead(humsuelo), 4095, 1200, 0, 99);
  Serial.println(valHumsuelo);
  Serial.print(" %");


  delay(1000);
}
