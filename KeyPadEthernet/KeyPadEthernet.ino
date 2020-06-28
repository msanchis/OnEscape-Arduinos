/*
  Actualitzat a dia 26/11/19
  Instalat 8/6/2020  
  KeyPadEthernet with RELAY, BUZZER AND LED

  TARJETA W5100 Ethernet
  Utilitza PINS 13, 12, 11 i 10 
  
  Comunicacion entre UNO y MQTT
    client.subscribe("sala1/apdirector", "ON");
    client.subscribe("sala1/cpdirector", "ON");
    client.publish("sala1/tpdirector", "123456"); //numero marcado
    client.publsih("sala1/padirector","ON");  //apertura puerta

  KEYPAD 4x4 ---> 9-8-7-6-5-4-3
  BUZZER o SPEAK ---> 2 (soltar quan conectem USB per SERIAL TX-RX)
  LED GREEN ---> A0  RED ---> A1
  RELAY ---> A2  

  PENDENT de programar: MODIFICAR ELS DELAYs PER millis()  --- FET
  
*/
#include <SPI.h>
#include <Ethernet.h> //Conexion Ethernet con carcasa W5100
#include <PubSubClient.h> //Conexion MQTT
#include <avr/wdt.h> // Incluir la librería de ATmel
#include <Keypad.h>

#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS 7 3729
#define NOTE_B7  3951

#define NOTE_A4 2000
#define NOTE_B4 2200
#define NOTE_C3 2000

int melody[] = {
  NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7
};

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {4, 3, 5};  
char keycount=0;
char code[5]; //Hold pressed keys
int LED_GREEN = A0;
int LED_RED = A1;
int LED_BLUE= A2;

int RELAY = A3;
int SPEAK = 2; 
int notes[] = {
  NOTE_A4, NOTE_B4, NOTE_C3, NOTE_E7
};

//initialize an instance of class NewKeypad
Keypad keypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0x66};//mac del arduino
//IPAddress ip(192, 168, 68, 15); //Ip fija del arduino
IPAddress server(192, 168, 68, 1); //Ip del server de mosquitto
//byte DNS[] = {8, 8, 8, 8};
//byte gateway[] = {192, 168, 1, 1};
//byte subnet[] = {255, 255, 255, 0};

EthernetClient ethClient; //Interfaz de red ethernet
PubSubClient client(ethClient); //cliente MQTT


char KEY[40] = {'1','3','6','2'}; // default secret key
char attempt[40] = {0,0,0,0};
int z=0;
const int tempsRelay = 2000; //temps en el que el relay està en funcionament (2 segs)
unsigned long marcaTemps = 0;
bool entra = false;
void setup(){
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY,HIGH); 
  Serial.begin(9600);
  Serial.println("inicia");

  //MQTT
  client.setServer(server, 1883);
  client.setCallback(callback);

   Ethernet.begin(mac);   
   //Ethernet.begin(mac,ip);
   //Ethernet.begin(mac, ip, DNS, gateway, subnet);    
   delay(1000);
   Serial.println(F("connecting..."));
   Serial.println(Ethernet.localIP());
 
}

void loop(){
   readKeypad();

  //Condició per a cambiar l'estat del relé, és a dir, tallar la corrent de l'apertura de porta
   if ( entra && (millis() - marcaTemps > tempsRelay) ){
     entra=false;
     digitalWrite(RELAY,HIGH);  
   }
   
   //Comunicació en serial entre el UNO y Consola
   if (Serial.available()){ 
    //String c = BT.readString() ;
    String c = Serial.readString();
    //Serial.println("LLEGA ALGO POR SERIE");
    Serial.println(c);
    //Serial.println("YA ESTA");
    String abrir="A";
    String cerrar="C";
    if ( abrir.charAt(0) == c.charAt(0)){
      marcaTemps=millis();
      digitalWrite(RELAY,LOW);
      entra=true;      
      //delay(tempsRelay); //temps en el que el relay està en funcionament
      //digitalWrite(RELAY,HIGH);  
    }else if (cerrar.charAt(0) == c.charAt(0)){
      digitalWrite( RELAY, HIGH);      
    }
   }
   if (!entra && !client.connected()) {
      reconnect();
   }
   client.loop();
}

