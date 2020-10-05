/* Fecha Modificacio : 8/6/2020
 *  Instalat 8/6/2020
 * La variable estat s'utilitza per determinar en quin moment de la sala està actualment
 * Es poden arribar a cada estat directament amb els següents missatges 
 * estat = 0 <-- al rebre "sala1/reinici" 
 * estat = 1 <-- al rebre "sala1/posta1"
 * ...
 * estat = 5 <-- al rebre "sala1/posta5"
 * estat = 6 <-- al rebre "sala1/fin"
 * estat = 7 <-- al rebre "sala1/final"
 * ********************************************************************************************************
 * Aquest es l'ordre normal per passar d'un estat a un altre
 * estat = 0 --> Encara no ha iniciat el joc --> al rebre "sala1/inici" passa al següent estat
 * estat = 1 --> Estan realitzant la prova 1 (posta1) --> envia "sala1/casco0" i passa al següent estat

 * estat = 2 ....
 * ...
 * estat = 5 --> Estan realitzant la prova 5 (posta5) --> envia "sala1/casco5" i passa al següent estat
 * estat = 6 --> Han accedit al <casco alado> i van a obrir la porta -- envia "sala1/final"
 * estat = 7 --> Han finalitzat el joc
 */

#include <SPI.h>
#include <Ethernet.h> //Conexion Ethernet con carcasa W5100
#include <PubSubClient.h> //Conexion MQTT
#include <NewPing.h>
#include <avr/wdt.h> // Incluir la librería de ATmel
// #include "Distancia.h"
 
#define SONAR_NUM     5 // Number or sensors.
#define MAX_DISTANCE 150 // Max distance in cm.
 
//VARIABLES NECESSARIES PER A LES PROVES
#define NUMSIMON2 3 //Nombre de postes que s'iluminen per a superar la prova2
#define NUMSIMON4 3 //Nombre de postes que s'iluminen per a superar la prova4
#define NUMSIMON5 4 //Nombre de postes del simon final (ha de ser major o igual que NUMSIMON4 per a que funcione correctament)

int tempsEspera1=800; //Temps d'espera davant dels sensors per tal de desactivar-los
int tempsEspera2=4000; //Temps de visualització dels estats de transició
int tempsEspera3=1500; //Temps de visualització de les postes encertades prova2, prova4 i prova5

int arrayPostes[NUMSIMON2]; //Array per a guardar els numeros random per a la prova2
int estatPosta[NUMSIMON2]; //Array per a realitzar la prova2
int index=0; //Variable per a recorrer els arrays de estats i de postes prova2
unsigned long marcaTemps=0; //Variable de temps per a la prova2
//Distancia miDistancia; //Objecte per a medir distancia prova2

int arrayPostesVar[NUMSIMON5]; //Array per guardar els numeros randoms de posta que s'iluminaran a les proves 4 i 5
int arrayRandomDist[NUMSIMON5]; //Array per guardar les distancies aleatories de cada posta a les proves 4 i 5
int estatPostaVar[5]; // Array per realitzar quasi totes les proves
int arrayPrueba4[5]={0,4,1,3,2};  //Array prova4
int arrayOpuestoPrueba4[6]={4,0,4,0,4,0}; //Array opuesto prova4
//Determina el temps entre el parpadeig dels leds
int delayLeds= 1000; //INICI de la sala
int delayLeds1=300;

int estat=0; //La VARIABLE més important de la sala... determina l'estat en que es troba el sistema

boolean ultimMinut = false; //Variable per saber si s'ha executat l'event de l'ultim Minut
unsigned long iniciUltimMinut = 0; //Inici de l'ultim minut

int distMin=0; //Variables para determinar la distancia mínima postes variables 2, 4 i 5
int distMax=0; //Variables para determinar la distancia máxima postes variables 2, 4 i 5

//Variable per començar el joc despres de sala1/inici TEMPS ESTABLIT després de tancar porta 35 segs
unsigned long tiempoInicio = 0;


//Variables per a les proves 1 i 3
unsigned long marcaTemps11 = 0;
unsigned long marcaTemps1 = 0;
unsigned long tempsProva2 = 0;
unsigned long tempsProva3 = 0;
unsigned long tempsProva4 = 0;
unsigned long tempsProva5 = 0;

unsigned long tempsp3 = 0;

int estatPosta1=0;
//Distancia miDistancia1;

unsigned long marcaTemps21 = 0;
unsigned long marcaTemps2 = 0;
int estatPosta2=0;
//Distancia miDistancia2;

unsigned long marcaTemps31 = 0;
unsigned long marcaTemps3 = 0;
int estatPosta3=0;
//Distancia miDistancia3;

unsigned long marcaTemps41 = 0;
unsigned long marcaTemps4 = 0;
int estatPosta4=0;
//Distancia miDistancia4;

unsigned long marcaTemps51 = 0;
unsigned long marcaTemps5 = 0;
int estatPosta5=0;
//Distancia miDistancia5;

//Variable per veure els prints al monitor
boolean DEBUG = true;
boolean DEBUG2 = true;

//Variable per encendre la posta al comensar prova1 2 i 3
bool primeraPosta=true;

//Variable per prova5
bool entra=true; 

//Variable per funcio prova5a
bool entra1=false;

//Variable per prova5a
bool dos=true; 

//Variables per a proba5b
boolean primeraVegada=true;
int index1=0;

//Variable inici joc coordinació audio 
bool iniciJoc=true;

//VARIABLES i objectes de la xarxa i client Mosquitto
byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0x68};//mac del arduino
IPAddress ip(192, 168, 68, 22); //Ip fija del arduino
IPAddress server(192, 168, 68, 1); //Ip del server de mosquitto

EthernetClient ethClient; //Interfaz de red ethernet
PubSubClient client(ethClient); //cliente MQTT

const int P1greenLEDPin = 43;    // LED connected to digital pin 43 Posta 1
const int P1redLEDPin = 41;     // LED connected to digital pin 41 Posta 1
const int P1blueLEDPin = 39;    // LED connected to digital pin 39 Posta 1

const int P2greenLEDPin = 37;    // LED connected to digital pin 37 Posta 2
const int P2redLEDPin = 35;     // LED connected to digital pin 35 Posta 2
const int P2blueLEDPin = 33;    // LED connected to digital pin 33 Posta 2

const int P3greenLEDPin = 48;    // LED connected to digital pin 48 Posta 3
const int P3redLEDPin = 46;     // LED connected to digital pin 46 Posta 3
const int P3blueLEDPin = 44;    // LED connected to digital pin 44 Posta 3

const int P4greenLEDPin = 49;    // LED connected to digital pin 49 Posta 4
const int P4redLEDPin = 47;     // LED connected to digital pin 47 Posta 4
const int P4blueLEDPin = 45;    // LED connected to digital pin 45 Posta 4

const int P5greenLEDPin = 42;    // LED connected to digital pin 42 Posta 5
const int P5redLEDPin = 40;     // LED connected to digital pin 40 Posta 5
const int P5blueLEDPin = 38;    // LED connected to digital pin 38 Posta 5

//TODO Comprobar y probar
const int CAgreenLEDPin = 32; // LED conneted to digital pin 32 CASCO ALADO
const int CAredLEDPin = 34; //LED connected to digital pin 34 CASCO ALADO
const int CAblueLEDPin = 36; //LED connected to digital pin 36 CASCO ALADO

const int P1TriggerPin = 23; // Trigger pin 23 Posta 1
const int P1EchoPin =22;     // Echo pin 22 Posta 1

