#include <Wire.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <PubSubClient.h>
#include <Servo.h>


//==========================================================================================================
//==========================================================================================================
//============================================= Declaraciones F1 ===========================================
//==========================================================================================================
//==========================================================================================================

Servo Servo1; // Crea un objeto de clase "Servo" llamado "Servo1"
int potpin = 0; // Pin analógico para el potenciómetro
int val; // Variable para el valor leído del potenciómetro

const int TH11 = 13;  // Pin donde se encuentra conectado el sensor dht11 mide humedad y temperatura.
const int trig = 12;  // Pin del ultrasonico.
const int echo = 11;  // Pin del ultrasonico.
const int MT = 7;     // Pin para el boton de mostrar temperatura y humedad
const int LA = 6;     // Pin para el boton de mostrar luminocidad
const int MP = 5;     // Pin para el boton de mostrar proximidad
const int MC = 4;     // Pin para el boton de mostrar co2
const int RELE1 = 3;   
const int RELE2 = 2;      
const int FAN = 9;    // Pin para el ventilador
int GE;
int ME;

//VARIABLES
float distancia;
int lum_data = 0;  // Lectura de Luminosidad
int raw_data = 0;  // Lectura de CO2
int temp;
int humedad;
int estadoBMT = 0;
int estadoBLA = 0;
int estadoBMC = 0;
int estadoBMP = 0;
int focoval;
int door;
int contador = 0;
int contador1 = 0;
int humanoval;
int velocidad1;

// Define variables para indicar si se debe mostrar cada tipo de dato
bool mostrarTemperatura = false;
bool mostrarLuminosidad = false;
bool mostrarCO2 = false;
bool mostrarProximidad = false;
bool velocidad = false;
bool foco = false;
bool puerta = false;
bool humano = false;
bool calidadaire = false;

// Declara una variable booleana para controlar el estado del servo
bool activarServo = false;

volatile bool guardarEeprom = false;
volatile bool mostrarEeprom = false;

unsigned long tiempoAnterior = 0;
unsigned long intervalo = 1000;  // 1000 milisegundos = 1 segundo

bool calDistancia(int trig, int echo);
// DEFINICIONES
DHT dht(TH11, DHT11);
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial mySerial(8, 10);

//==========================================================================================================
//==========================================================================================================
//=================================================== Setup ================================================
//==========================================================================================================
//==========================================================================================================


void setup() {
  //Inicializaciones de los dispositivos.
  Serial.begin(9600);
  mySerial.begin(9600);
  dht.begin();
  // Configuración de pines
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(MT, INPUT);
  pinMode(LA, INPUT);
  pinMode(MC, INPUT);
  pinMode(MP, INPUT);
  pinMode(FAN, OUTPUT);
  pinMode(RELE1, OUTPUT);
  pinMode(RELE2, OUTPUT);
  Servo1.attach(A2); // Adjunta el objeto al pin A2
  Servo1.write(90);
  digitalWrite(RELE1, HIGH);
  // Interrupciones
  attachInterrupt(digitalPinToInterrupt(GE), estado_guardarEeprom, FALLING);
  attachInterrupt(digitalPinToInterrupt(ME), estado_mostrarEeprom, FALLING);
  // Configuracion de la LCD.
  lcd.init();
  lcd.backlight();
  lcd.print(" ACE2 GRUPO 7");

  delay(2000);  // Retraso de 2 segundos para salir del setup.
}

//==========================================================================================================
//==========================================================================================================
//==================================================== Loop ================================================
//==========================================================================================================
//==========================================================================================================

