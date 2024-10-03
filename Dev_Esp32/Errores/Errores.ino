/*           Librerias necesarias para la conexion atravez de wifi           */ 
#include <WiFi.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

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

// Variables del sensor de humedad del suelo
const int humsuelo = 33; 
int valHumsuelo;
// Variables sensor LDR
int LDR = 32;
int ValorLDR;


//Variables del pulsador
int pinPulsador1 = 4; //Envio del Json
int pinPulsador2 = 36; //Encender Relé
int pinLed = 2;
//Variables Relé
int pinRele = 5;

/*           Datos necesarios para conectarse a internet            */
const char *ssid = "Familia Cera";  //Nombre del Wifi
const char *password = "D1001892527a"; // Contraseña

//const char *ssid = "FAMILIA JIMENEZ";  //Nombre del Wifi
//const char *password = "74814970"; // Contraseña

//const char *ssid = "Nokia";
//const char *password = "esunaprueba"; 
//const char *apiUrl = "http://127.0.0.1:8000"; // URL de la API

/***************Caracteres Especiales*************/
byte humeS[] = {
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111,
  B11111,
  B01110
};

byte tempChar[] = {
  B00100,
  B01010,
  B01010,
  B01010,
  B01110,
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

byte solChar[] = {
  B00000,
  B00000,
  B10000,
  B00100,
  B11000,
  B11101,
  B11110,
  B11110
};

byte wifiChar[] = {
  B00000,
  B00000,
  B00000,
  B00001,
  B00011,
  B00111,
  B01111,
  B11111
};


void setup() {

  //Iniciamos el LED del LCD
  lcd.init();
  lcd.backlight();
  Serial.begin(115200);

  /* Caracteres especiales */
  lcd.createChar(0, humeS);
  lcd.createChar(1, tempChar);
  lcd.createChar(2, humeChar);
  lcd.createChar(3, errorChar);
  lcd.createChar(4, solChar);
  lcd.createChar(5, wifiChar);
  lcd.home();
  attachInterrupt(digitalPinToInterrupt(pinPulsador2), ISR_interrupcionRele, RISING);
  attachInterrupt(digitalPinToInterrupt(pinPulsador1), ISR_interrupcionRele, RISING);

  /*           Conectar a la red WiFi           */

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print("Conectando");
    Serial.println(".");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Conectando");
    lcd.setCursor(0, 1);
    lcd.print(".");
  }
  Serial.println("");
  Serial.println("Conexion establecida");
  Serial.println("Dirección IP: ");
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Conexion WIFI");
  //lcd.write(byte(5));
  lcd.setCursor(0, 1);
  lcd.print("IP: ");
  lcd.print(WiFi.localIP());
  Serial.println(WiFi.localIP());
  delay(5000);
  

  dht.begin(); //Iniciar el DTH11
  
  pinMode(humsuelo, INPUT);
  pinMode(LDR, INPUT);

  pinMode(pinPulsador1, INPUT);
  pinMode(pinLed, OUTPUT);

  pinMode(pinPulsador2, INPUT);
  pinMode(pinRele, OUTPUT);

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("BIENVENIDOS");
  lcd.setCursor(2, 1);
  lcd.print("I fell good");

  delay(7000);

}

