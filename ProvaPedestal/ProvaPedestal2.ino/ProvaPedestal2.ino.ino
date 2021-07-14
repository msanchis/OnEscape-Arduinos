/*TODO Revisar voltatge i resistencies interruptors pull-up per a passar a pull-down
 * 
 * Arduino MEGA (interior Pedestal <al mig de la sala2>)
 * 
 * 6 electroimans 20 kgs
 * 1 electroiman 60 kgs
 * 7 llums/leds 12V
 * 1 final de carrera/pulsador
 * 
 **********************
 * 
 * Per reseteja el dispositiu
 *  sala2/reset
 *  sala2/resetPedestal
 *  
 *  Per reiniciar estat pedestal
 *  sala2/reiniciPedestal
 * 
 * Per establir dificultat de la sala (futur)
 *  sala2/dificultat
 * 
 * Per controlar els 7 electroimans i llums del pedestal
 *  sala2/estat0Pedestal
 *  sala2/estat1Pedestal
 *  sala2/reliquia1Pedestal
 *  sala2/reliquia2Pedestal
 *  sala2/reliquia3Pedestal    
 *  sala2/reliquia4Pedestal
 *  sala2/reliquia5Pedestal
 *  sala2/reliquia6Pedestal
 *  sala2/llumFinalPedestal
 * 
 * Per controlar la prova de la pared (sensorPuzzle) al pin A0
 * 
 *  comensa en estat=0 i pasa a estat=1
 * 
 * **********************
 * 
 * Events que envia
 * 
 * Per activar efecte so despres de puzzle
 *   sala2/activaPuzzle1
 * 
 * Per activar efecte so despres de reliquies
 *   sala2/reliquia1
 *   sala2/reliquia2
 *   ...
 *   sala2/reliquia6
 * 
 * Per activar la bascula despres de reliquia1
 *   sala2/activaBascula
 * 
 * Per desactivar bascula despres de reliquia2
 *   sala2/desactivaBascula
 * 
 * Per activar el organ despres de reliquia5
 *   sala2/activaOrgan
 *   
 * Per a desactivar organ i para baixada punxos/llum ultravioleta punxos
 *   sala2/acordCorrecte
 *  
 * Per a finalitzar el pedestal
 *   sala2/finalPedestal
 */


#include <SPI.h>
#include <Ethernet.h> //Conexion Ethernet con carcasa W5100
#include <PubSubClient.h> //Conexion MQTT
//#include <avr/wdt.h> // Incluir la librería de ATmel


//VARIABLES i objectes de la xarxa i client Mosquitto
byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xF0, 0xFE};//mac del arduino
IPAddress ip(192, 168, 68, 202); //Ip fija del arduino
IPAddress server(192, 168, 68, 55); //Ip del server de mosquitto

EthernetClient ethClient; //Interfaz de red ethernet
PubSubClient client(ethClient); //cliente MQTT

const boolean DEBUG = true;
//const boolean DEBUG = false;

unsigned long debugTemps=0;
unsigned long tempsIman1=0;
//Variable IMPORTANT que permet la màquina d'estats del pedestal
int estat=0; 

//Variables per a controlar el temps de cada activació de sensor
boolean entra=false;
boolean entra1=false;
boolean entra2=false;
boolean entra3=false;
boolean entra4=false;
boolean entra5=false;
boolean entra6=false;

boolean final=false;

const int SensorPuzzle = 49; //Sensor puzzle paret
unsigned long marcaTemps0 = 0; //Marca de temps per al puzzle paret

const int Sensor1 = 23;    // Sensor1 presencia iman1
const int Sensor2 = 25;     // Sensor1 presencia iman2

unsigned long marcaTemps1 = 0; //Marca de temps per als sensors 1 i 2
boolean condicio10=false;
boolean condicio11=false;

const int Sensor3 = 27;    // Sensor2 presencia iman1
const int Sensor4 = 29;    // Sensor2 presencia iman2
unsigned long marcaTemps2 = 0; //Marca de temps per als sensors 3 i 4
boolean condicio20=false;
boolean condicio21=false;

const int Sensor5 = 31;     // Sensor3 presencia iman1
const int Sensor6 = 33;    // Sensor3 presencia iman2
unsigned long marcaTemps3 = 0; //Marca de temps per als sensors 5 i 6
boolean condicio30=false;
boolean condicio31=false;