void loop() {
  if (mySerial.available() > 0) {
  recibirSolicitudesMQTT();
  }


  /*
  // Verificar si ha pasado el intervalo de 1 segundo
  unsigned long tiempoActual = millis();
  if (tiempoActual - tiempoAnterior >= intervalo) {
    // Verificar si hay datos disponibles en mySerial
    if (mySerial.available()) {
      // Procesar los datos de mySerial
      recibirSolicitudesMQTT();
    } else {
      // Si no hay datos disponibles, enviar datos por la comunicación serial
      enviarDatosSerial();
    }
    // Actualizar el tiempo anterior al actual
    tiempoAnterior = tiempoActual;
  }*/

  lcd.clear();  // Limpiamos el LCD.
  lcd.print(" ENVIANDO DATOS");
  //digitalWrite(trig, LOW); //para que lea algo y lo estabilicemos desde el inicio.

  // Variables de almacenamiento para la lecturas de los sensores
  humedad = dht.readHumidity();  // Lectura de humedad
  temp = dht.readTemperature();  // Lectura de
  raw_data = analogRead(A0);     // Lectura de CO2
  lum_data = analogRead(A1);     // Lectura de Luminosidad
  long t;                        //timepo que demora en llegar el eco
  long d;                        //distancia en centimetros

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);  //Enviamos un pulso de 10us
  digitalWrite(trig, LOW);

  t = pulseIn(echo, HIGH);  //obtenemos el ancho del pulso
  d = t / 59;      // escalamos una distancia en cm.
 

  if (raw_data > 100){
    contador1 += 1;
    if (contador1 == 20) {
        humanoval = 3;
        calidadaire = true;
        delay(100);
        enviarDatosSerial();
    } else if (contador1 == 40) {
        calidadaire = true;
        digitalWrite(RELE2, HIGH);
        digitalWrite(FAN, HIGH);
        Serial.println("Encendiendo el ventilador rapido");
        contador1 = 0;
        delay(100);
        humanoval = 4;
        enviarDatosSerial();
    }
} else {
    if(velocidad1 == 0){
      digitalWrite(RELE2, LOW);
      digitalWrite(FAN, LOW);
    } else if (velocidad == 1){
      digitalWrite(RELE2, LOW);
      digitalWrite(FAN, HIGH);
    }
    contador1 = 0;
    humanoval = 0;
    calidadaire = false;
}
delay(100);

  if (d > 200) {
    contador += 1; 
    if (contador == 20) {
        humanoval = 1;
        humano = true;
        delay(100);
        enviarDatosSerial();
    } else if (contador == 40) {
        humanoval = 2;
        humano = true;
        digitalWrite(RELE1, HIGH);
        focoval = 0;
        Serial.println("Apagando el foco");
        foco = false;
        contador = 0;
        delay(100);
        enviarDatosSerial();
    }
} else {
    contador = 0;
    humanoval = 0;
    humano = false;
}
delay(100);


  

  /*if (d < 1500){
    Serial.println(d);
  }
  if (mySerial.available() > 0) {
    recibirSolicitudesMQTT();
    delay(100);
  } else {
    enviarDatosSerial();
  }*/
  
  

  estadoBMT = digitalRead(MT);  // Lectura del botón para temperatura y humedad
  estadoBLA = digitalRead(LA);  // Lectura del botón para luminosidad
  estadoBMC = digitalRead(MC);  // Lectura del botón para CO2
  estadoBMP = digitalRead(MP);  // Lectura del botón para proximidad

  //If que valida el estado del sensor de temperatura y humedad
  if (isnan(humedad) || isnan(temp)) {  //si el sensor de humedad y temperatura falla mostrara el siguiente mensaje en la terminal.
    Serial.println("Error en el sensor");
    lcd.setCursor(0, 0);
    lcd.println("Sensor MQ135");
    lcd.setCursor(0, 1);
    lcd.println("no encontrado");
    return;
  }

  // Verificar si se debe mostrar cada tipo de dato y mostrarlo durante 2 segundos
  if (estadoBMT == HIGH) {
    lcd.clear();  // Limpiamos el LCD.
    mostrarTemperatura = true;
    delay(50);  // Pequeño retraso para evitar el rebote del botón
  } else if (estadoBLA == HIGH) {
    lcd.clear();  // Limpiamos el LCD.
    mostrarLuminosidad = true;
    delay(50);
  } else if (estadoBMC == HIGH) {
    lcd.clear();  // Limpiamos el LCD.
    mostrarCO2 = true;
    delay(50);
  } else if (estadoBMP == HIGH) {
    lcd.clear();  // Limpiamos el LCD.
    mostrarProximidad = true;
    delay(50);
  }



  // Mostrar los datos según corresponda
  if (mostrarTemperatura) {
    // Mostrar temperatura y humedad
    lcd.setCursor(0, 0);
    lcd.print("HUM:");
    lcd.setCursor(5, 0);
    lcd.print(humedad);
    lcd.print(" % ");
    lcd.setCursor(0, 1);
    lcd.print("TEM:");
    lcd.setCursor(5, 1);
    lcd.print(temp);
    lcd.print(" C");
    delay(2000);
    mostrarTemperatura = false;
  } else if (mostrarLuminosidad) {
    // Mostrar luminosidad
    lcd.setCursor(0, 0);
    lcd.print("LUM:");
    lcd.setCursor(4, 0);
    lcd.print(lum_data);
    lcd.print("  lum");
    delay(2000);
    mostrarLuminosidad = false;
  } else if (mostrarCO2) {
    // Mostrar CO2
    lcd.setCursor(0, 0);
    lcd.print("CO2:");
    lcd.setCursor(5, 0);
    lcd.print(raw_data);
    lcd.setCursor(9, 0);
    lcd.print("ppm");
    delay(2000);
    mostrarCO2 = false;
  } else if (mostrarProximidad) {
    // Mostrar proximidad
    lcd.setCursor(0, 0);
    lcd.print("DIS:");
    lcd.setCursor(5, 0);
    lcd.print(d);  //calDistancia(trig, echo)
    lcd.print(" cm");
    delay(2000);
    mostrarProximidad = false;
  } /* else if (guardarEeprom) {
    //Guadar datos en la EEPROM.
    guardarDatos(d, lum_data, raw_data, temp, humedad);
    Serial.print("Datos almacenados en memoria.");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Datos");
    lcd.setCursor(0, 1);
    lcd.print("almacenados.");
    guardarEeprom = false;
    delay(1000);
  } else if (mostrarEeprom) {
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
    // lcd
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temp_almacenada);
    lcd.print(" C");
    lcd.setCursor(0, 1);
    lcd.print("Hum: ");
    lcd.print(hum_almacenada);
    lcd.print(" %");
    delay(5000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dist: ");
    lcd.print(dist_almacenada);
    lcd.print(" cm");
    lcd.setCursor(0, 1);
    lcd.print("CO2: ");
    lcd.print(co2_almacenada);
    lcd.print(" ppm");
    delay(5000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Luz: ");
    lcd.print(lum_almacenada);
    mostrarEeprom = false;
    delay(2000);
  }*/
  delay(900);
}
// Variable para almacenar el último dato enviado
String ultimoDatoEnviado;

