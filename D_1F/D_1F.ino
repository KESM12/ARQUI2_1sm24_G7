#include<Wire.h>
#include<DHT.h>
#include<LiquidCrystal_I2C.h>
#include <EEPROM.h>

const int TH11 = 13; //
const int trig = 12; //
const int echo = 11; //
const int MT = 10; //
const int LA = 9; //
const int MC = 8; //
const int MP = 7; //
const int GE = 6; //
const int ME = 5; //

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
bool estadoLectura = false;


// DEFINICIONES
DHT dht(TH11, DHT11);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // put your setup code here, to run once:
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
  //madres de la lcd.
  lcd.init();
  lcd.backlight();
  lcd.print("Wenas.");


  // Serial.println("MQ135 Funcionando.");
  // Serial.println("DHT11 Funcionando.");
  delay(2000); //retraso como el que tiene el edwin.
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.clear();
  digitalWrite(trig, LOW); //para que lea algo y lo estabilicemos desde el inicio.
  delay(200);
  humedad = dht.readHumidity();
  temp = dht.readTemperature();
  int raw_data = analogRead(A0);
  int lum_data = analogRead(A1);
  estadoBMT = digitalRead(MT);
  estadoBLA = digitalRead(LA);
  estadoBMC = digitalRead(MC);
  estadoBMP = digitalRead(MP);
  estadoBGE = digitalRead(GE);
  estadoBME = digitalRead(ME);
  //que siga leyendo los datos despues del inicio xddd
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  //Vainas para el ultrasonico
  duracion = pulseIn(echo, HIGH); //Tiempo entre las lecturas de las distancias.
  //calculo de las distancias.
  distancia = duracion/58.4; //esto lo da en centimetros.

  //Vaina del dht11
  if(isnan(humedad) || isnan(temp)){ //si el sensor de humedad y temperatura falla mostrara el siguiente mensaje
    Serial.println("Error en el sensor");
    return;
  }

  estadoLectura = true;

  
  if(estadoLectura){
    if (estadoBMT == HIGH) {
    Serial.print(" Humedad: ");
    Serial.print(humedad);
    Serial.print("% Temperatura: ");
    Serial.print(temp);
    Serial.println(" C ");
    lcd.setCursor(0, 0);
    lcd.print("HUM:");
    lcd.setCursor(5, 0); //columna, fila
    lcd.print(humedad);
    lcd.setCursor(0, 1);
    lcd.print("TEM:");
    lcd.setCursor(5, 1); //columna, fila
    lcd.print(temp);
    delay(2000);
    estadoLectura = false;
  } else if (estadoBLA == HIGH) {
    Serial.print(" Luminosidad = ");
    Serial.println(lum_data);
    lcd.setCursor(0, 0);
    lcd.print("LUM:");
    lcd.setCursor(5, 0); //columna, fila
    lcd.print(lum_data);
    delay(2000);
    estadoLectura = false;
  } else if (estadoBMC == HIGH) {
    Serial.print("Gas raw value = ");
    Serial.println(raw_data);
    lcd.setCursor(0, 0);
    lcd.print("CO2:");
    lcd.setCursor(5, 0); //columna, fila
    lcd.print(raw_data);
    delay(2000);
    estadoLectura = false;
  } else if (estadoBMP == HIGH) {
    Serial.print("Distancia: ");
    Serial.print(distancia);
    Serial.println(" cm");
    lcd.setCursor(0, 0);
    lcd.print("DIS:");
    lcd.setCursor(5, 0); //columna, fila
    lcd.print(distancia);
    delay(2000);
    estadoLectura = false;
  } else if (estadoBGE == HIGH) {
    //Guadar datos en la EEPROM.
    guardarDatos(distancia, lum_data, raw_data, temp, humedad);
    Serial.print("Datos almacenados en memoria.");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Datos almacenados en memoria.");
    delay(200);
    estadoLectura = false;
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
    lcd.print("cm  CO2: ");
    lcd.print(co2_almacenada);
    lcd.print("ppm  Luz: ");
    lcd.print(lum_almacenada);
    delay(200);
  }
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