const int P2TriggerPin = 29;
const int P2EchoPin =28;

const int P3TriggerPin = 25;
const int P3EchoPin =24;  

const int P4TriggerPin = 31;
const int P4EchoPin =30;

const int P5TriggerPin = 27;
const int P5EchoPin = 26;

const int RELE_LLUM = A1;

NewPing sonar[SONAR_NUM] = { // Sensor object array.
  NewPing(P1TriggerPin, P1EchoPin, MAX_DISTANCE), 
  NewPing(P2TriggerPin, P2EchoPin, MAX_DISTANCE), 
  NewPing(P3TriggerPin, P3EchoPin, MAX_DISTANCE), 
  NewPing(P4TriggerPin, P4EchoPin, MAX_DISTANCE), 
  NewPing(P5TriggerPin, P5EchoPin, MAX_DISTANCE), 
};

void setup() {
  
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  if (DEBUG) {
    Serial.println(F("Inicia Arduino"));
  }

  //MQTT
  client.setServer(server, 1883);
  client.setCallback(callback);

  //Ethernet.begin(mac);   
  Ethernet.begin(mac,ip);
      
  delay(100); //Cambio de 1000 a 100
  if (DEBUG) {
    Serial.println(F("connectant..."));
    Serial.println(Ethernet.localIP());
    Serial.println(F("Definició dels PINS dels LEDS"));
  }
  
  // set the digital pins as outputs
  pinMode(P1greenLEDPin, OUTPUT);
  pinMode(P1redLEDPin, OUTPUT);
  pinMode(P1blueLEDPin, OUTPUT);

  pinMode(P2greenLEDPin, OUTPUT);
  pinMode(P2redLEDPin, OUTPUT);
  pinMode(P2blueLEDPin, OUTPUT);

  pinMode(P3greenLEDPin, OUTPUT);
  pinMode(P3redLEDPin, OUTPUT);
  pinMode(P3blueLEDPin, OUTPUT);

  pinMode(P4greenLEDPin, OUTPUT);
  pinMode(P4redLEDPin, OUTPUT);
  pinMode(P4blueLEDPin, OUTPUT);

  pinMode(P5greenLEDPin, OUTPUT);
  pinMode(P5redLEDPin, OUTPUT);
  pinMode(P5blueLEDPin, OUTPUT);
  //CASCO ALADO
  pinMode(CAgreenLEDPin, OUTPUT);
  pinMode(CAredLEDPin, OUTPUT);
  pinMode(CAblueLEDPin, OUTPUT);


  if (DEBUG) {   
    Serial.println(F("Definició dels PINS ECHO i TRIGGER dels sensors ultrasons"));
  }
  // Ponemos el pin Trig en modo salida y
  // el pin Echo en modo entrada
  pinMode(P1TriggerPin, OUTPUT);  
  pinMode(P1EchoPin, INPUT);

  pinMode(P2TriggerPin, OUTPUT);
  pinMode(P2EchoPin, INPUT);

  pinMode(P3TriggerPin, OUTPUT);
  pinMode(P3EchoPin, INPUT);

  pinMode(P4TriggerPin, OUTPUT);
  pinMode(P4EchoPin, INPUT);

  pinMode(P5TriggerPin, OUTPUT); 
  pinMode(P5EchoPin, INPUT);

  //Encendre i apagar llum
  pinMode(RELE_LLUM,OUTPUT);
  analogWrite(RELE_LLUM,LOW);

  //Establecemos la semilla en un pin analogico
  randomSeed(analogRead(A0));  
}

void enciendeCASCO(int redValue, int greenValue, int blueValue){
  apagaTODAS();
  enciendeSoloCASCO(redValue,greenValue,blueValue);
}

void enciendePOSTA1(int redValue, int greenValue, int blueValue){  
  apagaTODAS();
  enciendeSoloPOSTA1(redValue,greenValue,blueValue);
}

void enciendePOSTA2(int redValue, int greenValue, int blueValue){
  apagaTODAS();
  enciendeSoloPOSTA2(redValue,greenValue,blueValue);
}

void enciendePOSTA3(int redValue, int greenValue, int blueValue){  
  apagaTODAS();
  enciendeSoloPOSTA3(redValue,greenValue,blueValue);
}

void enciendePOSTA4(int redValue, int greenValue, int blueValue){  
  apagaTODAS();
  enciendeSoloPOSTA4(redValue,greenValue,blueValue);
}

void enciendePOSTA5(int redValue, int greenValue, int blueValue){
  apagaTODAS();
  enciendeSoloPOSTA5(redValue,greenValue,blueValue); 
}

void enciendeSoloCASCO(int redValue, int greenValue, int blueValue){
  analogWrite(CAredLEDPin, redValue);
  analogWrite(CAgreenLEDPin, greenValue);
  analogWrite(CAblueLEDPin, blueValue);
}

void enciendeSoloPOSTA1(int redValue, int greenValue, int blueValue){
  analogWrite(P1redLEDPin, redValue);
  analogWrite(P1greenLEDPin, greenValue);
  analogWrite(P1blueLEDPin, blueValue);
}

void enciendeSoloPOSTA2(int redValue, int greenValue, int blueValue){
  analogWrite(P2redLEDPin, redValue);
  analogWrite(P2greenLEDPin, greenValue);
  analogWrite(P2blueLEDPin, blueValue);
}

void enciendeSoloPOSTA3(int redValue, int greenValue, int blueValue){
  analogWrite(P3redLEDPin, redValue);
  analogWrite(P3greenLEDPin, greenValue);
  analogWrite(P3blueLEDPin, blueValue);
}

void enciendeSoloPOSTA4(int redValue, int greenValue, int blueValue){
  analogWrite(P4redLEDPin, redValue);
  analogWrite(P4greenLEDPin, greenValue);
  analogWrite(P4blueLEDPin, blueValue);
}

void enciendeSoloPOSTA5(int redValue, int greenValue, int blueValue){  
  analogWrite(P5redLEDPin, redValue);
  analogWrite(P5greenLEDPin, greenValue);
  analogWrite(P5blueLEDPin, blueValue);
}

void enciendeGENERAL(){
  digitalWrite(RELE_LLUM,HIGH); //ENCENDRE LLUM GENERAL
}

void apagaGENERAL(){
  digitalWrite(RELE_LLUM,LOW); //APAGA LLUM GENERAL
}

void enciendeTODALUZ(){ //ENCENDRE TOTES LES LLUMS INCLUIDA LA GENERAL

  int redValue, greenValue, blueValue=255;
  enciendeGENERAL(); //ENCENDRE LLUM GENERAL
  enciendeSoloCASCO( redValue,  greenValue,  blueValue);
  enciendeSoloPOSTA1( redValue,  greenValue,  blueValue);
  enciendeSoloPOSTA2( redValue,  greenValue,  blueValue);
  enciendeSoloPOSTA3( redValue,  greenValue,  blueValue);
  enciendeSoloPOSTA4( redValue,  greenValue,  blueValue);
  enciendeSoloPOSTA5( redValue,  greenValue,  blueValue);
}

