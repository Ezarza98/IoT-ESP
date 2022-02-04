#include "FS.h"
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


//Nombre del WIFI y la contraseña del mismo.
const char* ssid = "***********";
const char* password = "************";
//Variables generales
char msg[150];
int NumeroEnvio=0;
int Temperatura;
int Humedad;
int modo;

//Esto es para sincronizar con reloj para la verificacion de los certificados
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
//El endpointpara publicar
const char* AWS_endpoint = "*****************************"; //MQTT broker EndpoInt que seria como el ID de un broker normal
//Creacion de los clientes 
WiFiClientSecure espClient;
void callback(char* topic, byte* payload, unsigned int length);
PubSubClient client(AWS_endpoint, 8883, callback, espClient);
//Para ThingSpeak
WiFiClient  ThingClient;

unsigned long myChannelNumber = 1309498;
const char * myWriteAPIKey = "CD2L2EK6S561EDTH";
 
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
     mensaje=let;
    }
  }
  Serial.println(mensaje); 
  if (strcmp(topic,"MyTopic/iot/casa/cocina/luz/encender")==0){
    digitalWrite(16, HIGH);
  }else if (strcmp(topic,"MyTopic/iot/casa/cocina/luz/apagar")==0){
    digitalWrite(16, LOW);    
    } else if (strcmp(topic,"MyTopic/iot/casa/habitación/luz/encender")==0){
    digitalWrite(5, HIGH); 
  }else if (strcmp(topic,"MyTopic/iot/casa/habitación/luz/apagar")==0){
    digitalWrite(5, LOW);      
  }else if (strcmp(topic,"MyTopic/iot/casa/patio/luz/encender")==0){
    digitalWrite(4, HIGH); 
  }else if (strcmp(topic,"MyTopic/iot/casa/patio/luz/apagar")==0){
    digitalWrite(4, LOW);      
  }else if (strcmp(topic,"MyTopic/iot/casa/baño/luz/encender")==0){
    digitalWrite(0, HIGH); 
  }else if (strcmp(topic,"MyTopic/iot/casa/baño/luz/apagar")==0){
    digitalWrite(0, LOW);      
  }else if (strcmp(topic,"MyTopic/iot/casa/salón/luz/encender")==0){
    digitalWrite(12, HIGH); 
  }else if (strcmp(topic,"MyTopic/iot/casa/salón/luz/apagar")==0){
    digitalWrite(12, LOW);      
  }else if (strcmp(topic,"MyTopic/iot/casa/luz/encender")==0){
    digitalWrite(12, HIGH);
    digitalWrite(0, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(16, HIGH);    
  }else if (strcmp(topic,"MyTopic/iot/casa/luz/apagar")==0){
    digitalWrite(12, LOW);
    digitalWrite(0, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    digitalWrite(16, LOW);    
  } else if (strcmp(topic, "MyTopic/iot/casa/persianas")==0){
    modo=mensaje.toInt(); 
    if (modo==0){       
      Serial.println(modo);
      Serial.printf("modo control desactivado");
      //Serial1.print(modo); 
      digitalWrite(13,LOW);
      digitalWrite(15,LOW);      
    } else if (modo==1){       
      Serial.println(modo);
      Serial.printf("modo control activado");
      //Serial1.print(modo);
      digitalWrite(13,HIGH);
      digitalWrite(15,LOW);
    } else if (modo==2){ //modo de luz alto     
      Serial.println(modo);
      Serial.printf("modo de luz alto");
      //Serial1.print(modo);
      digitalWrite(13,HIGH);
      digitalWrite(15,HIGH);      
    } else if (modo==3){       
      Serial.println(modo);
      Serial.printf("modo de luz medio");
      //Serial1.print(modo);
      digitalWrite(13,HIGH);
      digitalWrite(15,LOW);      
    } else if (modo==4){       
      Serial.println(modo);
      Serial.printf("modo de luz bajo");
      //Serial1.print(modo);     
      digitalWrite(13,LOW);
      digitalWrite(15,HIGH);      
    }
  }  
  /*else if (strcmp(topic,"MyTopic/iot/casa/sensor/temperatura")==0){    
    snprintf (msg, 75, "{\"message\": \"%1d\", \"topic\": \"iot/topic/casa/sensor/temperatura\", \"session\":\"\", \"request\": { \"type\": \"InvocacionIoT\"} }", Temperatura);
    client.publish("iot/topic/casa/sensor/temperatura", msg);    
  }else if (strcmp(topic,"MyTopic/iot/casa/sensor/humedad")==0){
    snprintf (msg, 75, "{\"message\": \"%1d\", \"topic\": \"iot/topic/casa/sensor/humedad\", \"session\":\"\", \"request\": { \"type\": \"InvocacionIoT\"} }", Humedad);
    client.publish("iot/topic/casa/sensor/humedad", msg); */   
   else if (strcmp(topic,"MyTopic/iot/casa/sensor")==0){
    //CREAMOS EL MENSAJE A ENVIAR CON LOS DATOS NECESARIOS Y EN FORMATO JSON
    snprintf (msg, 75, "{\"message\":\"%1d\",\"topic\":\"T\",\"session\":\"\",\"request\":{\"type\":\"IoT\"}}", Temperatura);
    client.publish("iot/topic/casa/sensor/temperatura", msg);
    delay(250);
    snprintf (msg, 75, "{\"message\":\"%1d\",\"topic\":\"H\",\"session\":\"\",\"request\":{\"type\":\"IoT\"}}", Humedad);
    client.publish("iot/topic/casa/sensor/humedad", msg);
  } 
  };
   //usamos el puerto 883 ya que es el predeterminado
 /* if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }*/





void setup_wifi() {
  delay(10);
// We start by connecting to a WiFi network
  espClient.setBufferSizes(512, 512);
  Serial.println();
  Serial.print("Conectando a ");
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
    Serial.print("Conectando a MQTT...");
    // Attempt to connect
    if (client.connect("ESPthing")) {
      Serial.println("Conectado");
     // Cuando estamos conectados nos anunciamos que estamos readies
      client.publish("MyTopic/iot/casa", "conexion correcta");

    //SUSCRIPCION A LOS TÓPICOS PARA CONTROLAR LUCES/RELÉS(Cosas de ON/OFF)
      client.subscribe("MyTopic/iot/casa/baño/luz/encender");
      client.subscribe("MyTopic/iot/casa/baño/luz/apagar");
      client.subscribe("MyTopic/iot/casa/salón/luz/encender");
      client.subscribe("MyTopic/iot/casa/salón/luz/apagar");
      client.subscribe("MyTopic/iot/casa/cocina/luz/encender");
      client.subscribe("MyTopic/iot/casa/cocina/luz/apagar");
      client.subscribe("MyTopic/iot/casa/patio/luz/encender");
      client.subscribe("MyTopic/iot/casa/patio/luz/apagar");
      client.subscribe("MyTopic/iot/casa/habitación/luz/encender");
      client.subscribe("MyTopic/iot/casa/habitación/luz/apagar");
      client.subscribe("MyTopic/iot/casa/luz/encender");
      client.subscribe("MyTopic/iot/casa/luz/apagar");
      client.subscribe("MyTopic/iot/casa/persianas");

    //SUSCRIPCION A LOS TÓPICOS PARA LOS SENSORES
      client.subscribe("MyTopic/iot/casa/sensor/temperatura");
      client.subscribe("MyTopic/iot/casa/sensor/humedad");
      client.subscribe("MyTopic/iot/casa/sensor");
    } else {
      Serial.print("Fallo, rc=");
      Serial.print(client.state());
      Serial.println(" Probando en 5 segundos ");

      char buf[256];
      espClient.getLastSSLError(buf,256);
      Serial.print("Error WiFiClientSecure SSL: ");
      Serial.println(buf);

      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



void setup() {

  Serial.begin(115200);  //Inicializamos el puerto UART
  Serial.setDebugOutput(true);
  
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  setup_wifi();
  delay(1000);
  if (!SPIFFS.begin()) {
    Serial.println("Fallo al cargar los certificados");
    return;
  }

  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());

  // Load certificate file
  File cert = SPIFFS.open("/cert.der", "r"); //replace cert.crt eith your uploaded file name
  if (!cert) {
    Serial.println("Fallo al abrir archivo cert.der");
  }else {
  Serial.println("Archivo cert.der abierto con exito");
  };
  delay(1000);
  if (espClient.loadCertificate(cert)){
    Serial.println("cert.der cargado");
  } else{
    Serial.println("cert.der no cargado");
  };
  
// Load private key file
  File private_key = SPIFFS.open("/private.der", "r"); //replace private eith your uploaded file name
  if (!private_key) {
    Serial.println("Fallo al abrir certificado private.der");
  } else{
    Serial.println("private.cert abierto con exito");
  }
  delay(1000);
  if (espClient.loadPrivateKey(private_key)){
    Serial.println("private.der cargado");
  }else {
    Serial.println("private.der no cargado");
  };
  
  // Load CA file
  File ca = SPIFFS.open("/ca.der", "r"); //replace ca eith your uploaded file name
  if (!ca) {
    Serial.println("Fallo al abrir ca.der ");
  }else{
    Serial.println("ca.der abierto con exito");
  };
  delay(1000);

  if(espClient.loadCACert(ca)){
    Serial.println("ca.der cargado");
  } else {
    Serial.println("ca.der fallido");
  }
  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());

  ThingSpeak.begin(ThingClient);  // Initialize ThingSpeak  //ThingSpeak
  
  //Pines para los relés
  pinMode(16,OUTPUT);  //pin D0
  pinMode(5,OUTPUT);   //pin D1
  pinMode(4,OUTPUT);   //pin D2
  pinMode(0,OUTPUT);   //pin D3
  pinMode(12,OUTPUT);  //pin D6

  //Pin para el control de luminosidad
  pinMode(13,OUTPUT); //pin D7
  pinMode(15,OUTPUT);  //pin D8
  
  digitalWrite(13,LOW);
  digitalWrite(15,LOW);
  
  
}

void loop() {
//Si no estamos conectados a internet reconectamos
  if (!client.connected()) {
    reconnect();
  }

//Esta función nos permite estar atentos a las recepciones del topic
  client.loop();

//----RECEPCION DE LOS DATOS DEL SENSOR----------//
  if (Serial.available()>0){
    if (NumeroEnvio==0){
      NumeroEnvio=1;
      char letra;                  //aqui guardamos caracter por caracter
      String frase;
      while(Serial.available()>0){    //mientras estemos recibiendo datos del monitor serial
        letra=Serial.read();          //guardamos en letra la letra actual
        frase+=letra;              //añadimos a frase la letra y asi sucesivamente hasta que frase tenga todo lo escrito
        delay(100);
       }
       Temperatura=frase.toInt();
       
       Serial.println(Temperatura);
       
    }else if (NumeroEnvio==1){
      NumeroEnvio=0;
      char letra;                  //aqui guardamos caracter por caracter
      String frase;
      while(Serial.available()>0){    //mientras estemos recibiendo datos del monitor serial
        letra=Serial.read();          //guardamos en letra la letra actual
        frase+=letra;              //añadimos a frase la letra y asi sucesivamente hasta que frase tenga todo lo escrito
        delay(100);
       }
       Humedad=frase.toInt();
       
       Serial.println(Humedad);
       
       //Ahora el envío cada minuto de los datos de los sensores
       snprintf (msg, 75, "{\"message\":\"%1d\",\"topic\":\"T\",\"session\":\"\",\"request\":{\"type\":\"IoT\"}}", Temperatura);
      //ENVIAMOS EL MENSAJE AL TOPICO DE RECEPCION DESEADO
      client.publish("iot/topic/casa/sensor/temperatura", msg);
      delay(250);
      //CREAMOS EL MENSAJE A ENVIAR CON LOS DATOS NECESARIOS Y EN FORMATO JSON
      snprintf (msg, 75, "{\"message\":\"%1d\",\"topic\":\"H\",\"session\":\"\",\"request\":{\"type\":\"IoT\"}}", Humedad);
      //ENVIAMOS EL MENSAJE AL TOPICO DE RECEPCION DESEADO
      client.publish("iot/topic/casa/sensor/humedad", msg);

//-------------------SUBIMOS LOS DATOS A THINGSPEAK----------------//
      ThingSpeak.setField(1, Temperatura);
      ThingSpeak.setField(2, Humedad);
      int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      if(x == 200){
        Serial.println("Actualizacion completada.");
      }else{
        Serial.println("Problema al actualizar. HTTP codigo de error " + String(x));
      }
    }
  }
//-------------------------------------------//
  
}