const int Sensor7 = 35;    // Sensor4 presencia iman1
const int Sensor8 = 37;     // Sensor4 presencia iman2
unsigned long marcaTemps4 = 0; //Marca de temps per als sensors 7 i 8
boolean condicio40=false;
boolean condicio41=false;

const int Sensor9 = 39;    // Sensor5 presencia iman1
const int Sensor10 = 41;    // Sensor5 presencia iman2
unsigned long marcaTemps5 = 0; //Marca de temps per als sensors 9 i 10
boolean condicio50=false;
boolean condicio51=false;

const int Sensor11 = 43;     // Sensor6 presencia iman1
const int Sensor12 = 45;    // Sensor6 presencia iman2
unsigned long marcaTemps6 = 0; //Marca de temps per als sensors 11 i 12
boolean condicio60=false;
boolean condicio61=false;

const int RelElectro1 = 9; // Pin para cortar corriente Electroiman1 
const int RelElectro2 = 8; // Pin para cortar corriente Electroiman2 y encender luz4
const int RelElectro3 = 7; // Pin para cortar corriente Electroiman3 y encender luz5
const int RelElectro4 = 6; // Pin para cortar corriente Electroiman4 y encender luz1
const int RelElectro5 = 5; // Pin para cortar corriente Electroiman5 y encender luz2
const int RelElectro6 = 4; // Pin para cortar corriente Electroiman6 y encender luz3
const int RelElectro7 = 3; // Pin para cortar corriente Electroiman7 y encender luz7
const int RelElectro8 = 2; // Pin encender luz6

boolean pasaEstat=true; //Variable per comprovar que totes les reliquies estan al seu lloc per poder avançar


boolean entraFinal=false; //Comprova que ha realitzat la sequencia final

void(* resetFunc) (void) = 0; //declare reset function @ address 0

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
      
  delay(1000); //Cambio de 1000 a 100
  if (DEBUG) {
    Serial.println(F("connectant..."));
    Serial.println(Ethernet.localIP());
    Serial.println(F("Definició dels PINS dels LEDS"));
  }

  pinMode(SensorPuzzle, INPUT_PULLUP);
  
  pinMode(Sensor1, INPUT_PULLUP);
  pinMode(Sensor2, INPUT_PULLUP);
  pinMode(Sensor3, INPUT_PULLUP);
  pinMode(Sensor4, INPUT_PULLUP);
  pinMode(Sensor5, INPUT_PULLUP);
  pinMode(Sensor6, INPUT_PULLUP);
  pinMode(Sensor7, INPUT_PULLUP);
  pinMode(Sensor8, INPUT_PULLUP);
  pinMode(Sensor9, INPUT_PULLUP);
  pinMode(Sensor10, INPUT_PULLUP);
  pinMode(Sensor11, INPUT_PULLUP);
  pinMode(Sensor12, INPUT_PULLUP);

  pinMode(RelElectro1, OUTPUT);
  pinMode(RelElectro2, OUTPUT);
  pinMode(RelElectro3, OUTPUT);
  pinMode(RelElectro4, OUTPUT);
  pinMode(RelElectro5, OUTPUT);
  pinMode(RelElectro6, OUTPUT);
  pinMode(RelElectro7, OUTPUT);
  pinMode(RelElectro8, OUTPUT);

  digitalWrite(RelElectro1,LOW);
  digitalWrite(RelElectro2,LOW);
  digitalWrite(RelElectro3,LOW);
  digitalWrite(RelElectro4,LOW);
  digitalWrite(RelElectro5,LOW);
  digitalWrite(RelElectro6,LOW);
  digitalWrite(RelElectro7,LOW);
  digitalWrite(RelElectro8,LOW);
  
  debugTemps = millis();
  tempsIman1 = debugTemps;
}

