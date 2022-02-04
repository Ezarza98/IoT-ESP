/**
 * Este codigo va a ser el que vaya en el Arduino y va a servir para procesar los datos de los sensores usados
 */

//-------------SENSOR HUMEDAD Y TEMPERATURA---------------//
#include <dht_nonblocking.h> //Libreria
#define DHT_SENSOR_TYPE DHT_TYPE_11
static const int DHT_SENSOR_PIN = 2; //Pin al que conectamos el pin de datos del sensor
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE ); //Creacion del objeto representante del sensor
long UltimoEnvio;

//------------SENSOR DE LUMINOSIDAD----------------------//
#include <BH1750.h>
#include <Wire.h>
BH1750 sensor;
int IN1 = 8;      // pin digital 8 de Arduino a IN1 de modulo controlador
int IN2 = 9;      // pin digital 9 de Arduino a IN2 de modulo controlador
int IN3 = 10;     // pin digital 10 de Arduino a IN3 de modulo controlador
int IN4 = 11;     // pin digital 11 de Arduino a IN4 de modulo controlador
int movimiento; //Indica el sentido de giro en funcion del comando
int i=0; //Para indicar el avance
int modo1;
int modo2;
int luminoso;
int activo = 0;

int avance [4][4] =    // array 2D con la secuencia para el avance
{
  {1, 0, 0, 0},
  {0, 1, 0, 0},
  {0, 0, 1, 0},
  {0, 0, 0, 1}
};

int retroceso [4][4] =    // array 2D con la secuencia de retroceso
{
  {0, 0, 0, 1},
  {0, 0, 1, 0},
  {0, 1, 0, 0},
  {1, 0, 0, 0}
};

//---------------FUNCIONES SENSORES-----------------------//
static bool MuestreoAmbiente( float *temperature, float *humidity )
{
  static unsigned long TiempoMuestreo = millis( );
  if( millis( ) - TiempoMuestreo > 3000ul )
  {
    if( dht_sensor.measure( temperature, humidity ) == true )
    {
      TiempoMuestreo = millis( );
      return( true );
    }
  }
  return( false );
}

void MovMotor(int orden){
  if ( orden == 0){
  digitalWrite(IN1, LOW); //Paramos el motor por completo
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW); 
  }else if (orden ==1){  
    if(digitalRead(49)){  
     digitalWrite(IN1, avance[i][0]);  
     digitalWrite(IN2, avance[i][1]);
     digitalWrite(IN3, avance[i][2]);
     digitalWrite(IN4, avance[i][3]);
     delay(20);
     if (i==4){
       i=0;
     }else{
       i++;
     }
    }
  } else if (orden == 2){ 
    if (digitalRead(48)){   
     digitalWrite(IN1, retroceso[i][0]);  
     digitalWrite(IN2, retroceso[i][1]);
     digitalWrite(IN3, retroceso[i][2]);
     digitalWrite(IN4, retroceso[i][3]);
     delay(20);
     if (i==4){
       i=0;
     }else{
       i++;
     } 
    }   
  }
}

//-------------------------------------------------------//



void setup() {
  Serial.begin(115200);
  UltimoEnvio = millis();

  Wire.begin(); //Inicializacion de la comunicación I2C
  sensor.begin(); //Inicializacion del sensor

  pinMode(IN1, OUTPUT); //Pines para el driver del motor paso a paso
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(3, INPUT);//Para el tipo de control de persiana
  pinMode(4,INPUT);//Para el tipo de control de persiana

  pinMode(48,INPUT_PULLUP); //Fin de carrera inferior
  pinMode(49,INPUT_PULLUP); //Fin de carrera superior
}


void loop() {
  long now =millis();
//------------ENVIO TEMPERATURA Y HUMEDAD----------------//
  float Temperatura;
  float Humedad;
  if( MuestreoAmbiente( &Temperatura, &Humedad ) == true )
  {
    if ((now-UltimoEnvio)>60000){
    UltimoEnvio=now; 
    Serial.println(Temperatura,1);
    delay(1000);
    Serial.println(Humedad,1);                       
    }
  }
//------------------GESTION LUMINOSIDAD-----------------//
  unsigned int luz = sensor.readLightLevel();
  
  modo1=digitalRead(3);
  modo2=digitalRead(4);

  if ((modo1==0)and(modo2==0)){
    activo = 0;
  } else if ((modo1) and (modo2==0)){
      luminoso = 11;
      activo = 1;
   }else if((modo1==0) and (modo2)){
      luminoso = 500;
      activo = 1;
   }else if ((modo1) and (modo2)){
      luminoso = 1500;
      activo = 1;
   }
    
  if (activo==1){
    if (luz < (luminoso-50)) {
      movimiento = 1;
    }else if (luz >(luminoso+50)){
      movimiento = 2;
    }else{
      movimiento = 0;
    }
    }else if (activo==0){
      movimiento = 0;
    }
  MovMotor(movimiento); 
}
