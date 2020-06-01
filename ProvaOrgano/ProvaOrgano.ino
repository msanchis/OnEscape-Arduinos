#include <SPI.h>
#include <Ethernet.h> //Conexion Ethernet con carcasa W5100
#include <PubSubClient.h> //Conexion MQTT
#include <avr/wdt.h> // Incluir la librería de ATmel per a reiniciar el arduino

//Variable per veure els prints al monitor
boolean DEBUG = true;

//VARIABLES i objectes de la xarxa i client Mosquitto
byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xFA};//mac del arduino
IPAddress ip(192, 168, 68, 37); //Ip fija del arduino
IPAddress server(192, 168, 68, 1); //Ip del server de mosquitto

EthernetClient ethClient; //Interfaz de red ethernet
PubSubClient client(ethClient); //cliente MQTT 



int Led = 13 ;// Definimos el pin de alarma visual
int tecla1 = 22; // Definimos el pin de deteccion de llegada
int tecla2 = 23;
int tecla3 = 24;
int tecla4 = 25;
int tecla5 = 26; // Definimos el pin de deteccion de llegada
int tecla6 = 27;
int tecla7 = 28;
int tecla8 = 29;
int tecla9 = 30; // Definimos el pin de deteccion de llegada
int tecla10 = 31;
int tecla11 = 32;
int tecla12 = 33;
int tecla13 = 34; // Definimos el pin de deteccion de llegada
int tecla14 = 35;
int tecla15 = 36;
int tecla16 = 37;
int tecla17 = 38; // Definimos el pin de deteccion de llegada
int tecla18 = 39;
int tecla19 = 40;
int tecla20 = 41;

boolean AlarmaTecla1; // Definimos el nombre de la variable donde vamos a grabar las lecturas
boolean AlarmaTecla2;
boolean AlarmaTecla3;
boolean AlarmaTecla4;
boolean AlarmaTecla5; // Definimos el nombre de la variable donde vamos a grabar las lecturas
boolean AlarmaTecla6;
boolean AlarmaTecla7;
boolean AlarmaTecla8;
boolean AlarmaTecla9; // Definimos el nombre de la variable donde vamos a grabar las lecturas
boolean AlarmaTecla10;
boolean AlarmaTecla11;
boolean AlarmaTecla12;
boolean AlarmaTecla13; // Definimos el nombre de la variable donde vamos a grabar las lecturas
boolean AlarmaTecla14;
boolean AlarmaTecla15;
boolean AlarmaTecla16;
boolean AlarmaTecla17; // Definimos el nombre de la variable donde vamos a grabar las lecturas
boolean AlarmaTecla18;
boolean AlarmaTecla19;
boolean AlarmaTecla20;


boolean EstadoTecla1=0;
boolean EstadoTecla2=0;
boolean EstadoTecla3=0;
boolean EstadoTecla4=0;
boolean EstadoTecla5=0;
boolean EstadoTecla6=0;
boolean EstadoTecla7=0;
boolean EstadoTecla8=0;
boolean EstadoTecla9=0;
boolean EstadoTecla10=0;
boolean EstadoTecla11=0;
boolean EstadoTecla12=0;
boolean EstadoTecla13=0;
boolean EstadoTecla14=0;
boolean EstadoTecla15=0;
boolean EstadoTecla16=0;
boolean EstadoTecla17=0;
boolean EstadoTecla18=0;
boolean EstadoTecla19=0;
boolean EstadoTecla20=0;


unsigned long tiempoTecla1=0;
unsigned long tiempoTecla2=0;
unsigned long tiempoTecla3=0;
unsigned long tiempoTecla4=0;
unsigned long tiempoTecla5=0;
unsigned long tiempoTecla6=0;
unsigned long tiempoTecla7=0;
unsigned long tiempoTecla8=0;
unsigned long tiempoTecla9=0;
unsigned long tiempoTecla10=0;
unsigned long tiempoTecla11=0;
unsigned long tiempoTecla12=0;
unsigned long tiempoTecla13=0;
unsigned long tiempoTecla14=0;
unsigned long tiempoTecla15=0;
unsigned long tiempoTecla16=0;
unsigned long tiempoTecla17=0;
unsigned long tiempoTecla18=0;
unsigned long tiempoTecla19=0;
unsigned long tiempoTecla20=0;