void apagaTODALUZ(){ //APAGA TOTES LES LLUMS INCLUIDA LA GENERAL
  int redValue, greenValue, blueValue=0;
  apagaGENERAL(); //APAGA LLUM GENERAL
  enciendeSoloCASCO( redValue,  greenValue,  blueValue);
  enciendeSoloPOSTA1( redValue,  greenValue,  blueValue);
  enciendeSoloPOSTA2( redValue,  greenValue,  blueValue);
  enciendeSoloPOSTA3( redValue,  greenValue,  blueValue);
  enciendeSoloPOSTA4( redValue,  greenValue,  blueValue);
  enciendeSoloPOSTA5( redValue,  greenValue,  blueValue);
}

void enciendeTODAS(int redValue, int greenValue, int blueValue){
  enciendeSoloCASCO( redValue,  greenValue,  blueValue);
  enciendeSoloPOSTA1( redValue,  greenValue,  blueValue);
  enciendeSoloPOSTA2( redValue,  greenValue,  blueValue);
  enciendeSoloPOSTA3( redValue,  greenValue,  blueValue);
  enciendeSoloPOSTA4( redValue,  greenValue,  blueValue);
  enciendeSoloPOSTA5( redValue,  greenValue,  blueValue);
    
} 

void enciendeTODASPOSTAS(int redValue, int greenValue, int blueValue){
  enciendeSoloPOSTA1( redValue,  greenValue,  blueValue);
  enciendeSoloPOSTA2( redValue,  greenValue,  blueValue);
  enciendeSoloPOSTA3( redValue,  greenValue,  blueValue);
  enciendeSoloPOSTA4( redValue,  greenValue,  blueValue);
  enciendeSoloPOSTA5( redValue,  greenValue,  blueValue);
  
} 

void apagaTODASPOSTAS(){
  apagaPOSTA1();
  apagaPOSTA2();
  apagaPOSTA3();
  apagaPOSTA4();
  apagaPOSTA5();  
} 


//Funciones para apagar las postas
void apagaPosta(int posta){
   switch(posta){
    case 1:
      apagaPOSTA1();
      break;
    case 2:
      apagaPOSTA2();
      break;
    case 3:
      apagaPOSTA3();
      break;  
    case 4:
      apagaPOSTA4();
      break;
    case 5:
      apagaPOSTA5();
      break;
  }
}


void apagaCASCO(){
  enciendeCASCO(0,0,0);
}

void apagaPOSTA1(){
  enciendeSoloPOSTA1(0,0,0);
}
void apagaPOSTA2(){
  enciendeSoloPOSTA2(0,0,0);
}
void apagaPOSTA3(){
  enciendeSoloPOSTA3(0,0,0);
}
void apagaPOSTA4(){
  enciendeSoloPOSTA4(0,0,0);
}
void apagaPOSTA5(){
  enciendeSoloPOSTA5(0,0,0);
}

void apagaTODAS(){
  apagaGENERAL(); //APAGAR LLUM GENERAL
  enciendeTODAS(0,0,0);
}

void enciendeSoloPOSTA(int posta, int redValue, int greenValue, int blueValue) {
  switch(posta){
    case 1:
      enciendeSoloPOSTA1(redValue,greenValue,blueValue);
      break;
    case 2:
      enciendeSoloPOSTA2(redValue,greenValue,blueValue);
      break;
    case 3:
      enciendeSoloPOSTA3(redValue,greenValue,blueValue);
      break;  
    case 4:
      enciendeSoloPOSTA4(redValue,greenValue,blueValue);
      break;
    case 5:
      enciendeSoloPOSTA5(redValue,greenValue,blueValue);
      break;
  }
}