void callback(char* topic, byte* payload, unsigned int length) {
  if (DEBUG) {
    Serial.print(F("Message arrived"));
    Serial.println(topic);
  }
  int res=strcmp(topic,"sala2/reset");
  if (res == 0) {    //RESET para toda la sala
     //wdt_enable(WDTO_15MS); // Configuramos el contador de tiempo para que se reinicie en 15ms      
     resetFunc();
  }
  res=strcmp(topic,"sala2/resetPedestal");
  if (res == 0) {    //RESET pedestal
     //wdt_enable(WDTO_15MS); // Configuramos el contador de tiempo para que se reinicie en 15ms      
     resetFunc();
  }
  res=strcmp(topic,"sala2/reiniciPedestal");
  if (res == 0) {    //REINICI Pedestal
     estat=0;
     //Variables per a controlar el temps de cada activació de sensor
     entra=false;
     entra1=false;
     entra2=false;
     entra3=false;
     entra4=false;
     entra5=false;
     entra6=false;
     final=false;

     marcaTemps0 = 0; //Marca de temps per al puzzle paret
     marcaTemps1 = 0; //Marca de temps per als sensors 1 i 2
     marcaTemps2 = 0; //Marca de temps per als sensors 3 i 4
     marcaTemps3 = 0; //Marca de temps per als sensors 5 i 6
     marcaTemps4 = 0; //Marca de temps per als sensors 7 i 8
     marcaTemps5 = 0; //Marca de temps per als sensors 9 i 10
     marcaTemps6 = 0; //Marca de temps per als sensors 11 i 12
     
    digitalWrite(RelElectro1,LOW);
    digitalWrite(RelElectro2,LOW);
    digitalWrite(RelElectro3,LOW);
    digitalWrite(RelElectro4,LOW);
    digitalWrite(RelElectro5,LOW);
    digitalWrite(RelElectro6,LOW);
    digitalWrite(RelElectro7,LOW);
    digitalWrite(RelElectro8,LOW);
  
    if (DEBUG) Serial.print(F("Reinici Pedestal"));
  }
 
  res=strcmp(topic,"sala2/estat1Pedestal");
  if (res == 0) {    //Primer Rele
     digitalWrite(RelElectro1,HIGH);
     estat=1;
  }
  res=strcmp(topic,"sala2/estat0Pedestal");
  if (res == 0) {    //RESET pedestal
     estat=0;
  }

  res = strcmp(topic,"sala2/reliquia1Pedestal");
  if (res==0) {     //OBRI primer relé per obrir comporta
    digitalWrite(RelElectro1,HIGH);
    estat=1;
  }

  res = strcmp(topic,"sala2/reliquia2Pedestal");
  if (res==0) {     //OBRI segon relé per obrir comporta i encendre llum
    if (DEBUG) Serial.println("Entra en reliquia2Pedestal");
    digitalWrite(RelElectro2,HIGH);
    estat=2;
  }

   res = strcmp(topic,"sala2/reliquia3Pedestal");
  if (res==0) {     //OBRI tercer relé per obrir comporta i encendre llum
    digitalWrite(RelElectro3,HIGH);
    estat=3;
  }

  res = strcmp(topic,"sala2/reliquia4Pedestal");
  if (res==0) {     //OBRI quart relé per obrir comporta i encendre llum
    digitalWrite(RelElectro4,HIGH);
    estat=4;
  }

  res = strcmp(topic,"sala2/reliquia5Pedestal");
  if (res==0) {     //OBRI quint relé per obrir comporta i encendre llum
    digitalWrite(RelElectro5,HIGH);
    estat=5;
  }

  res = strcmp(topic,"sala2/reliquia6Pedestal");
  if (res==0) {     //OBRI segon relé per encendre llum
    digitalWrite(RelElectro6,HIGH);
    estat=6;
  }
  
  res = strcmp(topic,"sala2/llumFinalPedestal");
  if (res==0) {     //OBRI segon relé per encendre llum
    digitalWrite(RelElectro8,HIGH);
    estat=7;
  }
}
 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    if (DEBUG) Serial.print(F("Attempting MQTT connection..."));
    // Attempt to connect
    if (client.connect("Pedestal1")) {
      //Serial.println(F("connected"));      

      if (DEBUG) Serial.print(F("Subscribe to reset resetPedestal reiniciPedestal estat0Pedestal estat1Pedestal"));
      client.subscribe("sala2/reset"); 
      client.subscribe("sala2/resetPedestal");  
      client.subscribe("sala2/reiniciPedestal");
      client.subscribe("sala2/estat0Pedestal");
      client.subscribe("sala2/estat1Pedestal");
      client.subscribe("sala2/reliquia1Pedestal");
      client.subscribe("sala2/reliquia2Pedestal");
      client.subscribe("sala2/reliquia3Pedestal");
      client.subscribe("sala2/reliquia4Pedestal");
      client.subscribe("sala2/reliquia5Pedestal");
      client.subscribe("sala2/reliquia6Pedestal");
      client.subscribe("sala2/llumFinalPedestal");
    
    } else {
      if (DEBUG) {
        Serial.print(F("failed, rc="));
        Serial.print(client.state());
        Serial.println(" try again in 1 second");
      }
      // Wait 1 seconds before retrying
      delay(1000);
    }
  }
}