void enviarDatosSerial() {
  // Variables de almacenamiento para la lecturas de los sensores
  humedad = dht.readHumidity();  // Lectura de humedad
  temp = dht.readTemperature();  // Lectura de
  raw_data = analogRead(A0);     // Lectura de CO2
  lum_data = analogRead(A1);     // Lectura de Luminosidad
  long t;                        //timepo que demora en llegar el eco
  long d;                        //distancia en centimetros

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);  //Enviamos un pulso de 10us
  digitalWrite(trig, LOW);

  t = pulseIn(echo, HIGH);  //obtenemos el ancho del pulso
  d = t / 59;               // escalamos una distancia en cm.
  int estadoVentilador = digitalRead(FAN);
  int estadoFoco = digitalRead(RELE1);
 
  // Construir la cadena de datos
  String dataToSend = String(d ? d : 0) + "," + String(lum_data ? lum_data : 0) + "," + String(raw_data ? raw_data : 0) + "," + String(temp ? temp : 0) + "," + String(humedad ? humedad : 0) + "," + String(velocidad1 ? velocidad1 : 0) + "," + String(focoval ? focoval : 0) + "," + String(door ? door : 0) + "," + String(humanoval ? humanoval : 0);

  // Enviar los datos a través de la comunicación serial
  //Serial.println("Enviando datos: " + dataToSend);
  //mySerial.println(dataToSend);

  // Verificar si el nuevo dato es diferente al último enviado
  if (dataToSend != ultimoDatoEnviado) {
    // Enviar los datos solo si son diferentes
    mySerial.println(dataToSend);
    // Actualizar el valor del último dato enviado
    ultimoDatoEnviado = dataToSend;
    delay(50);
  }
}