void loop() {

  /* Pulsador encargado de enviar los datos */
  if(digitalRead(pinPulsador1) == HIGH){
    Serial.println("Mostrando direccion MAC");
    delay(1000);
  }else{
    Serial.println("Esperando");
    delay(1000);
  }


  unsigned long millisActuales = millis();
  unsigned long ultimaLecturaSensor = 0;
  unsigned long ultimaActualizacionLCD = 0;
  const unsigned long lecturaSensorIntervalo = 5000;

  if (millisActuales - ultimaLecturaSensor >= lecturaSensorIntervalo) {
    ultimaLecturaSensor = millisActuales;

  }

    int valHumsuelo = map(analogRead(humsuelo), 4092, 0, 0, 99);
    int ValorLDR = analogRead(LDR);
  

  unsigned long tiempoActual = millis();
  if (tiempoActual - ultimaActualizacionLCD >= 5000){

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("¡Error al leer el sensor DHT!");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Error DHT11");
    }

    Serial.print("Humedad: ");
    Serial.print(humidity);
    Serial.println(" %");
    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.println(" °C");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(byte(2));
    lcd.print(humidity);
    lcd.print("% ");
    lcd.write(byte(1));
    lcd.print(temperature);
    lcd.print("C ");

    /**********************Sensor de humedad del suelo********************/ 
    Serial.print("Humedad del suelo: ");
    Serial.print(valHumsuelo);
    Serial.println(" %");
    lcd.setCursor(0, 1);
    lcd.write(byte(0));
    lcd.print(valHumsuelo);
    lcd.print("% ");

    /**********************Sensor LDR********************/
    lcd.write(byte(4));
    lcd.print(ValorLDR);
    ultimaActualizacionLCD = tiempoActual;
  }
  
  //Caracter especial de conexion wifi
  lcd.setCursor(15, 0);
  lcd.write(byte(5));

  Serial.println("----");
  Serial.println("");

}

  /*           Json para mandar los datos           */
  /*
void sendDato(){
  
  // Ejecutar la función solo si no se ha ejecutado

    HTTPClient http;  // Declarar una instancia de HTTPClient
    String url = String(apiUrl) + "/create_lectura";

    http.begin(url); // Asignar URL
    http.addHeader("Content-Type", "application/json");

    // Obtener la hora del servidor NTP
    timeClient.update();
    String hora_colombiana = timeClient.getFormattedTime();

    // Crear un JSON con los valores necesarios
    const size_t bufferSize = JSON_OBJECT_SIZE(7);
    DynamicJsonDocument doc(bufferSize);

    // Asignar los valores del JSON
    doc["id_info"] = 0;
    doc["temp_ambiente"] = String(temperature); // Convertir el float a String para el JSON
    doc["hume_ambiente"] = String(humidity);
    doc["hume_suelo"] = valHumsuelo;
    doc["luz_porcen"] = ValorLDR; // Valor de 
    doc["fecha"] = fecha_actual;
    doc["hora"] = hora_colombiana;  
    doc["id_planta"] = direccionMAC;

    String output;
    serializeJson(doc, output);
    Serial.println(output);

    int httpResponseCode = http.POST(output);  // Realizar la solicitud POST con el JSON

    if (httpResponseCode > 0) {
      String payload = http.getString();
      Serial.println("Respuesta POST con datos JSON: " + payload);
    } else {
      Serial.println("Error en la solicitud POST con datos JSON");
    }

}
  */

void ISR_interrupcionRele(){
  /* Pulsado encargado de encender el relé */
  digitalWrite(pinRele, HIGH);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Regando...");
  lcd.setCursor(0, 1);
  lcd.print("Agua Activada");
  Serial.println("Agua Activada");
}

void ISR_interrupcionDcMac(){
    /*

  String direccionMAC = WiFi.macAddress();

  // Mostrar la dirección MAC completa en la primera fila
  lcd.setCursor(0, 1);
  //lcd.print(direccionMAC);

  int position = 0;

  while (true) {
    // Clear the LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MAC: ");
    lcd.setCursor(0, 1);

    // Write the text starting from the current position
    for (int i = position; i < direccionMAC.length(); i++) {
      if (i == 0) { 
      lcd.print(" ");   
      }
      lcd.print(direccionMAC[i]);
    }

    // Increment the position
    position++;

    // Wrap around if the position exceeds the text length
    if (position >= direccionMAC.length()) {
      position = 0;
    }

    // Delay to control the scrolling speed
    delay(1000);
    
  }
  delay(10000);
  */
}

