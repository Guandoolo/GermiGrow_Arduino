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

String direccionMAC = WiFi.macAddress();

// Variables del sensor de humedad del suelo
const int humsuelo = 33; //Lectura del sensor
int valHumsuelo;
// Variables sensor LDR
int LDR = 32;
int ValorLDR;
float luzPorcentaje;

//Variables del pulsador
int pinPulsador1 = 4; //Envio del Json
int pinPulRele = 34; //Encender Relé
int pinLed = 2;
int pinRele = 5;


/*           Datos necesarios para conectarse a internet            */

const char *ssid = "IUB.STUDENTS";  //Nombre del Wifi
const char *password = "1U3.res.soc"; // Contraseña

//const char *ssid = "Familia Cera";  //Nombre del Wifi
//const char *password = "D1001892527a"; // Contraseña

//const char *ssid = "FAMILIA JIMENEZ";  //Nombre del Wifi
//const char *password = "74814970"; // Contraseña

//const char *ssid = "Nokia";
//const char *password = "prueba2.0"; 

//const char *apiUrl = "http://(Ipv4):8000"; // URL base de la API
// La Ipv4 es del pc donde esta ejecutando la API
const char *apiUrl = "http://172.16.17.34:8000"; // URL de la API

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


/************************************************/

bool isWiFiConnected() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi is not connected!");
    return false;
  }
  Serial.println("WiFi is connected!");
  return true;
}

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

  /*           Conectar a la red WiFi           */

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print("Conectando");
    Serial.println(".");

    lcd.setCursor(0, 0);
    lcd.print("Conectando");
    lcd.setCursor(0, 1);
    lcd.print(".");
  }
  Serial.println("");
  Serial.println("Conexion establecida");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
  
  lcd.setCursor(0, 0);
  lcd.print("Conexion WIFI ");
  lcd.setCursor(0, 1);
  lcd.print("Exitosa");
  //lcd.print(WiFi.localIP());
  delay(3000);

  //Comprobacion para el DHT11
  Serial.println(F("Prueba del sensor DHTxx"));
  dht.begin(); //Iniciar el DTH11

  pinMode(humsuelo, INPUT);
  pinMode(LDR, INPUT);

  pinMode(pinPulsador1, INPUT);
  pinMode(pinLed, OUTPUT);

  pinMode(pinPulRele, INPUT);
  pinMode(pinRele, OUTPUT);

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("BIENVENIDOS");
  lcd.setCursor(2, 1);
  lcd.print("I fell good");

  delay(10000);

}



void sensores(){
  
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) && isnan(temperature) ) {
    Serial.println(F("Error al leer la humedad!"));
    Serial.println(F("Error al leer la temperatura!"));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(byte(3));
    lcd.print(" Error");
    lcd.setCursor(0, 1);
    lcd.print("al leer sensores");

  } else {
    
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
    //lcd.print(humedadInt);
    lcd.print("% ");
    lcd.write(byte(1));
    lcd.print(temperature);
    //lcd.print(temperaturaInt);
    lcd.print("C");
    lcd.print(" ");

    valHumsuelo = map(analogRead(humsuelo), 4095, 1100, 0, 99);
    //Imprimir valor
    Serial.print("Humedad del suelo: ");
    Serial.print(valHumsuelo);
    Serial.println(" %");
    lcd.setCursor(0, 1);
    lcd.write(byte(0));
    //lcd.print("Hume suelo: ");
    lcd.print(valHumsuelo);
    lcd.print("% ");

    int ValorLDR = analogRead(LDR);
    int VMinimo = 2600; //Es cuando menos luz hay 
    int VMaximo = 4000; //Es cuando mas luz hay
    //float Porcen = (ValorLDR - VMinimo) / (VMaximo - VMinimo) * 100.0;
    float luzPorcentaje = map(ValorLDR, VMinimo, VMaximo, 0, 100);

    Serial.print("Porcentaje de luz: ");
    Serial.print(luzPorcentaje);
    Serial.println(" %");

    lcd.write(byte(4));
    lcd.print(ValorLDR);
    //lcd.print(Porcentaje);
    //lcd.print("%");
  }
    lcd.setCursor(15, 0);
    lcd.write(byte(5));
    delay(5000);

    
    Serial.println("----");
    Serial.println("");

}

void sendDatos(){
  // Ejecutar la función solo si no se ha ejecutado

    HTTPClient http;  // Declarar una instancia de HTTPClient
    String url = String(apiUrl) + "/create_info_planta";

    http.begin(url); // Asignar URL
    http.addHeader("Content-Type", "application/json");

    // Obtener la hora del servidor NTP
    //timeClient.update();
    //String hora_colombiana = timeClient.getFormattedTime();

    // Crear un JSON con los valores necesarios
    const size_t bufferSize = JSON_OBJECT_SIZE(6);
    DynamicJsonDocument doc(bufferSize);

    // Asignar los valores del JSON
    doc["temp_ambiente"] = temperature;
    doc["hume_ambiente"] = humidity;
    doc["hume_suelo"] = valHumsuelo;
    doc["luz_porcen"] = luzPorcentaje;
    doc["fecha"] = "2007-05-08 12:35:29"; 
    doc["id_dispositivo"] = "12A1";

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
  Serial.println(httpResponseCode);
}



void loop() {

 sensores();
 
 if (isWiFiConnected()) {
    sendDatos();
  } else {
    Serial.println("Conexion WIFI Peridida!");
  }

  if(digitalRead(pinPulRele) == HIGH){
    interrupcionRele();
    digitalWrite(pinLed, HIGH);
  delay(100);
  }else{
  digitalWrite(pinRele, LOW);
  delay(100);
  }


  
}

void mostrarDireccionMAC() {
  

  // Dividir la dirección MAC en dos partes
  String primeraParte = direccionMAC.substring(0, 8);
  String segundaParte = direccionMAC.substring(8);

  // Mostrar la primera parte en la primera fila
  lcd.setCursor(0, 0);
  lcd.print("MAC: " + primeraParte);

  // Mostrar la segunda parte en la segunda fila
  lcd.setCursor(0, 1);
  lcd.print(segundaParte);
}

void interrupcionRele(){
  /* Pulsado encargado de encender el relé */
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Regando...");
  lcd.setCursor(0, 1);
  lcd.print("Agua Activada");
  Serial.println("Agua Activada");
  digitalWrite(pinRele, HIGH);

}