void recibirSolicitudesMQTT() {
  // Variables de almacenamiento para la lecturas de los sensores
  humedad = dht.readHumidity();  // Lectura de humedad
  temp = dht.readTemperature();  // Lectura de
  raw_data = analogRead(A0);     // Lectura de CO2
  lum_data = analogRead(A1);     // Lectura de Luminosidad
  long t;                        //timepo que demora en llegar el eco
  long d;                        //distancia en centimetros

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);  //Enviamos un pulso de 10us
  digitalWrite(trig, LOW);

  t = pulseIn(echo, HIGH);  //obtenemos el ancho del pulso
  d = t / 59;               // escalamos una distancia en cm.
  //Acciones realizadas por petición MQTT
  // Actualizar el último dato enviado para filtrar duplicados

  if (mySerial.available() > 0) {
    String receivedData = mySerial.readString();
    receivedData.trim();  // Elimina espacios en blanco al principio y al final
    ultimoDatoEnviado = receivedData;
    Serial.println(receivedData);
    
    if (receivedData.equals("0")) {
      if (velocidad == false){
        digitalWrite(RELE2, HIGH);
        Serial.println("Encendiendo el ventilador rapido");
        velocidad1 = 2;
        velocidad = true;
        enviarDatosSerial();
      } else {
        digitalWrite(FAN, LOW);
        Serial.println("Apagando el ventilador");
        digitalWrite(RELE2, LOW);
        velocidad = false;
        velocidad1 = 0;
        enviarDatosSerial();
      }
      delay(100);
    } else if (receivedData.equals("1")) {
      digitalWrite(FAN, HIGH);
      velocidad1 = 1;
      Serial.println("Encendiendo el ventilador despacio");
      enviarDatosSerial();
      delay(100);
    } else if (receivedData.equals("2")) {
      enviarDatosSerial();
      delay(100);
    } else if (receivedData.equals("3")) {
      //Recuperar datos de la EEPROM.
      Serial.print("Mostrando datos almacenados en memoria.");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MOSTRANDO");
      lcd.setCursor(0, 1);
      lcd.print("DATOS");
      float dist_almacenada, lum_almacenada, co2_almacenada, temp_almacenada, hum_almacenada;
      recuperarDatos(dist_almacenada, lum_almacenada, co2_almacenada, temp_almacenada, hum_almacenada);
      // Construir la cadena de datos
      String dataToSendM = "eeprom" + String(dist_almacenada ? dist_almacenada : 0) + "," + String(lum_almacenada ? lum_almacenada : 0) + "," + String(co2_almacenada ? co2_almacenada : 0) + "," + String(temp_almacenada ? temp_almacenada : 0) + "," + String(hum_almacenada ? hum_almacenada : 0);

      // Enviar los datos a través de la comunicación serial
      Serial.println("Enviando datos: " + dataToSendM);
      mySerial.println(dataToSendM);
      enviarDatosSerial();
      delay(1000);
    } else if (receivedData.equals("4")) {
      //Guadar datos en la EEPROM.
      guardarDatos(d, lum_data, raw_data, temp, humedad);
      Serial.print("Datos almacenados en memoria.");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("GUARDANDO");
      lcd.setCursor(0, 1);
      lcd.print("DATOS");
      enviarDatosSerial();
      delay(1000);
    } else if (receivedData.equals("5")) {
    
      if (foco == false){
        digitalWrite(RELE1, LOW);
        focoval = 1;
        Serial.println("Encendiendo el foco");
        foco = true;
        delay(100);
        enviarDatosSerial();
      } else {
        digitalWrite(RELE1, HIGH);
        focoval = 0;
        Serial.println("Apagando el foco");
        foco = false;
        delay(100);
        enviarDatosSerial();
      }
      delay(100);
    } else if (receivedData.equals("6")) {
    
      if (puerta == false){
        Servo1.write(0);
        Serial.println("Abriendo la puerta");
        door = 1;
        puerta = true;
        delay(100);
        enviarDatosSerial();
      } else {
        Servo1.write(90);
        door = 0;
        Serial.println("Cerrando la puerta");
        puerta = false;
        delay(100);
        enviarDatosSerial();
      }
      delay(100);
    } 
  }
}





//==========================================================================================================
//==========================================================================================================
//=================================================== EEPROM ================================================
//==========================================================================================================
//==========================================================================================================


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

void estado_guardarEeprom() {
  guardarEeprom = true;
  delay(50);
}

void estado_mostrarEeprom() {
  mostrarEeprom = true;
  delay(50);
}


//==========================================================================================================
//==========================================================================================================
//===================================================== FIN ================================================
//==========================================================================================================
//==========================================================================================================