void Apaga()
{
  delay(200);
  digitalWrite(LED_GREEN,LOW);
  digitalWrite(LED_RED,LOW);
  digitalWrite(LED_BLUE,LOW);
}

void Roig()
{
  digitalWrite(LED_GREEN,LOW);
  digitalWrite(LED_BLUE,LOW);
  digitalWrite(LED_RED,HIGH);
  Apaga();
}

void Blau()
{
  digitalWrite(LED_GREEN,LOW);
  digitalWrite(LED_RED,LOW);
  digitalWrite(LED_BLUE,HIGH);
  Apaga();
}

void Verd()
{
  digitalWrite(LED_GREEN,HIGH);
  digitalWrite(LED_BLUE,LOW);
  digitalWrite(LED_RED,LOW);
  Apaga();
}
void correctKEY() // do this if the correct KEY is entered
{
   
  Serial.println(" KEY ACCEPTED...");
  Verd();   
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 5; thisNote++) {
    int noteDuration = 200;
    tone(SPEAK, melody[thisNote], noteDuration);
    int pauseBetweenNotes = 100;
    delay(pauseBetweenNotes);
  }
  Verd();
    
  marcaTemps=millis();
  digitalWrite(RELAY,LOW);
  entra=true;  
  z=0;
  
  client.publish("sala1/padirector","ON");
}
void incorrectKEY() // do this if an incorrect KEY is entered
{
   //Serial.println("KEY REJECTED!");
   Roig();
   tone(SPEAK, notes[2], 200);
   delay(250);
   tone(SPEAK, notes[2], 200);
   delay(250);
   tone(SPEAK, notes[2], 200);
   delay(250);
   
}

void clearAttempt(){
  for (int zz=0; zz<40; zz++) // clear previous key input
   {
      attempt[zz]=0;
   }
}

void checkKEY()
{
   int correct=0;
   int incorrecto=0;
   int i;
   for ( i = 0; i < z ; i++ )
   {
      if (attempt[i]==KEY[i]){
         correct++;
      }else{
        incorrecto++;
      }
   }
   String cod = String(attempt);
   Serial.println(attempt);
   client.publish("sala1/tpdirector",(char *)cod.c_str());
   //Serial.print(cod);
   if (correct==4 && incorrecto==0){
      correctKEY();
   }else{
      incorrectKEY();
   }     
}

void readKeypad()
{
   char key = keypad.getKey();
   
   if (key != NO_KEY)
   {  
      
      Serial.println(key);      
      switch(key)
      {
      case '*':      
         tone(SPEAK, notes[3], 200);
         z=0;
         //Verd();
         //Roig();
         Blau();
         clearAttempt();
         break;
      case '#':
         delay(100); // added debounce                                               
         checkKEY();
         z=0;
         clearAttempt();
         break;
      default:
         tone(SPEAK, notes[2], 200);
         attempt[z]=key;
         if (attempt[z]==KEY[z]){
          Verd();
         }else{
          Roig();
         }
         z++;
      }
   }
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print(F("Message arrived ["));
  Serial.print(topic);  
  Serial.print(F("] "));
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }  
  
  int res=strcmp(topic,"sala1/abrirpuertadirector");
  if (res == 0) {
    marcaTemps=millis();
    digitalWrite(RELAY,LOW);
    entra=true;  
  }
  res=strcmp(topic,"sala1/cerrarpuertadirector");
  if (res == 0) {    
    digitalWrite(RELAY,HIGH);
  }
  res=strcmp(topic,"sala1/reset");
  int resu = strcmp(topic,"sala1/resetKeyPad");  
  if (res == 0 || resu == 0) { //RESET PLACA
     wdt_enable(WDTO_15MS); // Configuramos el contador de tiempo para que se reinicie en 15ms
  }
  
}

void reconnect() {

  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print(F("Attempting MQTT connection..."));
    // Attempt to connect
    if (client.connect("KeyPad")) {
      Serial.println(F("connected"));      
      client.subscribe("sala1/abrirpuertadirector");
      client.subscribe("sala1/cerrarpuertadirector");
      client.subscribe("sala1/reset");
      client.subscribe("sala1/resetKeyPad");     
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(client.state());
      //Serial.println(" try again in 5 seconds");
      // Wait 3 seconds before retrying
      delay(3000);
    }
  }
}
