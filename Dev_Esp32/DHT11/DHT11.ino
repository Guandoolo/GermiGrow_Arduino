//Librerias necesarias para el display LCD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Librerias y Variables del DHT
#include <DHT.h>
#define DHTPIN 15
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float humidity;
float temperature; 

/***************Caracteres Especiales*************/
byte tempChar[] = {
  B00100,
  B01110,
  B01110,
  B01110,
  B11111,
  B11111,
  B11111,
  B01110
};

byte humeChar[] = {
  B00100,
  B00100,
  B01110,
  B01110,
  B11111,
  B11111,
  B11111,
  B01110
};

byte errorChar[] = {
  B00000,
  B10101,
  B01010,
  B10101,
  B00000,
  B01010,
  B11111,
  B00000
};




/*************************************************/

void setup() {
  // put your setup code here, to run once:
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, tempChar);
  lcd.createChar(1, humeChar);
  lcd.createChar(2, errorChar);
  lcd.home();
  

  Serial.begin(115200);

    //Comprobacion para el DHT11
  Serial.println(F("Prueba del sensor DHTxx"));
  dht.begin(); //Iniciar el DTH11
}

void loop() {
  

  
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) && isnan(temperature) ) {
    Serial.println(F("Error al leer la humedad!"));
    Serial.println(F("Error al leer la temperatura!"));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(byte(1));
    lcd.print(" Error");
    lcd.setCursor(0, 1);
    lcd.print("al leer sensores");

  } else {
    int humedadInt = floor(humidity);
    int temperaturaInt = floor(temperature);

    Serial.print("Humedad: ");
    Serial.print(humidity);
    Serial.println(" %");
    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.println(" °C");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(byte(1));
    lcd.print(humidity);
    //lcd.print(humedadInt);
    lcd.print("% ");
    lcd.write(byte(0));
    lcd.print(temperature);
    //lcd.print(temperaturaInt);
    lcd.print("C");
    lcd.print(" ");
  }

  delay(5000);
}