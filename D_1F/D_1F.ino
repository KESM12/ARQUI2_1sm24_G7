#include<Wire.h>
#include<DHT.h>
#include<LiquidCrystal_I2C.h>

const int TH11 = 13; //
const int trig = 12; //
const int echo = 11; //
const int MT = 10; //
const int LA = 9; //
const int MC = 8; //
const int MP = 7; //
const int GE = 6; //

//VARIABLES 
int temp;
int humedad;
float distancia, duracion; 
int estadoBMT = 0;
int estadoBLA = 0;
int estadoBMC = 0;
int estadoBMP = 0;
int estadoBGE = 0;


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
  
  //aqui viene lo del lcd 
  lcd.setCursor(0,0);
  lcd.print("CO2:");
  lcd.setCursor(5,0); //columna, fila
  lcd.print(raw_data);
  Serial.println("que hay");
  // De aqui para abajo es solo un ejemplo de como deberias de hacer lo de los botones 
  // trate de hacerlos todos pero pues me dio hueva hacer lo de la eeprom pero ahi deje el boton. }
  // Como nota los siguientes botones solo tienen la validación de que los presionen a que voy con esto
  // hay que hacer mas validaciones y adaptarlo a lo de la lcd como lo de arriba pero pues en cada if
  // y para lo de la eeprom yo creo que deberias de preguntar o poner un boton que borre.
  if(estadoBMT == LOW){
    Serial.print(" Humedad: ");
    Serial.print(humedad);
    Serial.print("% Temperatura: ");
    Serial.print(temp);
    Serial.print(" C ");
    delay(200);
  }
  if(estadoBLA == HIGH){
    Serial.print(" Luminosidad = ");
    Serial.print(lum_data);
    delay(200);
  }
  if(estadoBMC == HIGH){
    Serial.print("Gas raw value = ");
    Serial.print(raw_data);
  }
  if(estadoBMP == HIGH){
    Serial.print("Distancia: ");
    Serial.print(distancia);
    Serial.println(" cm");
    delay(200);
  }
  if(estadoBGE == HIGH){
    //Pues aqui seria de guardar los datos en la eeprom.
    Serial.println("Mancos");
    delay(200);
  }
  //Nota final: ya lo probe pero la verdad que ya me dio hueva las validaciones de los botones son las que hay que hacer bien
  // porque lo que hace es que las vuelve a repetir igual voy a hacer un documento aparte explicando mas esto.
  delay(100);
}