void enciendePOSTA(int posta, int redValue, int greenValue, int blueValue) {
  switch(posta){
    case 1:
      enciendePOSTA1(redValue,greenValue,blueValue);
      break;
    case 2:
      enciendePOSTA2(redValue,greenValue,blueValue);
      break;
    case 3:
      enciendePOSTA3(redValue,greenValue,blueValue);
      break;  
    case 4:
      enciendePOSTA4(redValue,greenValue,blueValue);
      break;
    case 5:
      enciendePOSTA5(redValue,greenValue,blueValue);
      break;
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  if (DEBUG) {
    Serial.print(F("Message arrived ["));
    Serial.print(topic);    
    Serial.print(F("] "));
    for (int i=0;i<length;i++) {
      Serial.print((char)payload[i]);
    }  
    Serial.println();
  }
  int res=strcmp(topic,"sala1/reinici");
  if (res ==0) {
    iniciJoc=true;
    estat=0;
    index=0;
    index1=0;
    apagaTODAS();
    inicialitzaEstats(); 
    inicialitzaArrayPostes();
    inicialitzaArrayPostesVar();
    inicialitzaEstatPosta();
    inicialitzaEstatPostaVar();          
  }
  res=strcmp(topic,"sala1/inici");
  if (res == 0) {    
    tiempoInicio=millis();    
  } 
  res=strcmp(topic,"sala1/posta1");
  if (res == 0) {    
    if (DEBUG) {
      Serial.print(F("ENTRA a posta1"));
    }
    transicio1();
  }
  res=strcmp(topic,"sala1/posta2");
  if (res == 0) {    
    if (DEBUG) {
      Serial.print(F("ENTRA a posta2"));
    }
    transicio2();
  }
  res=strcmp(topic,"sala1/posta3");
  if (res == 0) {    
    if (DEBUG) {
      Serial.print(F("ENTRA a posta3"));
    }
    transicio3();
  }
  res=strcmp(topic,"sala1/posta4");
  if (res == 0) {    
    if (DEBUG) {
      Serial.print(F("ENTRA a posta4"));
    }
    transicio4();
  }
  res=strcmp(topic,"sala1/posta5");
  if (res == 0) {    
    if (DEBUG) {
      Serial.print(F("ENTRA a posta5"));
    }
    transicio5();
  }
  res=strcmp(topic,"sala1/final");
  if (res == 0) {    
    if (DEBUG) {
      Serial.print(F("ENTRA a final"));
    }
     apagaTODAS();
     estat=7;
  }
  res=strcmp(topic,"sala1/encenLlum");
  if (res == 0) {    
    if (DEBUG) {
      Serial.print(F("ENTRA en encenLlum"));
    }
     enciendeGENERAL();
  }
   res=strcmp(topic,"sala1/apagaLlum");
  if (res == 0) {    
    if (DEBUG) {
      Serial.print(F("ENTRA en apagaLlum"));
    }
     apagaGENERAL();
  }
  res=strcmp(topic,"sala1/rfid");
  if (res == 0) {    
    if (DEBUG) {
      Serial.print(F("ENTRA en rfid"));
    }     
    apagaTODAS();
    estat=7;
  } 
  res=strcmp(topic,"sala1/portafinal");
  if (res == 0) {    
    if (DEBUG) {
      Serial.print(F("ENTRA en portafinal"));
    }
     estat=8;
  }
  //TODO IMPLEMENTAR
  res=strcmp(topic,"sala1/ultimoMinuto");
  if (res == 0) {    
    if (DEBUG) {
      Serial.print(F("ENTRA en ultimoMinuto"));
    }
     //estat=20;
     iniciUltimMinut=millis();
     ultimMinut=true;
  }
  
  res=strcmp(topic,"sala1/reset");
  int resu = strcmp(topic,"sala1/resetSalaFinal");  
  if (res == 0 || resu == 0) { //RESET PLACA
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
    if (client.connect("SalaFinal")) {
      if (DEBUG) {
        Serial.println(F("connectat MQTT"));      
      }
      client.subscribe("sala1/reinici");
      client.subscribe("sala1/inici");
      client.subscribe("sala1/posta1");
      client.subscribe("sala1/posta2");
      client.subscribe("sala1/posta3");
      client.subscribe("sala1/posta4");
      client.subscribe("sala1/posta5"); 
      client.subscribe("sala1/final");
      client.subscribe("sala1/encenLlum");
      client.subscribe("sala1/apagaLlum");
      client.subscribe("sala1/rfid");
      client.subscribe("sala1/portafinal");
      client.subscribe("sala1/reset"); 
      client.subscribe("sala1/resetSalaFinal");
      client.subscribe("sala1/ultimoMinuto");
           
    } else {
      if (DEBUG) {
        Serial.print(F("FALLA, rc="));
        Serial.print(client.state());
      }     
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }

}

void inicialitzaArrayPostes(){

  //Variable on guardem el numero aleatori
  long randomNumber;
  long anterior=-1;
  for (int i=0; i < NUMSIMON2; i++){
    //Genera un numero aleatori entre 1 y 5
    randomNumber = random(1,6);  
     if (DEBUG){
      Serial.print(F("Funcion inicializaArrayPostes randomNumber: "));
      Serial.println(randomNumber);
    }
    if (randomNumber == anterior) i--; //No poden haver dos postes seguides
    else {
      arrayPostes[i]=randomNumber;  
      anterior=randomNumber;
    }
  }  
}

// 2 posicions 5-45  45-95
void inicialitzaArrayPostesVar(){

  //Variable on guardem el numero aleatori
  int randomNumber;
  int anterior=-1;
  int distAnterior=-1;
  
  for (int i=0; i < NUMSIMON5; i++){
    //Genera un numero aleatori entre 1 y 5
    randomNumber = random(1,6);  
    if (DEBUG){
      Serial.print(F("Funcion inicializaArrayPostesVar randomNumber: "));
      Serial.println(randomNumber);
    }
    if (randomNumber == anterior) i--; //No poden haver dos postes seguides
    else {
      arrayPostesVar[i]=randomNumber;  
      anterior=randomNumber;
      //Genera random entre 0 y 1
      randomNumber = random(0,2);
      arrayRandomDist[i]=randomNumber;      
    }       
  }     
}

void inicialitzaEstatPosta(){
  for (int i=0; i< NUMSIMON2; i++){
    estatPosta[i]=0;
  }
}

void inicialitzaEstatPostaVar(){
   for (int i=0; i< 5; i++){
    estatPostaVar[i]=0;
  }
}

void inicialitzaEstats(){
  primeraPosta=true;

  primeraVegada=true;
  marcaTemps = 0;
  
  marcaTemps1 = 0;
  marcaTemps11 = 0;
  estatPosta1=0;
  //miDistancia1.inicializa();
  
  marcaTemps2 = 0;
  marcaTemps21 = 0;
  estatPosta2=0;
  //miDistancia2.inicializa();
  
  marcaTemps3 = 0;
  marcaTemps31 = 0;
  estatPosta3=0;
  //miDistancia3.inicializa();
  
  marcaTemps4 = 0;
  marcaTemps41 = 0;
  estatPosta4=0;
  //miDistancia4.inicializa();
  
  marcaTemps5 = 0;
  marcaTemps51 = 0;
  estatPosta5=0;
  //miDistancia5.inicializa();  

  index=0;
  index1=0;
  
  dos = true;
  entra = true;
  entra1 = false;

  tempsProva2=0;
  tempsProva3=0;
  tempsProva4=0;
  tempsProva5=0;
}


//Funció per realitzar la prova1 situar-se davant de cada posta 5 segs a una deistancia de entre 30 i 50 cms
void prova1() {
  if (DEBUG2) {
      Serial.println(F("Entra a la funció prova1"));
  }
  
  if (estatPosta1 == 0 && primeraPosta) {
    apagaCASCO();
    enciendeSoloPOSTA1(0,0,255); //BLAU
    apagaPOSTA2();
    enciendeSoloPOSTA3(0,0,255); //BLAU
    apagaPOSTA4();
    enciendeSoloPOSTA5(0,0,255); //BLAU
    primeraPosta=false;
  }
  //INICI SENSOR POSTA1
   if (estatPosta1 == 0 || estatPosta1 == 1 ){
    //long distancia1=0;    
    long distancia1 = sonar[0].convert_cm(sonar[0].ping_median(6));
    //miDistancia1.insertar(sonar[0].convert_cm(sonar[0].ping_median(4)));    
    //if (miDistancia1.valida()){
    //  distancia1 = miDistancia1.media();    
    //}  
    if (DEBUG) {
      Serial.print(F("SENSOR 1: "));
      Serial.print(distancia1);
      Serial.println(F(" cms"));
    }
    if (estatPosta1 == 0 && distancia1 >= 5 && distancia1 <= 45 && (millis() - marcaTemps11) > 600 ){    
       if (DEBUG) {
          Serial.println(F("SENSOR 1: Primera Part -- estatPosta1=0"));          
        }
              
      enciendeSoloPOSTA1(255,255,0); //GROC
      marcaTemps1 = millis();
      estatPosta1=1;      
      
    }else if (estatPosta1 == 1 && distancia1 >= 5 && distancia1 <= 45 ) {
      if (DEBUG) {
          Serial.println(F("SENSOR 1: Segona Part -- estatPosta1=1"));          
      }
     
      if ( (millis() - marcaTemps1) > tempsEspera1 ){
        if (DEBUG) {
          Serial.println(F("SENSOR 1: Tercera Part -- Pasen 2 segs -- estatPosta1=2"));          
        }
        client.publish("sala1/completado","on");
        enciendeSoloPOSTA1(0,255,0); //VERD
        estatPosta1=2;
      }
    }else if (estatPosta1 == 1 && (millis() - marcaTemps1) > 600){
      if (DEBUG) {
        Serial.println(F("SENSOR 1: Quarta Part -- torna estatPosta1=0"));          
      } 
      marcaTemps11=millis();
      enciendeSoloPOSTA1(0,0,255); //BLAU
      estatPosta1=0;
    }  
   }
   //CODI PER ENCENDRE I JUGAR AMB LES POSTES  1-3-5
    if (estatPosta1 == 2) {
      if (estatPosta3 == 0 || estatPosta3 == 1 ){
          long distancia3=sonar[2].convert_cm(sonar[2].ping_median(6));          
           if (DEBUG) {
              Serial.print(F("SENSOR 3: "));
              Serial.print(distancia3);
              Serial.println(F(" cms"));
            }
          if (estatPosta3 == 0 && distancia3 >= 5 && distancia3 <= 45 && (millis() - marcaTemps31) > 600){
            enciendeSoloPOSTA3(255,255,0); //GROC
            marcaTemps3 = millis();
            estatPosta3=1;
          }else if (estatPosta3 == 1 && distancia3 >= 5 && distancia3 <= 45 ) {
            if ( millis() - marcaTemps3 > tempsEspera1 ){
              client.publish("sala1/completado","on");
              enciendeSoloPOSTA3(0,255,0); //VERD
              estatPosta3=2;
            }
          }else if (estatPosta3 == 1 && (millis() - marcaTemps3) > 600){
            enciendeSoloPOSTA3(0,0,255);
            estatPosta3=0;
            marcaTemps31 = millis();
          }
        }
        if (estatPosta3 == 2) {
            long distancia5=sonar[4].convert_cm(sonar[4].ping_median(6));
             if (DEBUG) {
                Serial.print(F("SENSOR 5: "));
                Serial.print(distancia5);
                Serial.println(F(" cms"));
              }
            if (estatPosta5 == 0 && distancia5 >= 5 && distancia5 <= 45 && (millis() - marcaTemps51) > 600 ){
              enciendeSoloPOSTA5(255,255,0); //GROC
              marcaTemps5 = millis();
              estatPosta5=1;
            }else if (estatPosta5 == 1 && distancia5 >= 5 && distancia5 <= 45 ) {
              if ( millis() - marcaTemps5 > tempsEspera1 ){
                client.publish("sala1/completado","on");
                enciendeSoloPOSTA5(0,255,0); //VERD
                estatPosta5=2;
              }
            }else if (estatPosta5 == 1 && (millis() - marcaTemps5) > 600){
              enciendeSoloPOSTA5(0,0,255);
              estatPosta5=0;
              marcaTemps51 = millis();
            }          
        }//EstatPosta3 == 2            
    }//EstatPosta1 == 2
  
    if (estatPosta1 == 2 && estatPosta3 == 2 && estatPosta5 == 2) {
      client.publish("sala1/casco1","on"); //Envia el event per al sonido i per al pisto
      enciendeSoloCASCO(0,255,0);
      if (DEBUG) {
          Serial.println(F("provaSensorPostes FINAL: Tots els estatPosta = 2 "));          
        }
        for (int i=0; i < 3; i++) {
          enciendeTODASPOSTAS(0,0,0);
          delay(500);
          enciendeTODASPOSTAS(0,255,0);
          delay(500);
        }
        //delay(tempsEspera2);
            
      transicio1();
    }
  
}

//Funcio per a la transicio entre la primera prova i la segona
void transicio1(){
  if (DEBUG2) {
     Serial.println(F("Funcio transicio1"));          
  }
  primeraPosta=true;
  estat=2;
  index=0;
  index1=0;
  inicialitzaEstats();
  inicialitzaEstatPosta();
  inicialitzaEstatPostaVar();
  inicialitzaArrayPostes();  
  enciendeTODAS(255,255,255);
  delay(tempsEspera2);
}

//Funcio per a realitzar la prova2. SIMON de NUMSIMON2 postes a una distancia de entre 5 i 45 cms
void prova2(){
  if (DEBUG2) {
      Serial.println(F("Entra a la funció prova2"));
      Serial.print(F("index="));
      Serial.print(index);
      Serial.print(F(" estatPosta[index]="));
      Serial.print(estatPosta[index]);
  }

  if ( index < NUMSIMON2 ) {    
      if (entra) {
        marcaTemps=millis();
        entra=false;
      }
      enciendePOSTA(arrayPostes[index],0,0,255);      
      if ( (millis() - marcaTemps) > tempsEspera3) {
        index++;
        entra=true;
      }
  } else {    
    //INICI SENSORS
    if (primeraPosta) {
      tempsProva2=millis();
      apagaTODAS();
      primeraPosta=false;
    }
    if (estatPosta[index1] == 0 || estatPosta[index1] == 1 ){      
      long distancia = sonar[arrayPostes[index1]-1].convert_cm(sonar[arrayPostes[index1]-1].ping_median(6));      
      if (DEBUG) {
        Serial.print(F("SENSOR : "));
        Serial.print(index1);
        Serial.print(F("ARRAYPOSTES[index1]"));
        Serial.print(arrayPostes[index1]);
        Serial.print(F("DISTANCIA :"));
        Serial.print(distancia);
        Serial.println(F(" cms"));
      } 
      if (estatPosta[index1] == 0 && distancia >= 5 && distancia <= 45 && (millis() - marcaTemps11) > 600){    
        if (DEBUG) {
            Serial.println(F("SENSOR : Primera Part -- estatPosta1=0"));          
        }
        enciendePOSTA(arrayPostes[index1],255,255,0); //GROC  
        marcaTemps = millis();
        estatPosta[index1]=1;      
        
      }else if (estatPosta[index1] == 1 && distancia >= 5 && distancia <= 45 ) {
         if (DEBUG) {
            Serial.println(F("SENSOR : Segona Part -- estatPosta1=1"));          
         }
       
         if ( (millis() - marcaTemps) > tempsEspera1 ){
          if (DEBUG) {
            Serial.println(F("SENSOR : Tercera Part -- Pasen 2 segs -- estatPosta1=2"));          
          }
          client.publish("sala1/completado","on");
          enciendePOSTA(arrayPostes[index1],0,255,0); //VERD
          delay(tempsEspera3);
          estatPosta[index1]=2;
          index1++;        
          marcaTemps=0;
          marcaTemps11=0;
          primeraPosta=true;          
        }
      }else if (estatPosta[index1] == 1 && (millis() - marcaTemps) > 600){
        if (DEBUG) {
          Serial.println(F("SENSOR : Quarta Part -- torna estatPosta1=0"));          
        } 
        marcaTemps11=millis();
        apagaPosta(arrayPostes[index1]);
        estatPosta[index1]=0;
      }  
     }  
  
    boolean todas=true; //Variable per a comprovar si s'ha superar la prova2
    for (int i=0; i < NUMSIMON2 && todas; i++){
      if ( estatPosta[i] != 2 ){
          todas=false; 
      }
    }     
    if (todas) {
      client.publish("sala1/casco2","on");//Envia el event per a la veu de OCCA
      enciendeSoloCASCO(0,255,0);
        if (DEBUG) {
            Serial.println(F("provaSensorPostes FINAL: Tots els estatPosta = 2 "));          
          } 
          
          for (int i=0; i < 3; i++) {
            enciendeTODASPOSTAS(0,0,0);
            delay(500);
            enciendeTODASPOSTAS(0,255,0);
            delay(500);
          }
        
        transicio2();  
    }
  }
}

//Funcio per a la transicio entre la segona prova i la tercera
void transicio2(){
  if (DEBUG2) {
     Serial.println(F("Funcio transicio2"));          
  }
  estat=3;    
  inicialitzaEstats();  
  enciendeTODAS(255,255,255);
  delay(tempsEspera2);
}


//Funció per realitzar la prova3, que cap posta detecte 
void prova3() {
  long distancia1;
  long distancia3;
  long distancia5;
  if (DEBUG2) {
      Serial.println(F("Entra a la funció prova3"));
  }
  
  if (estatPosta1 == 0 && primeraPosta) {
    primeraPosta=false;
    apagaTODAS();
    tempsProva3=millis();
  }

  distancia1=sonar[0].convert_cm(sonar[0].ping_median(4));
  delay(80);
  distancia3=sonar[2].convert_cm(sonar[2].ping_median(4));
  delay(80);
  distancia5=sonar[4].convert_cm(sonar[4].ping_median(4));

  if (distancia1 > 0 && distancia1 < 100 || distancia3 > 0 && distancia3 < 100 || distancia5 > 0 && distancia5 < 100){
    if (DEBUG) {
      Serial.println(F("Entra en detectado por algún sensor PROVA3"));
    }
    client.publish("sala1/error","on");  //si volem que sone alarma de error
    if (distancia1 > 0 && distancia1 < 100) enciendeSoloPOSTA1(255,0,0);
    else if (distancia3 > 0 && distancia3 < 100) enciendeSoloPOSTA3(255,0,0);
    else if (distancia5 >0 && distancia5 < 100) enciendeSoloPOSTA5(255,0,0);
    delay(1000);
    errorReinici();
    primeraPosta=true;
    client.publish("sala1/casco25","on");
    tempsProva3=millis();
  }else {

    if (millis() - tempsp3 >= 999) {
      if (DEBUG) {
        Serial.println(F("Entra en envia evento completado"));
      }
      client.publish("sala1/completado","on"); 
      tempsp3=millis();
    }    
  }
  
}

//Funcio per a la transicio entre la segona prova i la tercera
void transicio3(){
  if (DEBUG) {
          Serial.println(F("Funcio transicio3"));          
  }
  estat=4;
  index=0;
  inicialitzaEstats();
  inicialitzaArrayPostesVar();
  inicialitzaEstatPostaVar();

  enciendeTODAS(255,255,255);    
  delay(tempsEspera2);
}

void prova4() {
  if (DEBUG2) {
      Serial.println(F("Entra a la funció prova4"));
  }

  if (index == 5 && entra) {
    
    prova4b();
    
  }else {

    if (entra) {
        marcaTemps=millis();
        entra=false;
    }
  
    enciendeTODASPOSTAS(255,0,0);
    enciendeSoloPOSTA(arrayPrueba4[index]+1,0,0,255);        
    
    if ( (millis() - marcaTemps) > tempsEspera3) {
        index++;
        entra=true;
    }
  }
  
}

void prova4b(){
  if (DEBUG2) { 
     Serial.println(F("Funcio prova4b"));          
  } 

  if (primeraVegada) {
    tempsProva4=millis();
    apagaTODAS();
    primeraVegada=false;    
    index1=0;   
  }

  //INICI SENSORS
   if (estatPostaVar[index1] == 0 || estatPostaVar[index1] == 1 ){    

  if (DEBUG) {
    Serial.println(F("Entra mesura distancies prova4"));
  }
    
    long distancia=sonar[arrayPrueba4[index1]].convert_cm(sonar[arrayPrueba4[index1]].ping_median(4)); //Distancia posta bona
    delay(80);
    //Distancies sensors per para la prova (error)
    long distOp1=sonar[arrayOpuestoPrueba4[index1]].convert_cm(sonar[arrayOpuestoPrueba4[index1]].ping_median(4)); 
    long distOp2=0;
    delay(80);
    if (index1 == 0 || index1 == 1) distOp2=sonar[2].convert_cm(sonar[2].ping_median(4));
    else if (index1 == 4) distOp2=sonar[arrayOpuestoPrueba4[index1+1]].convert_cm(sonar[arrayOpuestoPrueba4[index1+1]].ping_median(4));
    
    if (DEBUG) {
      Serial.print(F("SENSOR:"));
      Serial.print(index1);
      Serial.print(F("  arrayPrueba4[index1] "));
      Serial.print(arrayPrueba4[index1]);
      Serial.print(F(" DISTANCIA:"));
      Serial.print(distancia);
      Serial.println(F(" cms"));
      Serial.print(F(" DISTOp1:"));
      Serial.print(distOp1);
      Serial.println(F(" cms"));
      Serial.print(F(" DISTOp2:"));
      Serial.print(distOp2);
      Serial.println(F(" cms"));
      Serial.println("PRIMERA VEGADA");
      Serial.println(primeraVegada);
      Serial.print(F("estatPostaVar[index1]" ));
      Serial.println(estatPostaVar[index1]);
    }

    if (distOp1 > 0 && distOp1 < 100 || distOp2 > 0 && distOp2 < 100) {
      if (DEBUG) {
        Serial.println(F("Entra en detectado por algún sensor PROVA4"));
      }
      client.publish("sala1/error","on");  //si volem que sone alarma de error
      if (distOp1 > 0 && distOp1 < 100) enciendeSoloPOSTA(arrayOpuestoPrueba4[index1]+1,255,0,0);
      else if (distOp2 > 0 && distOp2 < 100) {
         if (index1 == 0 || index1 == 1) enciendeSoloPOSTA3(255,0,0);
         else if (index1 == 4) enciendeSoloPOSTA1(255,0,0);
      }
      delay(1000);
      errorReinici();
      tempsProva4=millis();
      primeraPosta=true;
      client.publish("sala1/casco35","on");  
    }
    
    if (estatPostaVar[index1] == 0 && distancia >= 5 && distancia <= 45){    
      if (DEBUG) {
         Serial.println(F("SENSOR : Primera Part -- estatPostaVar=0"));
      }
      enciendePOSTA(arrayPrueba4[index1]+1,255,255,0);    //GROC  
      marcaTemps = millis();
      estatPostaVar[index1]=1;      
      
    }else if (estatPostaVar[index1] == 1 && distancia >= 5 && distancia <= 45 ) {
      if (DEBUG) {
          Serial.println(F("SENSOR : Segona Part -- estatPostaVar=1"));          
      }
     
      //if ( (millis() - marcaTemps) > tempsEspera1 ){
      if ( (millis() - marcaTemps) > 100 ){
        if (DEBUG) {
          Serial.println(F("SENSOR : Tercera Part -- Pasen 2 segs -- estatPostaVar=2"));          
        }
        client.publish("sala1/completado","on");
        enciendePOSTA(arrayPrueba4[index1]+1,0,255,0); //VERD
        delay(tempsEspera3);
        estatPostaVar[index1]=2;
        apagaPosta(arrayPrueba4[index1]+1);
        index1++;        
        marcaTemps=0;
        marcaTemps11=0;
        entra=true;          
        
      }
    }else if (estatPostaVar[index1] == 1 && (millis() - marcaTemps) > 600){
      if (DEBUG) {
        Serial.println(F("SENSOR : Quarta Part -- torna estatPostaVar=0"));          
      }
      apagaPosta(arrayPrueba4[index1]+1);
      estatPostaVar[index1]=0;
      marcaTemps11=millis();
    }      
      
   }  
       
  
    boolean todas=true; //Variable per a comprovar si s'ha superar la prova4
    for (int i=0; i < 5 && todas; i++){
      if ( estatPostaVar[i] != 2 ){
          todas=false; 
      }
    }    
    if (todas) {
        tempsProva4=millis();
        client.publish("sala1/casco4","on"); //Envia el event per a OCCA
        enciendeSoloCASCO(0,255,0);
        if (DEBUG) {
            Serial.println(F("provaSensorPostes FINAL: Tots els estatPostaVar = 2 "));          
          }
          for (int i=0; i < 3; i++) {
            enciendeTODASPOSTAS(0,0,0);
            delay(500);
            enciendeTODASPOSTAS(0,255,0);
            delay(500);
          }  
       
        transicio4();  
    }
  
}

void transicio4(){
  if (DEBUG2) {
      Serial.println(F("Funcio transicio4"));          
  }  
  estat=5;
  index=0;
  inicialitzaEstats();
  inicialitzaArrayPostesVar(); 
  inicialitzaEstatPostaVar();
  enciendeTODAS(255,255,255);
  delay(tempsEspera2);
}

//Funcio per a realitzar la prova5, l'última. SIMON de NUMSIMON5 postes a una distancia variable 2 posicions //5-45  55-95
void prova5(){
  if (DEBUG2) {
     Serial.println(F("Funcio prova5"));         
     Serial.print("dos y entra: ");
     Serial.print(dos);
     Serial.print(" ");
     Serial.println(entra); 
  }
  if (dos){
    if (entra) {
        apagaTODAS();
        marcaTemps=millis();
        entra=false;
    }
    enciendeSoloPOSTA1(255,0,255); //MORAT
    enciendeSoloPOSTA5(255,0,255);       
    if ( (millis() - marcaTemps) > tempsEspera3) {
        dos=false;
        entra=true;
    }
      
  }else if ( index < NUMSIMON5 ) {    
      if (entra) {
        marcaTemps=millis();
        entra=false;
      }
      switch(arrayRandomDist[index]){
          case 0: //45-95
            enciendePOSTA(arrayPostesVar[index],255,0,255); //MORAT
            break;
          case 1: //5-45
            enciendePOSTA(arrayPostesVar[index],0,0,255); //BLAU          
            break;    
        }
    
      if ( (millis() - marcaTemps) > tempsEspera3) {
        index++;
        entra=true;
      }
  }else {
    if (entra){
      apagaTODAS();
      entra=false;
      primeraVegada=true;
    }
    prova5a();
  } 
  
}

//Detecta la posta 1 y la posta 5 al mismo tiempo a una distancia de entre 5 y 45 cms
void prova5a(){
   long distancia1;
   long distancia5;
   if (DEBUG2) {
     Serial.println(F("Funcio prova5a"));          
   } 
   if (estatPosta1 != 2 && estatPosta5 != 2){
     if (primeraVegada) {
       estatPosta1=0;
       estatPosta5=0;
       apagaTODAS();
       tempsProva5=millis();
       primeraVegada=false;
     }
  
     distancia1=sonar[0].convert_cm(sonar[0].ping_median(6));
     delay(5);
     distancia5=sonar[4].convert_cm(sonar[4].ping_median(6));
   
   if (DEBUG2) {
     Serial.println(F("Despues de lectura sensores"));          
     Serial.println("Distancia 1  Distancia5 ");
     Serial.print(distancia1);
     Serial.print(" ");
     Serial.println(distancia5);
   } 
   if (estatPosta1 == 0 && distancia1 >= 45 && distancia1 <= 95 && (millis() - marcaTemps11) > 600 && estatPosta5 == 0 && distancia5 >= 45 && distancia5 <= 95 && (millis() - marcaTemps51) > 600 ){    
       if (DEBUG) {
          Serial.println(F("SENSOR 1: Primera Part -- estatPosta1=0 estatPosta5=0"));          
        }
              
      enciendeSoloPOSTA1(255,255,0); //GROC
      enciendeSoloPOSTA5(255,255,0);
      marcaTemps1 = millis();
      marcaTemps5 = millis();
      estatPosta1=1;    
      estatPosta5=1;
      
    }else if (estatPosta1 == 1 && distancia1 >= 45 && distancia1 <= 95 && estatPosta5 == 1 && distancia5 >= 45 && distancia5 <= 95 ) {
      if (DEBUG) {
          Serial.println(F("SENSOR 1: Segona Part -- estatPosta1=1 estatPOSTA5=1"));          
      }
     
      //if ( (millis() - marcaTemps1) > tempsEspera1 && (millis() - marcaTemps5) > tempsEspera1){
      if ( (millis() - marcaTemps1) > 100 && (millis() - marcaTemps5) > 100){ //Modificacion tiempo espera delante de postes
        if (DEBUG) {
          Serial.println(F("SENSOR 1: Tercera Part -- Pasen 2 segs -- estatPosta1=2 estatPosta5=2"));          
        }
        client.publish("sala1/completado","on");      
        enciendeSoloPOSTA1(0,255,0); //VERD
        enciendeSoloPOSTA5(0,255,0);
        delay(tempsEspera3);
        estatPosta1=2;
        estatPosta5=2;
        entra1=true;
        
      }
    }else if (estatPosta1 == 1 && (millis() - marcaTemps1) > 600 || estatPosta5 == 1 && (millis() - marcaTemps5) > 600){
      if (DEBUG) {
        Serial.println(F("SENSOR 1: Quarta Part -- torna estatPosta1=0"));          
      } 
      marcaTemps11=millis();
      marcaTemps51=millis();
      apagaTODAS();
      estatPosta1=0;
      estatPosta5=0;
    }  
              
   }else {
      if (entra1) {
        primeraVegada=true;
        entra1=false;
      }
      prova5b();      
   }
   
}

void prova5b(){
  if (DEBUG2) {
     Serial.println(F("Funcio prova5b"));          
  } 

  if (primeraVegada) {
    apagaTODAS();
    primeraVegada=false;    
    index1=0;
  }

  if (estatPostaVar[index1] == 0 && primeraPosta )  {
     switch(arrayRandomDist[index1]){
      case 0: //45-95
        distMin=45;
        distMax=95;       
        break;
      case 1: //5-45
        distMin=5;
        distMax=45;        
        break;     
    }
    primeraPosta=false;
  }
  //INICI SENSORS
   if (estatPostaVar[index1] == 0 || estatPostaVar[index1] == 1 ){    
    long distancia=sonar[arrayPostesVar[index1]-1].convert_cm(sonar[arrayPostesVar[index1]-1].ping_median(6));
    if (DEBUG) {
      Serial.print(F("SENSOR:"));
      Serial.print(index1);
      Serial.print(F(" ARRAYPOSTESVAR[index1]:"));
      Serial.print(arrayPostesVar[index1]);
      Serial.print(F(" ARRAYRANDOMDIST[index1]:"));
      Serial.print(arrayRandomDist[index1]);
      Serial.print(F(" DISTANCIA:"));
      Serial.print(distancia);
      Serial.println(F(" cms"));
      Serial.print(F("DISTMIN-DISTMAX:"));
      Serial.print(distMin);
      Serial.print(F("-"));
      Serial.println(distMax);
    }
    
    if (estatPostaVar[index1] == 0 && distancia >= distMin && distancia <= distMax ){    
      if (DEBUG) {
         Serial.println(F("SENSOR : Primera Part -- estatPostaVar=0"));          
      }
      enciendePOSTA(arrayPostesVar[index1],255,255,0);    //GROC  
      marcaTemps = millis();
      estatPostaVar[index1]=1;      
      
    }else if (estatPostaVar[index1] == 1 && distancia >= distMin && distancia <= distMax ) {
      if (DEBUG) {
          Serial.println(F("SENSOR : Segona Part -- estatPostaVar=1"));          
      }
     
      //if ( (millis() - marcaTemps) > tempsEspera1 ){
      if ( (millis() - marcaTemps) > 100 ){ //Modificacion tiempo espera delante de sensores
        if (DEBUG) {
          Serial.println(F("SENSOR : Tercera Part -- Pasen 2 segs -- estatPostaVar=2"));          
        }
        client.publish("sala1/completado","on");
        enciendePOSTA(arrayPostesVar[index1],0,255,0); //VERD
        primeraPosta=true;
        delay(tempsEspera3);
        apagaTODAS();
        estatPostaVar[index1]=2;
        index1++;        
        marcaTemps=0;        
      }
    }else if (estatPostaVar[index1] == 1 && (millis() - marcaTemps) > 600){
      if (DEBUG) {
        Serial.println(F("SENSOR : Quarta Part -- torna estatPostaVar=0"));          
      }      
      apagaPosta(arrayPostesVar[index1]);
      estatPostaVar[index1]=0;
      marcaTemps11=millis();
    }  
   }  
 
  boolean todas=true; //Variable per a comprovar si s'ha superar la prova4
  for (int i=0; i < NUMSIMON5 && todas; i++){
    if ( estatPostaVar[i] != 2 ){
        todas=false; 
    }
  }    
  if (todas) {
      tempsProva5=millis();
      client.publish("sala1/casco5","on");//Envia el event per a OCCA 
      enciendeSoloCASCO(0,255,0);
      if (DEBUG) {
          Serial.println(F("provaSensorPostes FINAL: Tots els estatPostaVar = 2 "));          
        }
       for (int i=0; i < 3; i++) {
          enciendeTODASPOSTAS(0,0,0);
          delay(500);
          enciendeTODASPOSTAS(0,255,0);
          delay(500);
        }     
      
      transicio5();  
  }    
}

void transicio5(){
  if (DEBUG2) {
      Serial.println(F("Funcio transicio5"));          
  }  
  estat=6;      
  apagaTODASPOSTAS();
  enciendeSoloCASCO(0,255,0);
  //delay(tempsEspera2);
  delay(16000); //Temps espera mentre Baixa Pisto i agafen CASCO ALADO
  client.publish("sala1/resetPortaFinal","on");
}

bool pasa=false;
void estat6(){ //Despres de superar les 5 proves
  if (DEBUG2) {
      Serial.println(F("Funcio estat6"));          
  }  
  if (!pasa){
    enciendeCASCO(255,0,0);
    delay(delayLeds);  
    enciendeSoloPOSTA5(255,0,0);
    delay(delayLeds);   
    enciendeSoloPOSTA4(255,0,0);
    delay(delayLeds);
    enciendeSoloPOSTA3(255,0,0);
    delay(delayLeds);
    enciendeSoloPOSTA2(255,0,0);
    delay(delayLeds);  
    enciendeSoloPOSTA1(255,0,0);
    delay(delayLeds);
    for (int i=0;  i < 3; i++){    
      apagaTODAS();
      delay(delayLeds);
      enciendeTODAS(255,0,0);
      delay(delayLeds);
    }  
    //enciendeGENERAL();
    pasa=true;
  }
}

void estatFinal(){
    //apagaTODAS();
    for (int i=0;  i < 3; i++){    
      apagaTODAS();
      delay(delayLeds);
      enciendeTODAS(255,0,0);
      delay(delayLeds);
    }  
    
    //delay(7000); //Temps espera DECODIFICANDO
    delay(1000); //Temps de 6 segs + 1 espera DECODIFICANDO
    apagaTODAS();
    estat=8;
}

bool pasa2=false; //Variable para que entre una única vez en encendido general
void fin(){
  if (!pasa2) {
    enciendeTODAS(255,255,255);
    enciendeGENERAL();
    pasa2=true;
  }
}

void errorReinici(){
  //client.publish("sala1/error","on");  //si volem que sone alarma de error
  enciendeTODAS(255,0,0); 
  delay(tempsEspera2);
  delay(tempsEspera2);
  index=0;
  index1=0;
  primeraVegada=true;
  apagaTODAS();
  inicialitzaArrayPostes();
  inicialitzaArrayPostesVar();
  inicialitzaEstatPosta();
  inicialitzaEstatPostaVar();
  inicialitzaEstats();   
  delay(tempsEspera1);
  //estat=5;  
}



bool pasa3 = false; //Variable para que espere 44 segs en encendre llums general (cridar funcio fin())
unsigned long tempsFin=0;
void loop() {


if (ultimMinut && ( millis() - iniciUltimMinut > 610000 )) {
  estat=8;
}

 switch(estat){    

    case 0:

     /* if (DEBUG2) {
        Serial.println(F("PASA: Estat = 0"));        
      }*/

      if (tiempoInicio > 0 && millis() - tiempoInicio > 35000) {
        //Serial.println("ENTRA PER AKI");
        //Serial.println(tiempoInicio);
        delay(200); //AJustar en el servidor de audio
        enciendePOSTA1(255,255,255);
        delay(delayLeds);
        enciendeSoloPOSTA2(255,255,255);
        delay(delayLeds); 
        enciendeSoloPOSTA3(255,255,255);
        delay(delayLeds);
        enciendeSoloPOSTA4(255,255,255);
        delay(delayLeds);
        enciendeSoloPOSTA5(255,255,255);
        delay(delayLeds);
         enciendeSoloCASCO(255,255,255);    
        delay(5000);
        estat=1;
      }
    break;
    case 1:
      if (DEBUG2) {
         Serial.println(F("PASA: Estat = 1"));          
      }
      //Temps espera per a coordinar la llum i el sonido
      if (iniciJoc){
         delay(5000);
         iniciJoc=false;
      }
      //PROVA per a permaneixer 1,2 segs davant de cada posta entre 5 i 45 cms 
      prova1();      
      break;
    case 2:
      if (DEBUG2) {
         Serial.println(F("PASA: Estat = 2"));          
      }
      //PROVA SIMON pas a pas, permaneixer 1 seg davant de cada posta entre 5 i 45 cms
      prova2();
      if ( estat == 2 && tempsProva2 > 0 && (millis() - tempsProva2) > 18000) {
        client.publish("sala1/error","on");  //si volem que sone alarma de error
        errorReinici();
        tempsProva2=0;
        client.publish("sala1/casco15","on");
      }
      break;
    case 3:
      if (DEBUG2) {
         Serial.println(F("PASA: Estat = 3"));          
      }      
      prova3();
      if ( estat == 3 && tempsProva3 > 0 && (millis() - tempsProva3) > 6500) {
        client.publish("sala1/casco3","on");
        tempsProva3=0;
        enciendeSoloCASCO(0,255,0);     
        for (int i=0; i < 3; i++) {
           enciendeTODASPOSTAS(0,0,0);
           delay(500);
           enciendeTODASPOSTAS(0,255,0);
           delay(500);
        }        
        transicio3();
      }
      break;
    case 4:    
      if (DEBUG2) {
         Serial.println(F("PASA: Estat = 4"));          
      }      
      prova4(); //PROVA SIMON pas a pas amb DISTANCIA variable
      if (estat == 4 && tempsProva4 > 0 && (millis() - tempsProva4) > 28000) {
        client.publish("sala1/error","on");  //si volem que sone alarma de error
        errorReinici();
        tempsProva4=0;
        client.publish("sala1/casco35","on");
      }
      break;
    case 5:
      if (DEBUG2) {
         Serial.println(F("PASA: Estat = 5"));          
      }            
      prova5();//PROVA SIMON amb DISTANCIA variable
      if (tempsProva5 > 0 && !primeraVegada && (millis() - tempsProva5) > 45000) {
        client.publish("sala1/error","on");  //si volem que sone alarma de error
        errorReinici();
        tempsProva5=0;
        client.publish("sala1/casco45","on");
      }
      break;
    case 6:
      if (DEBUG2) {
        Serial.println(F("PASA: Estat = 6"));
      }      
      estat6(); //ESTAT DE TRANSICIO per encendre llums i poc més (standby)
      break;
    case 7:
      if (DEBUG2) {
        Serial.println(F("PASA: Estat = 7"));        
      }
      estatFinal();
      break;
    case 8:
      if (DEBUG2) {
        Serial.println(F("PASA: Estat = 8"));        
      }
      if (!pasa3) {
        tempsFin=millis();
        pasa3=true;
      }
      if ((millis() - tempsFin) > 44000 ) fin(); //Espera 44 segs per encendre la llum i obrir la porta, tot a la vegada
      break;
    case 20: //sala1/ultimoMinuto
      if (DEBUG2) {
        Serial.println(F("PASA: Estat = 20  ulitmoMinuto"));        
      }
      break;
 }

 if (!client.connected()) {
      reconnect();
 }
 client.loop();
  
} 
