#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <time.h>

// WiFi credentials
const char *ssid = "7u7";             // Replace with your WiFi name
const char *password = "A8151623a2";  // Replace with your WiFi password

// MQTT Broker settings
const int mqtt_port = 8883;                                     // MQTT port (TLS)
const char *mqtt_broker = "w2fea0a7.ala.us-east-1.emqxsl.com";  // EMQX broker endpoint
const char *mqtt_topic = "project2g7";                          // MQTT topic
const char *mqtt_username = "oscar";                            // MQTT username for authentication
const char *mqtt_password = "oscar";                            // MQTT password for authentication

// NTP Server settings
const char *ntp_server = "pool.ntp.org";  // Default NTP server// const char* ntp_server = "cn.pool.ntp.org"; // Recommended NTP server for users in China
const long gmt_offset_sec = 0;            // GMT offset in seconds (adjust for your time zone)
const int daylight_offset_sec = 0;        // Daylight saving time offset in seconds

// Variable para almacenar el último mensaje enviado por MQTT
String ultimoMensajeEnviado = "";

// WiFi and MQTT client initialization
BearSSL::WiFiClientSecure espClient;
PubSubClient mqtt_client(espClient);

// SSL certificate for MQTT broker
// Load DigiCert Global Root G2, which is used by EMQX Public Broker: broker.emqx.io
static const char ca_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=
-----END CERTIFICATE-----
)EOF";

// Load DigiCert Global Root CA ca_cert, which is used by EMQX Cloud Serverless Deployment
/*
static const char ca_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=
-----END CERTIFICATE-----
)EOF";
*/


// Function declarations
void connectToWiFi();

void connectToMQTT();

void syncTime();

void mqttCallback(char *topic, byte *payload, unsigned int length);


void setup() {
  Serial.begin(9600);
  connectToWiFi();
  syncTime();  // X.509 validation requires synchronization time
  mqtt_client.setServer(mqtt_broker, mqtt_port);
  mqtt_client.setCallback(mqttCallback);
  connectToMQTT();
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando al WiFi...");
  }
  Serial.println("WiFi Conectado");
}

void syncTime() {
  configTime(gmt_offset_sec, daylight_offset_sec, ntp_server);
  Serial.print("Waiting for NTP time sync: ");
  while (time(nullptr) < 8 * 3600 * 2) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Time synchronized");
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    Serial.print("Current time: ");
    Serial.println(asctime(&timeinfo));
  } else {
    Serial.println("Failed to obtain local time");
  }
}

void connectToMQTT() {
  BearSSL::X509List serverTrustedCA(ca_cert);
  espClient.setTrustAnchors(&serverTrustedCA);
  while (!mqtt_client.connected()) {
    if (WiFi.status() == WL_CONNECTED) {
      String client_id = "esp8266-client-" + String(WiFi.macAddress());
      Serial.printf("Conectando a MQTT Broker como %s.....\n", client_id.c_str());
      if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
        Serial.println("MQTT broker conectado");
        mqtt_client.subscribe(mqtt_topic);
        // Publish message upon successful connection
        mqtt_client.publish(mqtt_topic, "Hola EMQX I'm ESP8266 ^^");
      } else {
        char err_buf[128];
        espClient.getLastSSLError(err_buf, sizeof(err_buf));
        Serial.print("Error al conectar MQTT broker, rc=");
        Serial.println(mqtt_client.state());
        Serial.print("Error en SSL: ");
        Serial.println(err_buf);
        delay(5000);
      }
    }
    connectToWiFi();
    break;
  }
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}


void loop() {
  if (!mqtt_client.connected()) {
    connectToMQTT();
  }
  mqtt_client.loop();
  // Leer datos de la comunicación serial y enviarlos por MQTT
  while (Serial.available() > 0) {
    String serialData = Serial.readStringUntil('\n');  // Leer hasta encontrar un salto de línea
                                                       // Verificar si el mensaje recibido es diferente al último mensaje enviado

    // Verificar si el mensaje recibido es diferente al último mensaje enviado
    if (serialData != ultimoMensajeEnviado) {
      // Enviar el mensaje por MQTT solo si es diferente
      mqtt_client.publish(mqtt_topic, serialData.c_str());  // Convertir a const char* y enviar
      // Actualizar el último mensaje enviado
      ultimoMensajeEnviado = serialData;
    }
  }
}