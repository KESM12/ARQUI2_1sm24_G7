#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <PubSubClient.h>
#include "SoftwareSerial.h"

//Conexión a la red wifi: nombre de la red y contraseña
#define WIFI_AP "7u7"
#define WIFI_PASSWORD "A8151623a2"

//Nombre o IP del servidor mosquitto
char *server = "broker.emqx.io";

//Inicializamos el objeto de cliente esp
WiFiEspClient espClient;

//Iniciamos el objeto subscriptor del cliente 
//con el objeto del cliente
PubSubClient client(espClient);

//Conexión serial para el esp con una comunicación
//serial, pines 2: rx y 3: tx
SoftwareSerial soft(8, 10);


//PIN para el ventilador
int fanPin = 9;

//Contador para el envio de datos
unsigned long lastSend;

int status = WL_IDLE_STATUS;

String resultS = "";
int var = 0;


//Función para recibir mensajes desde el broker
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("] ");

  char payload_string[length + 1];
  
  int resultI;

  memcpy(payload_string, payload, length);
  payload_string[length] = '\0';
  resultI = atoi(payload_string);
  
  var = resultI;

  resultS = "";
  
  for (int i=0;i<length;i++) {
    resultS= resultS + (char)payload[i];
     Serial.println(resultS);
  }

 
  
  // Control del ventilador basado en el mensaje recibido
  if (strcmp(topic, "ENTRADA") == 0) {
    if (var == 1) {
      // Encender el ventilador
      digitalWrite(fanPin, HIGH);
    } else {
      // Apagar el ventilador
      digitalWrite(fanPin, LOW);
    }
  }
}



void setup() {
    //Inicializamos la comunicación serial para el log
    Serial.begin(9600);
    
    // Pin Mode 
    pinMode(fanPin, OUTPUT);

    //Iniciamos la conexión a la red WiFi
    InitWiFi();
    //Colocamos la referencia del servidor y el puerto
    client.setServer( server, 1883 );
    client.setCallback(callback);
    client.subscribe("ENTRADAINO"); 
    lastSend = 0;
}

void loop() {
    //Validamos si el modulo WiFi aun esta conectado a la red
    status = WiFi.status();
    if(status != WL_CONNECTED) {
        //Si falla la conexión, reconectamos el modulo
        reconnectWifi();
    }

    //Validamos si esta la conexión del servidor
    if(!client.connected() ) {
        //Si falla reintentamos la conexión
        client.subscribe("ENTRADA");
        reconnectClient();
    }

    //Creamos un contador para enviar la data cada 2 segundos
    if(millis() - lastSend > 2000 ) {
        sendDataTopic();
        lastSend = millis();
    }

    client.loop();
    
     if(var == 0){
    digitalWrite(fanPin, LOW);
    } else if (var == 1) {
      digitalWrite(fanPin, HIGH);
    }
}

void sendDataTopic() {
    Serial.println("Enviando Datos");
    String payload = "  PTM NO SALE COMPI 2 ";
    

    // Publicar el payload como texto plano en el topic "FAN/0"
    client.publish("FAN", payload.c_str());

    Serial.println(payload);
}


//Inicializamos la conexión a la red wifi
void InitWiFi()
{
    //Inicializamos el puerto serial
    soft.begin(9600);
    //Iniciamos la conexión wifi
    WiFi.init(&soft);
    //Verificamos si se pudo realizar la conexión al wifi
    //si obtenemos un error, lo mostramos por log y denememos el programa
    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("El modulo WiFi no esta presente");
        while (true);
    }
    reconnectWifi();
}

void reconnectWifi() {
    Serial.println("Iniciar conección a la red WIFI");
    while(status != WL_CONNECTED) {
        Serial.print("Intentando conectarse a WPA SSID: ");
        Serial.println(WIFI_AP);
        //Conectar a red WPA/WPA2
        status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
        delay(500);
    }
    Serial.println("Conectado a la red WIFI");
}

void reconnectClient() {
    //Creamos un loop en donde intentamos hacer la conexión
    while(!client.connected()) {
        Serial.print("Conectando a: ");
        Serial.println(server);
        //Creamos una nueva cadena de conexión para el servidor
        //e intentamos realizar la conexión nueva
        //si requiere usuario y contraseña la enviamos connect(clientId, username, password)
        String clientId = "ESP8266Client-" + String(random(0xffff), HEX);
        if(client.connect(clientId.c_str())) {
            Serial.println("[DONE]");
        } else {
            Serial.print( "[FAILED] [ rc = " );
            Serial.print( client.state() );
            Serial.println( " : reintentando en 5 segundos]" );
            delay( 5000 );
        }
    }
}