void loop() {

  int value  = digitalRead(SensorPuzzle);
  int value1 = digitalRead(Sensor1);
  int value2 = digitalRead(Sensor2);
  int value3 = digitalRead(Sensor3);
  int value4 = digitalRead(Sensor4);
  int value5 = digitalRead(Sensor5);
  int value6 = digitalRead(Sensor6);
  int value7 = digitalRead(Sensor7);
  int value8 = digitalRead(Sensor8);
  int value9 = digitalRead(Sensor9);
  int value10 = digitalRead(Sensor10);
  int value11 = digitalRead(Sensor11);
  int value12 = digitalRead(Sensor12);

/*
  if (millis() > (tempsIman1 + 10000)) {
    if (DEBUG) Serial.println("RelElectro1 HIGH");
    digitalWrite(RelElectro1,HIGH); // Desactivem el electroiman4 per a que obriga comporta
    estat=1; 
  }
*/
  if (DEBUG && (millis() > (debugTemps + 500))) {
    debugTemps=millis();
    Serial.print("ESTAT ");
    Serial.println(estat);
    Serial.println("0 1 2 3 4 5 6 7 8 9 0 1 2");
    Serial.print(value);
    Serial.print(" ");
    Serial.print(value1);
    Serial.print(" ");
    Serial.print(value2);
    Serial.print(" ");
    Serial.print(value3);
    Serial.print(" ");
    Serial.print(value4);
    Serial.print(" ");
    Serial.print(value5);
    Serial.print(" ");
    Serial.print(value6);
    Serial.print(" ");
    Serial.print(value7);
    Serial.print(" ");
    Serial.print(value8);
    Serial.print(" ");
    Serial.print(value9);
    Serial.print(" ");
    Serial.print(value10);
    Serial.print(" ");
    Serial.print(value11);
    Serial.print(" ");
    Serial.print(value12);
    Serial.println(" ");  
  }

//SENSORS RELIQUIA1
if (value7 == 0 || value8 == 0) condicio10=true;
else condicio10=false;
if (value7 == 1 && value8 == 1) condicio11=true;
else condicio11=false;

//SENSORS RELIQUIA2
if (value11 == 0 || value12 == 0) condicio20=true;
else condicio20=false;
if (value11 == 1 && value12 == 1) condicio21=true;
else condicio21=false;

//SENSORS RELIQUIA3
if (value1 == 0 || value2 == 0) condicio30=true;
else condicio30=false;
if (value1 == 1 && value2 == 1) condicio31=true;
else condicio31=false;

//SENSORS RELIQUIA4
if (value9 == 0 || value10 == 0) condicio40=true;
else condicio40=false;
if (value9 == 1 && value10 == 1) condicio41=true;
else condicio41=false;

//SENSORS RELIQUIA5
if (value3 == 0 || value4 == 0) condicio50=true;
else condicio50=false;
if (value3 == 1 && value4 == 1) condicio51=true;
else condicio51=false;

//SENSORS RELIQUIA6
if (value5 == 0 || value6 == 0) condicio60=true;
else condicio60=false;
if (value5 == 1 && value6 == 1) condicio61=true;
else condicio61=false;

if (estat < 7) {

  if ( estat > 1 ){
    if (condicio10 ) digitalWrite(RelElectro2,HIGH);//Encenem llum
    else if (condicio11 ) digitalWrite(RelElectro2,LOW); //Apaguem llum
  }
  
  if (estat > 2 ) {
    if (condicio20) digitalWrite(RelElectro3,HIGH);
    else if (condicio21) digitalWrite(RelElectro3,LOW);
  }
  
  if ( estat > 3 ){
    if (condicio30 ) digitalWrite(RelElectro4,HIGH);
    else if (condicio31 ) digitalWrite(RelElectro4,LOW);
  }
  
  if (estat > 4 ) {
    if (condicio40) digitalWrite(RelElectro5,HIGH);
    else if (condicio41) digitalWrite(RelElectro5,LOW);
  }
  
  if ( estat > 5 ){
    if (condicio50 ) digitalWrite(RelElectro6,HIGH); //Apaguem llum
    else if (condicio51 ) digitalWrite(RelElectro6,LOW);//Encenem llum
  }
  
}

 switch(estat){    

    case 0: //Esperant puzzle1
      //if (DEBUG) Serial.println("ESTAT 0");
      if (value == 0 && !entra ){ //Comprovem si han superat el puzzle de la paret
        marcaTemps0=millis();
        entra=true;
      }
      if (value == 1 ){
        marcaTemps0=0;
        entra=false;
      }
      if (marcaTemps0 > 0 && (millis()-marcaTemps0 > 300)){
        if (DEBUG) Serial.println("RelElectro1 HIGH");
        digitalWrite(RelElectro1,HIGH); // Desactivem el electroiman4 per a que obriga comporta
        client.publish("sala2/activaPuzzle1","1");
        estat=1;        
      }      
      break;
      
    case 1: //Esperant relíquia1
      //if (DEBUG) Serial.println("ESTAT 1");
      if (condicio10 && !entra1){    //SENSORS 7 i 8
        marcaTemps1=millis();
        entra1=true;
      }
      if (condicio11){    //SENSORS 7 i 8
        marcaTemps1=0;
        entra1=false;
      }
      if (marcaTemps1 > 0 && (millis()-marcaTemps1 > 300)){
         if (DEBUG) Serial.println("RelElectro2 HIGH");
         digitalWrite(RelElectro2,HIGH); //Desactivem electroiman6 i encenem llum 4   
         client.publish("sala2/reliquia1","1");     
         estat=2;
      }else digitalWrite(RelElectro2,LOW); //Apaguem llum     
      break;             
    
    case 2: //Esperant relíquia2
                
      if (!condicio11 && condicio20 && !entra2){   //SENSORS 7 i 8 + 11 i 12
        marcaTemps2=millis();
        entra2=true;
      }
      if (condicio21){              //SENSORS 11 i 12
        marcaTemps2=0;
        entra2=false;
      }
      if (marcaTemps2 > 0 && (millis()-marcaTemps2  > 300)){
        if (DEBUG) Serial.println("RelElectro3 HIGH");
        digitalWrite(RelElectro3,HIGH); //Desactivem electroiman1 i encenem llum 6
        estat=3;
        //Enviem publicacio segona reliquia activada (ENCÉN LLUM BASCULA)
        client.publish("sala2/reliquia2","1");
        client.publish("sala2/activaBascula","ON");
        
      }else digitalWrite(RelElectro3,LOW);
      break;
    
    case 3:
     
      if (!condicio11 && !condicio21 && condicio30 && !entra3){     //SENSORS 1 i 2
        marcaTemps3=millis();
        entra3=true;
      }
      if (condicio31){                //SENSORS 1 i 2
        marcaTemps3=0;
        entra3=false;
      }
      if (marcaTemps3 > 0 && (millis()-marcaTemps3 > 300)){
         if (DEBUG) Serial.println("RelElectro4 HIGH");
        digitalWrite(RelElectro4,HIGH); //Desactivem electroiman5 i encenem llum 1
        estat=4;
        //Enviem publicacio tercera reliquia activada
        client.publish("sala2/reliquia3","1");
        client.publish("sala2/desactivaBascula","ON");
      }else digitalWrite(RelElectro4,LOW);
      break;

    case 4:
                  
      if (!condicio11 && !condicio21 && !condicio31 && condicio40 && !entra4){       //SENSORS 9 i 10
        marcaTemps4=millis();
        entra4=true;
      }
      if (condicio41){                   //SENSORS 9 i 10
        marcaTemps4=0;
        entra4=false;
      }
      if (marcaTemps4 > 0 && (millis()-marcaTemps4 > 300)){
         if (DEBUG) Serial.println("RelElectro5 HIGH");
        digitalWrite(RelElectro5,HIGH); //Desactivem electroiman3 i encenem llum 5
        client.publish("sala2/reliquia4","1");
        estat=5;
      }else digitalWrite(RelElectro5,LOW);
      break;

    case 5:
      
      if (!condicio11 && !condicio21 && !condicio31 && !condicio41 && condicio50 && !entra5){       //SENSORS 3 i 4
        marcaTemps5=millis();
        entra5=true;
      }
      if (condicio51){                    //SENSORS 3 i 4
        marcaTemps5=0;
        entra5=false;
      }
      if (marcaTemps5 > 0 && (millis()-marcaTemps5 > 300)){
        if (DEBUG) Serial.println("RelElectro6 HIGH");
        digitalWrite(RelElectro6,HIGH); //Desactivem electroiman6 i encenem llum 3
        //Enviem publicacio segona reliquia activada (ENCÉN LLUM BASCULA)
        client.publish("sala2/reliquia5","1");
        client.publish("sala2/activaOrgan","ON");
        estat=6;
      }else digitalWrite(RelElectro6,LOW);
      break;
    case 6:
    
      if (!condicio11 && !condicio21 && !condicio31 && !condicio41 && !condicio51 && condicio60 && !entra6){   //SENSORS 5 i 6
        marcaTemps6=millis();
        entra6=true;
      }
      if (condicio61){                //SENSORS 5 i 6
        marcaTemps6=0;
        entra6=false;
      }
      if (marcaTemps6 > 0 && (millis()-marcaTemps6 > 300)){
        if (DEBUG) Serial.println("RelElectro7 HIGH");
        digitalWrite(RelElectro7,HIGH); //Encenem llum 6
        client.publish("sala2/reliquia6","1");
        estat=7;
      }else digitalWrite(RelElectro7,LOW);
      break;

    case 7:

    if (DEBUG) {      
      Serial.println("ESTAT 7 ");
      Serial.println("condicio11 condicio21 condicio31 condicio41 condicio51 condicio61 final" );       
      Serial.print(condicio11);
      Serial.print(" ");
      Serial.print(condicio21);
      Serial.print(" ");
      Serial.print(condicio31);
      Serial.print(" ");
      Serial.print(condicio41);
      Serial.print(" ");
      Serial.print(condicio51);
      Serial.print(" ");
      Serial.print(condicio61);
      Serial.print(" ");
      Serial.println(final);
    }
  
      unsigned long ara=millis();

      //if (marcaTemps1 > 0 && marcaTemps2 > 0 && marcaTemps3 > 0 && marcaTemps4 > 0 && marcaTemps5 > 0 && marcaTemps6 > 0)
      //    if ( !final && (ara - marcaTemps1) > 500 && (ara - marcaTemps2) > 500 && (ara - marcaTemps3) > 500 && (ara - marcaTemps4) > 500 && (ara - marcaTemps5) > 500 && (ara - marcaTemps6) > 500 ) {
      if (!condicio11 && !condicio21 && !condicio31 && !condicio41 && !condicio51 && !condicio61 && !final && (ara - marcaTemps6) > 300){
             
         final=true;

         client.publish("sala2/finalPedestal","on");//Final Pedestal         
         
      }
      break;

  }
  
  if (!client.connected()) {
    reconnect();
  }  
  
  client.loop();

  if (final && !entraFinal) {
    
         delay(1000);
         digitalWrite(RelElectro7,LOW); //Apaguem totes les llums
         delay(500);
         digitalWrite(RelElectro6,LOW);
         delay(500);
         digitalWrite(RelElectro5,LOW);
         delay(500);
         digitalWrite(RelElectro4,LOW);
         delay(500);
         digitalWrite(RelElectro3,LOW);
         delay(500);
         digitalWrite(RelElectro2,LOW);             

         delay(3000);
         
         digitalWrite(RelElectro2,HIGH); //Encenem totes les llums
         //delay(500);
         digitalWrite(RelElectro3,HIGH);
         //delay(500);
         digitalWrite(RelElectro4,HIGH);
         //delay(500);
         digitalWrite(RelElectro5,HIGH);
         //delay(500);
         digitalWrite(RelElectro6,HIGH);
         //delay(500);
         digitalWrite(RelElectro7,HIGH);   
         //delay(500);
         
         if (DEBUG) Serial.println("RelElectro8 HIGH");
         digitalWrite(RelElectro8,HIGH); //Desactivem electroiman de dLOW i encenem la llum de dLOW

         entraFinal=true;
  }
   
}
