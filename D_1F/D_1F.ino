#include<Wire.h>
#include<DHT.h>
#include<LiquidCrystal_I2C.h>
#include <EEPROM.h>

const int TH11 = 13; // Pin donde se encuentra conectado el sensor dht11 mide humedad y temperatura.
const int trig = 12; // Pin del ultrasonico.
const int echo = 11; // Pin del ultrasonico.
const int MT = 10; // Pin para el boton de mostrar temperatura y humedad
const int LA = 9; // Pin para el boton de mostrar luminocidad
const int MC = 8; // Pin para el boton de mostrar co2
const int MP = 7; // Pin para el boton de mostrar proximidad
const int GE = 6; // Pin para el boton de guardar todos los datos en la eeprom
const int ME = 5; // Pin para el boton de mostrar los datos almacenados en la eeprom

//VARIABLES 
int temp;
int humedad;
float distancia, duracion; 
int estadoBMT = 0;
int estadoBLA = 0;
int estadoBMC = 0;
int estadoBMP = 0;
int estadoBGE = 0;
int estadoBME = 0;


// DEFINICIONES
DHT dht(TH11, DHT11);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  //Inicializaciones de los dispositivos.
  Serial.begin(9600);
  dht.begin();
  // Configuración de pines
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(MT, INPUT);
  pinMode(LA, INPUT);
  pinMode(MC, INPUT);
  pinMode(MP, INPUT);
  pinMode(GE, INPUT);
  pinMode(ME, INPUT);
  // Configuracion de la LCD.
  lcd.init();
  lcd.backlight();
  lcd.print("ACE2 GRUPO 7");
  delay(2000); // Retraso de 2 segundos para salir del setup.
}

void loop() {
  lcd.clear(); // Limpiamos el LCD.
  digitalWrite(trig, LOW); //para que lea algo y lo estabilicemos desde el inicio.
  //delay(200);esto era opcional 

  // Variables de almacenamiento para la lecturas de los sensores
  humedad = dht.readHumidity(); // Lectura de humedad
  temp = dht.readTemperature(); // Lectura de
  int raw_data = analogRead(A0); // Lectura de CO2
  int lum_data = analogRead(A1); // Lectura de Luminosidad
  estadoBMT = digitalRead(MT); // Lectura del botón para temperatura y humedad
  estadoBLA = digitalRead(LA); // Lectura del botón para luminosidad
  estadoBMC = digitalRead(MC); // Lectura del botón para CO2
  estadoBMP = digitalRead(MP); // Lectura del botón para proximidad
  estadoBGE = digitalRead(GE); // Lectura del botón para guardar datos en la eeprom
  estadoBME = digitalRead(ME); // Lectura del botón para mostrar datos de la eeprom
  
  //Configuración del ultrasonico
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duracion = pulseIn(echo, HIGH); //Tiempo entre las lecturas de las distancias.
  distancia = duracion/58.4; // Distancia medida en centimetros.

  //If que valida el estado del sensor de temperatura y humedad
  if(isnan(humedad) || isnan(temp)){ //si el sensor de humedad y temperatura falla mostrara el siguiente mensaje en la terminal.
    Serial.println("Error en el sensor");
    lcd.setCursor(0, 0);
    lcd.println("Sensor MQ135");
    lcd.setCursor(0, 1);
    lcd.println("no encontrado");
    return;
  }

  if (estadoBMT == HIGH) {
    // Monitor serial.
    Serial.print(" Humedad: ");
    Serial.print(humedad);
    Serial.print("% Temperatura: ");
    Serial.print(temp);
    Serial.println(" C ");
    // LCD.
    lcd.setCursor(0, 0);
    lcd.print("HUM:");
    lcd.setCursor(5, 0); //columna, fila
    lcd.print(humedad);
    lcd.print(" % ");
    lcd.setCursor(0, 1);
    lcd.print("TEM:");
    lcd.setCursor(5, 1); //columna, fila
    lcd.print(temp);
    lcd.print("° C");
    // Retraso
    delay(2000);
  } else if (estadoBLA == HIGH) {
    Serial.print(" Luminosidad = ");
    Serial.println(lum_data);
    lcd.setCursor(0, 0);
    lcd.print("LUM:");
    lcd.setCursor(5, 0); //columna, fila
    lcd.print(lum_data);
    lcd.print("lum");
    delay(2000);
  } else if (estadoBMC == HIGH) {
    Serial.print("CO2: ");
    Serial.println(raw_data);
    lcd.setCursor(0, 0);
    lcd.print("CO2:");
    lcd.setCursor(5, 0); //columna, fila
    lcd.print(raw_data);
    delay(2000);
  } else if (estadoBMP == HIGH) {
    Serial.print("Distancia: ");
    Serial.print(distancia);
    Serial.println(" cm");
    lcd.setCursor(0, 0);
    lcd.print("DIS:");
    lcd.setCursor(5, 0); //columna, fila
    lcd.print(distancia);
    lcd.print(" cm");
    delay(2000);
  } else if (estadoBGE == HIGH) {
    //Guadar datos en la EEPROM.
    guardarDatos(distancia, lum_data, raw_data, temp, humedad);
    Serial.print("Datos almacenados en memoria.");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Datos");
    lcd.setCursor(0, 1);
    lcd.print("almacenados.");
    delay(1000);
  } else if (estadoBME = HIGH){
    //Recuperar datos de la EEPROM.
    float dist_almacenada, lum_almacenada, co2_almacenada, temp_almacenada, hum_almacenada;
    recuperarDatos(dist_almacenada, lum_almacenada, co2_almacenada, temp_almacenada, hum_almacenada);
    Serial.print("Distancia almacenada: ");
    Serial.print(dist_almacenada);
    Serial.println(" cm");
    Serial.print("Luminosidad almacenada: ");
    Serial.println(lum_almacenada);
    Serial.print("CO2 almacenada: ");
    Serial.println(co2_almacenada);
    Serial.print("Temperatura almacenada: ");
    Serial.print(temp_almacenada);
    Serial.println(" C");
    Serial.print("Humedad almacenada: ");
    Serial.print(hum_almacenada);
    Serial.println(" %");
    //LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temp_almacenada);
    lcd.print("C  Hum: ");
    lcd.print(hum_almacenada);
    lcd.setCursor(0, 1);
    lcd.print("Dist: ");
    lcd.print(dist_almacenada);
    lcd.print("cm   ");
    lcd.setCursor(0, 1);
    lcd.print("CO2: ");
    lcd.print(co2_almacenada);
    lcd.print("ppm  Luz: ");
    lcd.print(lum_almacenada);
    delay(2000);
  }
  delay(1000);
}

void guardarDatos(float dist_val, float lum_val, float co2_val, float temp_val, float hum_val) {
  int direccion = 0;
  EEPROM.put(direccion, dist_val);
  direccion += sizeof(float);
  EEPROM.put(direccion, lum_val);
  direccion += sizeof(float);
  EEPROM.put(direccion, co2_val);
  direccion += sizeof(float);
  EEPROM.put(direccion, temp_val);
  direccion += sizeof(float);
  EEPROM.put(direccion, hum_val);
}

void recuperarDatos(float& dist_val, float& lum_val, float& co2_val, float& temp_val, float& hum_val) {
  int direccion = 0;
  EEPROM.get(direccion, dist_val);
  direccion += sizeof(float);
  EEPROM.get(direccion, lum_val);
  direccion += sizeof(float);
  EEPROM.get(direccion, co2_val);
  direccion += sizeof(float);
  EEPROM.get(direccion, temp_val);
  direccion += sizeof(float);
  EEPROM.get(direccion, hum_val);
}