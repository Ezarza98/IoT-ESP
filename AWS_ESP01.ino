#include "FS.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

//Nombre del WIFI y la contraseña del mismo.
const char* ssid = "*******";
const char* password = "*********";

int activar; //para el control automatico
int manual; //para el control manual
int preferencia; //Esta variable da preferencia a la orden manual o automatica segun sea la ultima dicha
int horario; //Esta va a guardar el tipo de horario en el que estamos
int retraso=2; //Esta va a guardar el retraso respecto a la hora de centroeuropa

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 0);
//El endpointpara publicar
const char* AWS_endpoint = "**********************************"; //MQTT broker EndpoInt que seria como el ID de un broker normal
//Creacion de los clientes 
WiFiClientSecure espClient;
void callback(char* topic, byte* payload, unsigned int length);
PubSubClient client(AWS_endpoint, 8883, callback, espClient);
 
//Dentro de esta funcion se organizan todas las detecciones de los topicos a los que se suscribe el chip
void callback(char* topic, byte* payload, unsigned int length) {
  char let;                  //aqui guardamos caracter por caracter
  String mensaje;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    let=(char)payload[i];
    if (i==1){
     mensaje=let; //solo nos interesa el mensaje del activacion
    }
  }
  Serial.println(mensaje);

  if (strcmp(topic,"MyTopic/iot/casa/caldera/control")==0){
    activar=mensaje.toInt(); //pasamos lo recibido a INT para identificar si esta activo o no
    preferencia = 0;
  } else if (strcmp(topic, "MyTopic/iot/casa/caldera/manual")==0){
    manual=mensaje.toInt();
    preferencia = 1;
  } else if (strcmp(topic,"MyTopic/iot/casa/caldera/horario")==0){
    horario = mensaje.toInt(); //Pasamos a guardar en INT lo recibido
    if (horario == 1){ //Modo horario de verano
      retraso =2 ;
    }else if (horario == 0){//Modo horario de invierno
      retraso =1;
    } else { //Fallo en el entendimiento
      retraso = 2;
    }   
  } 
}


void setup_wifi() {
  delay(10);
// We start by connecting to a WiFi network
  espClient.setBufferSizes(512, 512);
  Serial.println();
  Serial.print("Conectando a: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
  while(!timeClient.update()){
    timeClient.forceUpdate();
  }

  espClient.setX509Time(timeClient.getEpochTime());
}


void reconnect() {
// Si se desconecta del wifi vuelve a conectar
  while (!client.connected()) {
    Serial.print("Esperando conexión a MQTT...");
    // Attempt to connect
    if (client.connect("ESPthing")) {
      Serial.println("Conectado");
     // Cuando estamos conectados nos anunciamos que estamos readies
      client.publish("MyTopic/iot/casa", "hello world");
    //SUSCRIPCION A LOS TÓPICOS PARA CONTROLAR LA CALDERA(Cosas de ON/OFF)
      client.subscribe("MyTopic/iot/casa/caldera/control");
      client.subscribe("MyTopic/iot/casa/caldera/manual");
      client.subscribe("MyTopic/iot/casa/caldera/horario");
    } else {
      Serial.print("Fallido, rc=");
      Serial.print(client.state());
      Serial.println(" Probando de nuevo en 5s");

      char buf[256];
      espClient.getLastSSLError(buf,256);
      Serial.print("WiFiClientSecure SSL error: ");
      Serial.println(buf);

      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  setup_wifi();
  delay(1000);
  if (!SPIFFS.begin()) {
    Serial.println("Fallo al cargar archivos");
    return;
  }

  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());

  // Load certificate file
  File cert = SPIFFS.open("/cert.der", "r"); //replace cert.crt eith your uploaded file name
  if (!cert) {
    Serial.println("Fallo al abrir cert.der");
  }else {
  Serial.println("Exito al abrir cert.der");
  };
  delay(1000);
  if (espClient.loadCertificate(cert)){
    Serial.println("cert cargado");
  } else{
    Serial.println("cert no cargado");
  };
  
// Load private key file
  File private_key = SPIFFS.open("/private.der", "r"); //replace private eith your uploaded file name
  if (!private_key) {
    Serial.println("Fallo al abrir private.der");
  } else{
    Serial.println("Exito al abrir private.der");
  }
  delay(1000);
  if (espClient.loadPrivateKey(private_key)){
    Serial.println("private.der cargado");
  }else {
    Serial.println("privated.der no cargado");
  };
  
  // Load CA file
  File ca = SPIFFS.open("/ca.der", "r"); //replace ca eith your uploaded file name
  if (!ca) {
    Serial.println("Fallo al abrir ca.der ");
  }else{
    Serial.println("Exito al abrir ca.der");
  };
  delay(1000);

  if(espClient.loadCACert(ca)){
    Serial.println("ca.der exito");
  } else {
    Serial.println("ca.der fallido");
  }
  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());


  pinMode(2,OUTPUT);
}



void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  timeClient.update(); 
  if (preferencia == 0){
    if (activar){
      if ( ((((timeClient.getHours() + retraso)>=24) or ((timeClient.getHours() + retraso)>=0) ) and ((timeClient.getHours() + retraso)<10)) or 
         (((timeClient.getHours() + retraso)>=14) and ((timeClient.getHours() + retraso)<18)) or 
         (((timeClient.getHours() + retraso)>=22) and ((timeClient.getHours() + retraso)<24))){
          digitalWrite(2,HIGH);
          Serial.println("activo auto ON");
         }else {
          digitalWrite(2,LOW);
          Serial.println("Activa auto OFF");
         }
   }else if (activar==0){
    digitalWrite(2,LOW);
    Serial.println("ACTIVAR AUTO");
   }
  } else if (preferencia){
    if (manual){
      digitalWrite(2,HIGH);
      Serial.println("Activar manual ON");
    }else if(manual==0){
      digitalWrite(2,LOW);
      Serial.println("Activar manual OFF");
    }}
    Serial.println((timeClient.getHours() + retraso));
    delay(1000);}