void setup ()
{
  pinMode (Led, OUTPUT) ; // Definimos el pin de alarma visual como salida
  pinMode (tecla1, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla2, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla3, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla4, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla5, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla6, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla7, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla8, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla9, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla10, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla11, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla12, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla13, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla14, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla15, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla16, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla17, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla18, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla19, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla20, INPUT) ; // Definimos el pin de detección de la pulsación
  
  Serial.begin(9600);
  if (DEBUG) {
    Serial.println(F("Inicia Arduino"));
  }
  //MQTT
  client.setServer(server, 1883);
  client.setCallback(callback);


  //Ethernet.begin(mac);   
  Ethernet.begin(mac,ip);
  delay(1000);

  if (DEBUG) {
    Serial.println(F("connectant..."));
    Serial.println(Ethernet.localIP());
    Serial.println(F("Definició dels PINS de les tecles"));
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  char msg[length];
  if (DEBUG) {
    Serial.print(F("Message arrived ["));
    Serial.print(topic);    
    Serial.print(F("] "));
    
    for (int i=0;i<length;i++) {
      msg[i]=(char)payload[i];
      Serial.print((char)payload[i]);
    }  
    Serial.println();
  }
  int res=strcmp(topic,"sala2/tecla1");
  if (res == 0) {    
    if (DEBUG) {
      Serial.print(F("ENTRA a tecla1"));
    }                  
  }

  res=strcmp(topic,"sala2/reset");
  int resu = strcmp(topic,"sala2/resetOrgano");  
  if (res == 0 || resu == 0) { //RESET PLACA
      if (DEBUG) {
        Serial.print(F("ENTRA a reset o resetOrgano"));
      }
     wdt_enable(WDTO_15MS); // Configuramos el contador de tiempo para que se reinicie en 15ms
  }
}

void reconnect() {

  // Loop until we're reconnected
  while (!client.connected()) {
    if (DEBUG) {
      Serial.print(F("Intentant connecció MQTT..."));
    }
    
    // Attempt to connect
    if (client.connect("Organo")) {
      if (DEBUG) {
        Serial.println(F("connectat MQTT"));      
      }

      client.subscribe("sala2/reset"); 
      client.subscribe("sala2/resetOrgano"); 
      client.subscribe("sala2/tecla1");     
      
    } else {
      if (DEBUG) {
        Serial.print(F("FALLA, rc="));
        Serial.print(client.state());
      }
      //Serial.println(" try again in 5 seconds");
      // Wait 3 seconds before retrying
      delay(3000);
    }
  }

}



void loop ()
{
  AlarmaTecla1= digitalRead (tecla1) ; // Leemos el estado del pin asociado a la pulsación  
  AlarmaTecla2= digitalRead (tecla2) ; // Leemos el estado del pin asociado a la pulsación
  AlarmaTecla3= digitalRead (tecla3) ; // Leemos el estado del pin asociado a la pulsación
  AlarmaTecla4= digitalRead (tecla4) ; // Leemos el estado del pin asociado a la pulsación
  AlarmaTecla5= digitalRead (tecla5) ; // Leemos el estado del pin asociado a la pulsación  
  AlarmaTecla6= digitalRead (tecla6) ; // Leemos el estado del pin asociado a la pulsación
  AlarmaTecla7= digitalRead (tecla7) ; // Leemos el estado del pin asociado a la pulsación
  AlarmaTecla8= digitalRead (tecla8) ; // Leemos el estado del pin asociado a la pulsación
  AlarmaTecla9= digitalRead (tecla9) ; // Leemos el estado del pin asociado a la pulsación  
  AlarmaTecla10= digitalRead (tecla10) ; // Leemos el estado del pin asociado a la pulsación
  AlarmaTecla11= digitalRead (tecla11) ; // Leemos el estado del pin asociado a la pulsación
  AlarmaTecla12= digitalRead (tecla12) ; // Leemos el estado del pin asociado a la pulsación
  AlarmaTecla13= digitalRead (tecla13) ; // Leemos el estado del pin asociado a la pulsación  
  AlarmaTecla14= digitalRead (tecla14) ; // Leemos el estado del pin asociado a la pulsación
  AlarmaTecla15= digitalRead (tecla15) ; // Leemos el estado del pin asociado a la pulsación
  AlarmaTecla16= digitalRead (tecla16) ; // Leemos el estado del pin asociado a la pulsación
  AlarmaTecla17= digitalRead (tecla17) ; // Leemos el estado del pin asociado a la pulsación  
  AlarmaTecla18= digitalRead (tecla18) ; // Leemos el estado del pin asociado a la pulsación
  AlarmaTecla19= digitalRead (tecla19) ; // Leemos el estado del pin asociado a la pulsación
  AlarmaTecla20= digitalRead (tecla20) ; // Leemos el estado del pin asociado a la pulsación
  
  if (AlarmaTecla1) {    
    if (!EstadoTecla1 && (millis()-tiempoTecla1) > 300 ) {
      Serial.println("tecla1 cambio de estado a 1");      
      EstadoTecla1=1;
    }
  } else {
    if (EstadoTecla1) {
      Serial.println("tecla1 cambio de estado a 0");
      EstadoTecla1=0;   
      tiempoTecla1=millis();
    }
  }

 if (AlarmaTecla2) {    
    if (!EstadoTecla2 && (millis()-tiempoTecla2) > 300) {
      Serial.println("tecla2 cambio de estado a 1");      
      EstadoTecla2=1;
    }
  } else {
    if (EstadoTecla2) {
      Serial.println("tecla2 cambio de estado a 0");
      EstadoTecla2=0;   
      tiempoTecla2=millis();
    }
  }

 if (AlarmaTecla3 && (millis()-tiempoTecla3) > 300) {    
    if (!EstadoTecla3) {
      Serial.println("tecla3 cambio de estado a 1");      
      EstadoTecla3=1;
    }
  } else {
    if (EstadoTecla3) {
      Serial.println("tecla3 cambio de estado a 0");
      EstadoTecla3=0;   
      tiempoTecla3=millis();
    }
  }

  if (AlarmaTecla4 && (millis()-tiempoTecla4) > 300) {    
    if (!EstadoTecla4) {
      Serial.println("tecla4 cambio de estado a 1");      
      EstadoTecla4=1;
    }
  } else {
    if (EstadoTecla4) {
      Serial.println("tecla4 cambio de estado a 0");
      EstadoTecla4=0;   
      tiempoTecla4=millis();
    }
  }

  if (AlarmaTecla5) {    
    if (!EstadoTecla5 && (millis()-tiempoTecla5) > 300 ) {
      Serial.println("tecla5 cambio de estado a 1");      
      EstadoTecla5=1;
    }
  } else {
    if (EstadoTecla5) {
      Serial.println("tecla5 cambio de estado a 0");
      EstadoTecla5=0;   
      tiempoTecla5=millis();
    }
  }

 if (AlarmaTecla6) {    
    if (!EstadoTecla6 && (millis()-tiempoTecla6) > 300) {
      Serial.println("tecla6 cambio de estado a 1");      
      EstadoTecla6=1;
    }
  } else {
    if (EstadoTecla6) {
      Serial.println("tecla6 cambio de estado a 0");
      EstadoTecla6=0;   
      tiempoTecla6=millis();
    }
  }

 if (AlarmaTecla7 && (millis()-tiempoTecla7) > 300) {    
    if (!EstadoTecla7) {
      Serial.println("tecla7 cambio de estado a 1");      
      EstadoTecla7=1;
    }
  } else {
    if (EstadoTecla7) {
      Serial.println("tecla7 cambio de estado a 0");
      EstadoTecla7=0;   
      tiempoTecla7=millis();
    }
  }

  if (AlarmaTecla8 && (millis()-tiempoTecla8) > 300) {    
    if (!EstadoTecla8) {
      Serial.println("tecla8 cambio de estado a 1");      
      EstadoTecla8=1;
    }
  } else {
    if (EstadoTecla8) {
      Serial.println("tecla8 cambio de estado a 0");
      EstadoTecla8=0;   
      tiempoTecla8=millis();
    }
  }

  if (AlarmaTecla9) {    
    if (!EstadoTecla9 && (millis()-tiempoTecla9) > 300 ) {
      Serial.println("tecla9 cambio de estado a 1");      
      EstadoTecla9=1;
    }
  } else {
    if (EstadoTecla9) {
      Serial.println("tecla9 cambio de estado a 0");
      EstadoTecla9=0;   
      tiempoTecla9=millis();
    }
  }

 if (AlarmaTecla10) {    
    if (!EstadoTecla10 && (millis()-tiempoTecla10) > 300) {
      Serial.println("tecla10 cambio de estado a 1");      
      EstadoTecla10=1;
    }
  } else {
    if (EstadoTecla10) {
      Serial.println("tecla10 cambio de estado a 0");
      EstadoTecla10=0;   
      tiempoTecla10=millis();
    }
  }

 if (AlarmaTecla11 && (millis()-tiempoTecla11) > 300) {    
    if (!EstadoTecla11) {
      Serial.println("tecla11 cambio de estado a 1");      
      EstadoTecla11=1;
    }
  } else {
    if (EstadoTecla11) {
      Serial.println("tecla11 cambio de estado a 0");
      EstadoTecla11=0;   
      tiempoTecla11=millis();
    }
  }

  if (AlarmaTecla12 && (millis()-tiempoTecla12) > 300) {    
    if (!EstadoTecla12) {
      Serial.println("tecla12 cambio de estado a 1");      
      EstadoTecla12=1;
    }
  } else {
    if (EstadoTecla12) {
      Serial.println("tecla12 cambio de estado a 0");
      EstadoTecla12=0;   
      tiempoTecla12=millis();
    }
  }

  if (AlarmaTecla13) {    
    if (!EstadoTecla13 && (millis()-tiempoTecla13) > 300 ) {
      Serial.println("tecla13 cambio de estado a 1");      
      EstadoTecla13=1;
    }
  } else {
    if (EstadoTecla13) {
      Serial.println("tecla13 cambio de estado a 0");
      EstadoTecla13=0;   
      tiempoTecla13=millis();
    }
  }

 if (AlarmaTecla14) {    
    if (!EstadoTecla14 && (millis()-tiempoTecla14) > 300) {
      Serial.println("tecla14 cambio de estado a 1");      
      EstadoTecla14=1;
    }
  } else {
    if (EstadoTecla14) {
      Serial.println("tecla14 cambio de estado a 0");
      EstadoTecla14=0;   
      tiempoTecla14=millis();
    }
  }

 if (AlarmaTecla15 && (millis()-tiempoTecla15) > 300) {    
    if (!EstadoTecla15) {
      Serial.println("tecla15 cambio de estado a 1");      
      EstadoTecla15=1;
    }
  } else {
    if (EstadoTecla15) {
      Serial.println("tecla15 cambio de estado a 0");
      EstadoTecla15=0;   
      tiempoTecla15=millis();
    }
  }

  if (AlarmaTecla16 && (millis()-tiempoTecla16) > 300) {    
    if (!EstadoTecla16) {
      Serial.println("tecla16 cambio de estado a 1");      
      EstadoTecla16=1;
    }
  } else {
    if (EstadoTecla16) {
      Serial.println("tecla16 cambio de estado a 0");
      EstadoTecla16=0;   
      tiempoTecla16=millis();
    }
  }

  if (AlarmaTecla17) {    
    if (!EstadoTecla17 && (millis()-tiempoTecla17) > 300 ) {
      Serial.println("tecla17 cambio de estado a 1");      
      EstadoTecla17=1;
    }
  } else {
    if (EstadoTecla17) {
      Serial.println("tecla17 cambio de estado a 0");
      EstadoTecla17=0;   
      tiempoTecla17=millis();
    }
  }

 if (AlarmaTecla18) {    
    if (!EstadoTecla18 && (millis()-tiempoTecla18) > 300) {
      Serial.println("tecla18 cambio de estado a 1");      
      EstadoTecla18=1;
    }
  } else {
    if (EstadoTecla18) {
      Serial.println("tecla18 cambio de estado a 0");
      EstadoTecla18=0;   
      tiempoTecla18=millis();
    }
  }

 if (AlarmaTecla19 && (millis()-tiempoTecla19) > 300) {    
    if (!EstadoTecla19) {
      Serial.println("tecla19 cambio de estado a 1");      
      EstadoTecla19=1;
    }
  } else {
    if (EstadoTecla19) {
      Serial.println("tecla19 cambio de estado a 0");
      EstadoTecla19=0;   
      tiempoTecla19=millis();
    }
  }

  if (AlarmaTecla20 && (millis()-tiempoTecla20) > 300) {    
    if (!EstadoTecla20) {
      Serial.println("tecla20 cambio de estado a 1");      
      EstadoTecla20=1;
    }
  } else {
    if (EstadoTecla20) {
      Serial.println("tecla20 cambio de estado a 0");
      EstadoTecla20=0;   
      tiempoTecla20=millis();
    }
  }
  
  if (AlarmaTecla1 || AlarmaTecla2 || AlarmaTecla3 || AlarmaTecla4 || AlarmaTecla5 || AlarmaTecla6 || AlarmaTecla7 || AlarmaTecla8 || AlarmaTecla9 || AlarmaTecla10 || AlarmaTecla11 || AlarmaTecla12 || AlarmaTecla13 || AlarmaTecla14 || AlarmaTecla15 || AlarmaTecla16 || AlarmaTecla17 || AlarmaTecla18 || AlarmaTecla19 || AlarmaTecla20) digitalWrite (Led, HIGH); // encendiendo el LED de alarma que tenemos asociados al pin 13,  si no....
  else digitalWrite (Led, LOW); // el LED se mantiene apagado.

  if (!client.connected()) {
      reconnect();
  }
  client.loop();
  
}
